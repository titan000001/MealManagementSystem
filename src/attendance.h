#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>
#include <vector>

struct MealAttendance {
    int user_id;
    std::string date;
    std::string meal_type; // Breakfast, Lunch, Dinner
};

bool recordAttendance(int user_id, const std::string& date, const std::string& meal_type);
std::vector<MealAttendance> getAttendanceByDate(const std::string& date);
std::vector<MealAttendance> getAttendanceByUser(int user_id);

#endif // ATTENDANCE_H
