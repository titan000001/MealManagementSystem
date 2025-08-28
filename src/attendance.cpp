#include "attendance.h"
#include "user.h"
#include "database.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>

bool recordAttendance(int user_id, const std::string& date, const std::string& meal_type) {
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO meal_attendance (user_id, attendance_date, meal_type) VALUES (?, STR_TO_DATE(?, '%Y-%m-%d'), ?)")
        );
        pstmt->setInt(1, user_id);
        pstmt->setString(2, date);
        pstmt->setString(3, meal_type);
        pstmt->execute();
        return true;
    } catch (sql::SQLException& e) {
        if (e.getErrorCode() == 1062) { // ER_DUP_ENTRY
            std::cerr << "Error: Attendance for this user and meal has already been recorded." << std::endl;
        } else {
            std::cerr << "SQL Error in recordAttendance: " << e.what() << std::endl;
        }
        return false;
    }
}

// Stub for the next function
std::vector<MealAttendance> getAttendanceForDate(const std::string& date) {
    std::vector<MealAttendance> attendanceList;
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT ma.user_id, ma.meal_type, u.name AS user_name "
                "FROM meal_attendance ma "
                "JOIN users u ON ma.user_id = u.id "
                "WHERE ma.attendance_date = STR_TO_DATE(?, '%Y-%m-%d') "
                "ORDER BY ma.meal_type, u.name"
            )
        );
        pstmt->setString(1, date);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            MealAttendance attendance;
            attendance.user_id = res->getInt("user_id");
            attendance.user_name = res->getString("user_name");
            attendance.meal_type = res->getString("meal_type");
            attendanceList.push_back(attendance);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in getAttendanceForDate: " << e.what() << std::endl;
    }
    return attendanceList;
}

bool addMultipleAttendance(const std::string& date, const std::vector<AttendanceRecord>& records) {
    if (records.empty()) {
        return true;
    }
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        std::string query = "INSERT INTO meal_attendance (user_id, attendance_date, meal_type) VALUES ";
        for (size_t i = 0; i < records.size(); ++i) {
            query += "(?, STR_TO_DATE(?, '%Y-%m-%d'), ?)";
            if (i < records.size() - 1) {
                query += ", ";
            }
        }
        // Add ON DUPLICATE KEY UPDATE to ignore errors if a record already exists
        query += " ON DUPLICATE KEY UPDATE user_id=user_id";

        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(query));
        int paramIndex = 1;
        for (const auto& record : records) {
            pstmt->setInt(paramIndex++, record.user_id);
            pstmt->setString(paramIndex++, date);
            pstmt->setString(paramIndex++, record.meal_type);
        }
        pstmt->execute();
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in addMultipleAttendance: " << e.what() << std::endl;
        return false;
    }
}

bool deleteMultipleAttendance(const std::string& date, const std::vector<AttendanceRecord>& records) {
    if (records.empty()) {
        return true;
    }
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        // Build a query like: DELETE FROM ... WHERE (user_id, meal_type) IN ((?,?), (?,?)) AND attendance_date = ?
        std::string query = "DELETE FROM meal_attendance WHERE (user_id, meal_type) IN (";
        for (size_t i = 0; i < records.size(); ++i) {
            query += "(?, ?)";
            if (i < records.size() - 1) query += ",";
        }
        query += ") AND attendance_date = STR_TO_DATE(?, '%Y-%m-%d')";

        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(query));
        int paramIndex = 1;
        for (const auto& record : records) {
            pstmt->setInt(paramIndex++, record.user_id);
            pstmt->setString(paramIndex++, record.meal_type);
        }
        pstmt->setString(paramIndex, date);
        pstmt->executeUpdate();
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in deleteMultipleAttendance: " << e.what() << std::endl;
        return false;
    }
}