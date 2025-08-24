#include "period.h"
#include "database.h"
#include <iostream>
#include <memory>

bool setupMealPeriod(const std::string& month, const std::string& year) {
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string query = "INSERT INTO meal_periods (month, year) VALUES ('" + month + "', '" + year + "')";
        stmt->execute(query);
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        return false;
    }
}
