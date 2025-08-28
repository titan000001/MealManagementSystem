#ifndef PERIOD_H
#define PERIOD_H

#include <string>
#include <vector>

struct MealPeriod {
    int id;
    std::string month;
    std::string year;
};

bool setupMealPeriod(const std::string& month, const std::string& year);
std::vector<MealPeriod> getAllMealPeriods();

#endif // PERIOD_H
