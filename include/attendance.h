#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>
#include <vector>
#include "user.h" // For User struct

struct MealAttendance {
    int user_id;
    std::string user_name;
    std::string meal_type;
};

struct AttendanceRecord {
    int user_id;
    std::string meal_type;
};

bool recordAttendance(int user_id, const std::string& date, const std::string& meal_type);
std::vector<MealAttendance> getAttendanceForDate(const std::string& date);

bool addMultipleAttendance(const std::string& date, const std::vector<AttendanceRecord>& records);
bool deleteMultipleAttendance(const std::string& date, const std::vector<AttendanceRecord>& records);

#endif // ATTENDANCE_H