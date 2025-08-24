#include "attendance.h"
#include "database.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>

bool recordAttendance(int user_id, const std::string& date, const std::string& meal_type) {
    try {
        sql::Connection* con = getConnection();
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
        sql::Connection* con = getConnection();
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