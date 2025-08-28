#include "expense.h"
#include "user.h"
#include "database.h"
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <iostream>
#include <memory>
#include <vector>

bool addExpense(const std::string& purchase_date, const std::string& item_name, double price, int paid_by_user_id, const std::string& category) {
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        // Using STR_TO_DATE to convert the string date from the user to a SQL DATE type
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO expenses (purchase_date, item_name, price, paid_by_user_id, category) VALUES (STR_TO_DATE(?, '%Y-%m-%d'), ?, ?, ?, ?)")
        );
        pstmt->setString(1, purchase_date);
        pstmt->setString(2, item_name);
        pstmt->setDouble(3, price);
        pstmt->setInt(4, paid_by_user_id);
        pstmt->setString(5, category);
        pstmt->execute();
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in addExpense: " << e.what() << std::endl;
        return false;
    }
}

// Stubs for other functions declared in expense.h
// These can be implemented later.

bool editExpense(int id, const std::string& item_name, double price, const std::string& category) {
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("UPDATE expenses SET item_name = ?, price = ?, category = ? WHERE id = ?")
        );
        pstmt->setString(1, item_name);
        pstmt->setDouble(2, price);
        pstmt->setString(3, category);
        pstmt->setInt(4, id);
        return pstmt->executeUpdate() > 0; // Returns true if a row was updated
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in editExpense: " << e.what() << std::endl;
        return false;
    }
}
bool deleteExpense(int id) {
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("DELETE FROM expenses WHERE id = ?")
        );
        pstmt->setInt(1, id);
        return pstmt->executeUpdate() > 0; // Returns true if a row was deleted
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in deleteExpense: " << e.what() << std::endl;
        return false;
    }
}
std::vector<Expense> getAllExpenses() {
    std::vector<Expense> expenses;
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        // Join with the users table to get the name of the person who paid
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT e.id, DATE_FORMAT(e.purchase_date, '%Y-%m-%d') AS purchase_date, e.item_name, e.price, e.category, u.name AS paid_by_user_name "
            "FROM expenses e JOIN users u ON e.paid_by_user_id = u.id "
            "ORDER BY e.purchase_date DESC, e.id DESC"
        ));

        while (res->next()) {
            Expense expense;
            expense.id = res->getInt("id");
            expense.purchase_date = res->getString("purchase_date");
            expense.item_name = res->getString("item_name");
            expense.price = res->getDouble("price");
            expense.paid_by_user_name = res->getString("paid_by_user_name");
            expense.category = res->getString("category");
            expenses.push_back(expense);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in getAllExpenses: " << e.what() << std::endl;
    }
    return expenses;
}
std::vector<Expense> getExpensesByCategory(const std::string& category) {
    std::vector<Expense> expenses;
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT e.id, DATE_FORMAT(e.purchase_date, '%Y-%m-%d') AS purchase_date, e.item_name, e.price, e.category, u.name AS paid_by_user_name "
                "FROM expenses e JOIN users u ON e.paid_by_user_id = u.id "
                "WHERE e.category = ? "
                "ORDER BY e.purchase_date DESC, e.id DESC"
            )
        );
        pstmt->setString(1, category);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            Expense expense;
            expense.id = res->getInt("id");
            expense.purchase_date = res->getString("purchase_date");
            expense.item_name = res->getString("item_name");
            expense.price = res->getDouble("price");
            expense.paid_by_user_name = res->getString("paid_by_user_name");
            expense.category = res->getString("category");
            expenses.push_back(expense);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in getExpensesByCategory: " << e.what() << std::endl;
    }
    return expenses;
}