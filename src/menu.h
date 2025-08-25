#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

struct MenuItem {
    int id;
    std::string name;
};

struct DailyMenu {
    std::string date;
    std::vector<MenuItem> breakfast;
    std::vector<MenuItem> lunch;
    std::vector<MenuItem> dinner;
};

bool addMenuItem(const std::string& name);
bool editMenuItem(int id, const std::string& name);
bool deleteMenuItem(int id);
std::vector<MenuItem> getAllMenuItems();
bool setDailyMenu(const std::string& date, const std::vector<int>& breakfastItems, const std::vector<int>& lunchItems, const std::vector<int>& dinnerItems);
DailyMenu getDailyMenu(const std::string& date);
std::vector<DailyMenu> getMenuHistory();

#endif // MENU_H
