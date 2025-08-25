#include "finance.h"
#include "database.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>

bool recordPayment(int user_id, double amount, const std::string& date) {
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO payments(user_id, amount, date) VALUES(?, ?, STR_TO_DATE(?, '%Y-%m-%d'))"));
        pstmt->setInt(1, user_id);
        pstmt->setDouble(2, amount);
        pstmt->setString(3, date);
        pstmt->executeUpdate();
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in recordPayment: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Payment> getPaymentsByUser(int user_id) {
    std::vector<Payment> payments;
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT id, amount, date FROM payments WHERE user_id = ?"));
        pstmt->setInt(1, user_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        while (res->next()) {
            Payment p;
            p.id = res->getInt("id");
            p.user_id = user_id;
            p.amount = res->getDouble("amount");
            p.date = res->getString("date");
            payments.push_back(p);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in getPaymentsByUser: " << e.what() << std::endl;
    }
    return payments;
}

FinancialReport getUserFinancialReport(int user_id) {
    FinancialReport report = {user_id, "", 0.0, 0.0, 0.0};
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "SELECT u.name, COALESCE(SUM(p.amount), 0) AS total_payments, COALESCE(SUM(e.price), 0) AS total_expenses "
            "FROM users u "
            "LEFT JOIN payments p ON u.id = p.user_id "
            "LEFT JOIN expenses e ON u.id = e.paid_by_user_id "
            "WHERE u.id = ? "
            "GROUP BY u.id, u.name"
        ));
        pstmt->setInt(1, user_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            report.user_name = res->getString("name");
            report.total_contributions = res->getDouble("total_payments");
            report.total_expenses = res->getDouble("total_expenses");
            report.debt_or_surplus = report.total_contributions - report.total_expenses;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in getUserFinancialReport: " << e.what() << std::endl;
    }
    return report;
}

std::vector<FinancialReport> getAllFinancialReports() {
    std::vector<FinancialReport> reports;
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT u.id, u.name, COALESCE(p.total_payments, 0) AS total_payments, COALESCE(e.total_expenses, 0) AS total_expenses "
            "FROM users u "
            "LEFT JOIN (SELECT user_id, SUM(amount) AS total_payments FROM payments GROUP BY user_id) p ON u.id = p.user_id "
            "LEFT JOIN (SELECT paid_by_user_id, SUM(price) AS total_expenses FROM expenses GROUP BY paid_by_user_id) e ON u.id = e.paid_by_user_id"
        ));

        while (res->next()) {
            FinancialReport report;
            report.user_id = res->getInt("id");
            report.user_name = res->getString("name");
            report.total_contributions = res->getDouble("total_payments");
            report.total_expenses = res->getDouble("total_expenses");
            report.debt_or_surplus = report.total_contributions - report.total_expenses;
            reports.push_back(report);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in getAllFinancialReports: " << e.what() << std::endl;
    }
    return reports;
}

std::pair<double, std::vector<SettlementReport>> generateMonthlySettlement(int period_id) {
    double meal_rate = 0.0;
    std::vector<SettlementReport> reports;
    std::string month, year;

    try {
        sql::Connection* con = getConnection();

        // 1. Get the month and year for the selected period
        std::unique_ptr<sql::PreparedStatement> pstmt_period(con->prepareStatement("SELECT month, year FROM meal_periods WHERE id = ?"));
        pstmt_period->setInt(1, period_id);
        std::unique_ptr<sql::ResultSet> res_period(pstmt_period->executeQuery());
        if (!res_period->next()) {
            std::cerr << "Error: Meal period with ID " << period_id << " not found." << std::endl;
            return {meal_rate, reports};
        }
        month = res_period->getString("month");
        year = res_period->getString("year");

        // 2. Calculate total expenses for the period
        double total_expenses_period = 0.0;
        std::unique_ptr<sql::PreparedStatement> pstmt_exp(con->prepareStatement("SELECT COALESCE(SUM(price), 0) AS total FROM expenses WHERE MONTHNAME(purchase_date) = ? AND YEAR(purchase_date) = ?"));
        pstmt_exp->setString(1, month);
        pstmt_exp->setString(2, year);
        std::unique_ptr<sql::ResultSet> res_exp(pstmt_exp->executeQuery());
        if (res_exp->next()) {
            total_expenses_period = res_exp->getDouble("total");
        }

        // 3. Calculate total meals for the period
        int total_meals_period = 0;
        std::unique_ptr<sql::PreparedStatement> pstmt_meals(con->prepareStatement("SELECT COUNT(*) AS total FROM meal_attendance WHERE MONTHNAME(attendance_date) = ? AND YEAR(attendance_date) = ?"));
        pstmt_meals->setString(1, month);
        pstmt_meals->setString(2, year);
        std::unique_ptr<sql::ResultSet> res_meals(pstmt_meals->executeQuery());
        if (res_meals->next()) {
            total_meals_period = res_meals->getInt("total");
        }

        // 4. Calculate meal rate
        if (total_meals_period > 0) {
            meal_rate = total_expenses_period / total_meals_period;
        }

        // 5. Get data for all users and calculate their individual reports
        std::unique_ptr<sql::PreparedStatement> pstmt_users(con->prepareStatement(
            "SELECT u.id, u.name, "
            "COALESCE(att.meal_count, 0) AS total_meals, "
            "COALESCE(pay.total_payments, 0) AS total_payments, "
            "COALESCE(exp.total_shopping, 0) AS total_shopping "
            "FROM users u "
            "LEFT JOIN (SELECT user_id, COUNT(*) as meal_count FROM meal_attendance WHERE MONTHNAME(attendance_date) = ? AND YEAR(attendance_date) = ? GROUP BY user_id) att ON u.id = att.user_id "
            "LEFT JOIN (SELECT user_id, SUM(amount) as total_payments FROM payments WHERE MONTHNAME(date) = ? AND YEAR(date) = ? GROUP BY user_id) pay ON u.id = pay.user_id "
            "LEFT JOIN (SELECT paid_by_user_id, SUM(price) as total_shopping FROM expenses WHERE MONTHNAME(purchase_date) = ? AND YEAR(purchase_date) = ? GROUP BY paid_by_user_id) exp ON u.id = exp.paid_by_user_id "
            "ORDER BY u.id"
        ));
        // Bind parameters for all subqueries
        pstmt_users->setString(1, month);
        pstmt_users->setString(2, year);
        pstmt_users->setString(3, month);
        pstmt_users->setString(4, year);
        pstmt_users->setString(5, month);
        pstmt_users->setString(6, year);

        std::unique_ptr<sql::ResultSet> res_users(pstmt_users->executeQuery());
        while (res_users->next()) {
            SettlementReport report;
            report.user_id = res_users->getInt("id");
            report.user_name = res_users->getString("name");
            report.total_meals = res_users->getInt("total_meals");
            report.total_payments = res_users->getDouble("total_payments");
            report.total_shopping_expenses = res_users->getDouble("total_shopping");

            // Perform final calculations
            report.total_meal_cost = report.total_meals * meal_rate;
            report.total_contributions = report.total_payments + report.total_shopping_expenses;
            report.final_balance = report.total_contributions - report.total_meal_cost;

            reports.push_back(report);
        }

    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in generateMonthlySettlement: " << e.what() << std::endl;
    }

    return {meal_rate, reports};
}