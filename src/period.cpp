#include "period.h"
#include "database.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <memory>
#include <vector>

bool setupMealPeriod(const std::string& month, const std::string& year) {
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO meal_periods (month, year) VALUES (?, ?)"));
        pstmt->setString(1, month);
        pstmt->setString(2, year);
        pstmt->execute();
        return true;
    } catch (sql::SQLException& e) {
        if (e.getErrorCode() == 1062) { // ER_DUP_ENTRY for `period_unique` constraint
            std::cerr << "Error: Meal period for " << month << " " << year << " already exists." << std::endl;
        } else {
            std::cerr << "SQL Error in setupMealPeriod: " << e.what() << std::endl;
        }
        return false;
    }
}

std::vector<MealPeriod> getAllMealPeriods() {
    std::vector<MealPeriod> periods;
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT id, month, year FROM meal_periods ORDER BY year DESC, month DESC"));
        while (res->next()) {
            MealPeriod period;
            period.id = res->getInt("id");
            period.month = res->getString("month");
            period.year = res->getString("year");
            periods.push_back(period);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in getAllMealPeriods: " << e.what() << std::endl;
    }
    return periods;
}
