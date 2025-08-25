#include "settings.h"
#include "database.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>

SystemSettings getSystemSettings() {
    SystemSettings settings;
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT currency FROM settings WHERE id = 1"));
        if (res->next()) {
            settings.currency = res->getString("currency");
        } else {
            settings.currency = "USD"; // Default
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in getSystemSettings: " << e.what() << std::endl;
    }
    return settings;
}

bool updateSystemSettings(const SystemSettings& settings) {
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE settings SET currency = ? WHERE id = 1"));
        pstmt->setString(1, settings.currency);
        pstmt->executeUpdate();
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in updateSystemSettings: " << e.what() << std::endl;
        return false;
    }
}