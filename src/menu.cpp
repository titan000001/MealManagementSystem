#include "menu.h"
#include "database.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <memory>
#include <vector>

bool addMenuItem(const std::string& name) {
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO menu_items (name) VALUES (?)")
        );
        pstmt->setString(1, name);
        pstmt->execute();
        return true;
    } catch (sql::SQLException& e) {
        // Handle unique constraint violation gracefully
        if (e.getErrorCode() == 1062) { // 1062 = ER_DUP_ENTRY
            std::cerr << "Error: Menu item '" << name << "' already exists." << std::endl;
        } else {
            std::cerr << "SQL Error in addMenuItem: " << e.what() << std::endl;
        }
        return false;
    }
}

// Stubs for other functions to be implemented later
bool editMenuItem(int id, const std::string& name) { return false; }
bool deleteMenuItem(int id) { return false; }
std::vector<MenuItem> getAllMenuItems() {
    std::vector<MenuItem> items;
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery("SELECT id, name FROM menu_items ORDER BY name ASC")
        );

        while (res->next()) {
            MenuItem item;
            item.id = res->getInt("id");
            item.name = res->getString("name");
            items.push_back(item);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in getAllMenuItems: " << e.what() << std::endl;
    }
    return items;
}

bool setDailyMenu(const std::string& date, const std::vector<MenuItem>& breakfast, const std::vector<MenuItem>& lunch, const std::vector<MenuItem>& dinner) {
    sql::Connection* con = getConnection();
    try {
        // Start a transaction for atomicity
        con->setAutoCommit(false);

        // First, delete any existing menu for this date to allow for easy updates.
        std::unique_ptr<sql::PreparedStatement> del_pstmt(
            con->prepareStatement("DELETE FROM daily_menus WHERE menu_date = STR_TO_DATE(?, '%Y-%m-%d')")
        );
        del_pstmt->setString(1, date);
        del_pstmt->execute();

        // Prepare the insert statement once
        std::unique_ptr<sql::PreparedStatement> ins_pstmt(
            con->prepareStatement("INSERT INTO daily_menus (menu_date, meal_type, menu_item_id) VALUES (STR_TO_DATE(?, '%Y-%m-%d'), ?, ?)")
        );

        // Helper lambda to insert items for a meal type
        auto insertItems = [&](const std::vector<MenuItem>& items, const std::string& mealType) {
            for (const auto& item : items) {
                ins_pstmt->setString(1, date);
                ins_pstmt->setString(2, mealType);
                ins_pstmt->setInt(3, item.id);
                ins_pstmt->execute();
            }
        };

        insertItems(breakfast, "Breakfast");
        insertItems(lunch, "Lunch");
        insertItems(dinner, "Dinner");

        // If all inserts were successful, commit the transaction
        con->commit();
        con->setAutoCommit(true); // Restore default behavior
        return true;

    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in setDailyMenu: " << e.what() << std::endl;
        // An error occurred, roll back the entire transaction
        con->rollback();
        con->setAutoCommit(true); // Restore default behavior
        return false;
    }
}

DailyMenu getDailyMenu(const std::string& date) {
    return {};
}
std::vector<DailyMenu> getMenuHistory() {
    return {};
}