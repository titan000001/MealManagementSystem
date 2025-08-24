#include "period.h"
#include "database.h"
#include <cppconn/prepared_statement.h> // Include for PreparedStatement definition
#include <iostream>
#include <memory>

bool setupMealPeriod(const std::string& month, const std::string& year) {
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO meal_periods (month, year) VALUES (?, ?)"));
        pstmt->setString(1, month);
        pstmt->setString(2, year);
        pstmt->execute();
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        return false;
    }
}
