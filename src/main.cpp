#include <iostream>
#include "database.h"
#include "user.h"
#include "menu.h"
#include "expense.h"
#include "attendance.h"
#include "period.h"
#include "finance.h"
#include "settings.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <memory>
#include <limits>
#include <iomanip>
#include <sstream>
#include <regex>

bool isValidDate(const std::string& date) {
    const std::regex pattern("^(\\d{4})-(\\d{2})-(\\d{2})$");
    std::smatch matches;
    if (!std::regex_match(date, matches, pattern)) {
        return false;
    }

    try {
        int year = std::stoi(matches[1].str());
        int month = std::stoi(matches[2].str());
        int day = std::stoi(matches[3].str());

        if (month < 1 || month > 12 || day < 1 || day > 31) {
            return false;
        }

        // Check for months with 30 days
        if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
            return false;
        }

        // Check for February and leap years
        if (month == 2) {
            bool isLeap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
            if (isLeap && day > 29) {
                return false;
            }
            if (!isLeap && day > 28) {
                return false;
            }
        }
    } catch (const std::out_of_range& oor) {
        // Year, month, or day is too large to be an int
        return false;
    }

    return true;
}

// Helper function for robust integer input to prevent crashes on non-numeric input.
int getIntegerInput() {

    int value;
    std::cin >> value;
    while (std::cin.fail()) {
        std::cout << "Invalid input. Please enter a whole number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> value;
    }
    return value;
}

// Helper function for robust double input.
double getDoubleInput() {
    double value;
    std::cin >> value;
    while (std::cin.fail()) {
        std::cout << "Invalid input. Please enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> value;
    }
    return value;
}

void displayUser(const User& user) {
    std::cout << "ID: " << user.id << std::endl;
    std::cout << "Username: " << user.username << std::endl;
    std::cout << "Name: " << user.name << std::endl;
    std::cout << "Role: " << roleToString(user.role) << std::endl;
}

void handleUserManagement(const User& currentUser) {
    int userChoice = -1;
    while (userChoice != 0) {
        std::cout << "\n--- User Management ---" << std::endl;
        std::cout << "1. View My Profile" << std::endl;
        std::cout << "2. Update My Name" << std::endl;
        if (currentUser.role == UserRole::Admin) {
            std::cout << "3. Register a New User" << std::endl;
            std::cout << "4. View Another User's Profile" << std::endl;
            std::cout << "5. View All Users" << std::endl;
        }
        std::cout << "0. Back" << std::endl;
        std::cout << "Enter your choice: ";
        userChoice = getIntegerInput();
        switch (userChoice) {
            case 3: { // Register User (Admin Only)
                if (currentUser.role != UserRole::Admin) { std::cout << "Invalid choice." << std::endl; break; }
                std::string username, password, name;
                int roleInt;
                std::cout << "Enter username: "; std::cin >> username;
                std::cout << "Enter password: "; std::cin >> password;
                std::cout << "Enter full name: ";
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume the leftover newline
                std::getline(std::cin, name);

                UserRole role = UserRole::Student; // Initialize to a safe default.
                bool roleIsValid = false;
                std::cout << "Select role (0: Student, 1: Staff, 2: Admin): ";
                roleInt = getIntegerInput();
                
                // Use a switch statement for a more robust and explicit mapping from int to UserRole.
                // This avoids potential issues with static_cast.
                switch (roleInt) {
                    case 0: role = UserRole::Student; roleIsValid = true; break;
                    case 1: role = UserRole::Staff;   roleIsValid = true; break;
                    case 2: role = UserRole::Admin;   roleIsValid = true; break;
                    default:
                        std::cout << "Invalid role selected." << std::endl;
                        break;
                }

                if (!roleIsValid) {
                    std::cout << "Invalid role selected." << std::endl;
                    break;
                }
                if (registerUser(username, password, name, role)) {
                    std::cout << "User registered successfully!" << std::endl;
                } else {
                    std::cout << "User registration failed." << std::endl;
                }
                break;
            }
            case 4: { // View Another User's Profile (Admin Only)
                if (currentUser.role != UserRole::Admin) { std::cout << "Invalid choice." << std::endl; break; }
                int id;
                std::cout << "Enter user ID: "; id = getIntegerInput();
                auto user = getUserById(id);
                if (user) {
                    std::cout << "User Profile:" << std::endl;
                    displayUser(*user);
                } else {
                    std::cout << "User not found." << std::endl;
                }
                break;
            }
            case 1: { // View My Profile
                std::cout << "\n--- Your Profile ---" << std::endl;
                displayUser(currentUser);
                break;
            }
            case 2: { // Update My Name
                std::string newName;
                std::cout << "Enter new full name: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume the leftover newline
                std::getline(std::cin, newName);
                if (updateUserProfile(currentUser.id, newName)) {
                    std::cout << "Profile updated successfully!" << std::endl;
                } else {
                    std::cout << "Profile update failed." << std::endl;
                }
                break;
            }
            case 5: {
                if (currentUser.role != UserRole::Admin) { std::cout << "Invalid choice." << std::endl; break; }
                std::vector<User> users = getAllUsers();
                if (users.empty()) {
                    std::cout << "No users found." << std::endl;
                } else {
                    std::cout << "\n--- All Registered Users ---" << std::endl;
                    std::cout << std::left << std::setw(5) << "ID"
                              << std::setw(20) << "Username"
                              << std::setw(30) << "Full Name"
                              << "Role" << std::endl;
                    std::cout << std::string(70, '-') << std::endl;
                    for (const auto& user : users) {
                        std::cout << std::left << std::setw(5) << user.id
                                  << std::setw(20) << user.username
                                  << std::setw(30) << user.name
                                  << roleToString(user.role) << std::endl;
                    }
                }
                break;
            }
            case 0:
                std::cout << "Returning to main menu..." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

void handlePeriodManagement(const User& currentUser) {
    if (currentUser.role != UserRole::Admin) {
        std::cout << "Access Denied. Only Admins can manage meal periods." << std::endl;
        return;
    }
                    int periodChoice = -1;
                    while(periodChoice != 0) {
                        std::cout << "\n--- Monthly Meal Period Management ---" << std::endl;
                        std::cout << "1. Setup New Meal Period" << std::endl;
                        std::cout << "2. View All Meal Periods" << std::endl;
                        std::cout << "0. Back" << std::endl;
                        std::cout << "Enter your choice: ";
                        periodChoice = getIntegerInput();

                        switch(periodChoice) {
                            case 1: {
                                std::string month, year;
                                std::cout << "Enter month (e.g., August): ";
                                std::cin >> month;
                                std::cout << "Enter year (e.g., 2025): ";
                                std::cin >> year;
                                if (setupMealPeriod(month, year)) {
                                    std::cout << "Meal period setup successful for " << month << " " << year << "." << std::endl;
                                } else {
                                    std::cout << "Failed to set up meal period." << std::endl;
                                }
                                break;
                            }
                            case 2: {
                                std::vector<MealPeriod> periods = getAllMealPeriods();
                                if (periods.empty()) {
                                    std::cout << "No meal periods found." << std::endl;
                                } else {
                                    std::cout << "\n--- All Meal Periods ---" << std::endl;
                                    std::cout << std::left << std::setw(5) << "ID"
                                              << std::setw(15) << "Month"
                                              << "Year" << std::endl;
                                    std::cout << std::string(30, '-') << std::endl;
                                    for (const auto& period : periods) {
                                        std::cout << std::left << std::setw(5) << period.id
                                                  << std::setw(15) << period.month
                                                  << period.year << std::endl;
                                    }
                                }
                                break;
                            }
                            case 0:
                                break;
                            default:
                                std::cout << "Invalid choice. Please try again." << std::endl;
                        }
                    }
}

void handleMenuManagement(const User& currentUser) {
    if (currentUser.role != UserRole::Admin && currentUser.role != UserRole::Staff) {
        std::cout << "Access Denied. You must be an Admin or Staff." << std::endl;
        return;
    }

    int menuChoice = -1;
                    while (menuChoice != 0) {
                        std::cout << "\n--- Menu Management ---" << std::endl;
                        std::cout << "1. Add Menu Item" << std::endl;
                        std::cout << "2. View All Menu Items" << std::endl;
                        std::cout << "3. Edit Menu Item" << std::endl;
                        std::cout << "4. Delete Menu Item" << std::endl;
                        std::cout << "5. Set Daily Menu" << std::endl;
                        std::cout << "6. View Daily Menu" << std::endl;
                        std::cout << "7. View Menu History" << std::endl;
                        std::cout << "0. Back" << std::endl;
                        std::cout << "Enter your choice: ";
                        menuChoice = getIntegerInput();

                        switch (menuChoice) {
                            case 1: {
                                std::string itemName;
                                std::cout << "Enter the name of the new menu item: ";
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                std::getline(std::cin, itemName);
                                if (addMenuItem(itemName)) {
                                    std::cout << "Menu item '" << itemName << "' added successfully." << std::endl;
                                } else {
                                    std::cout << "Failed to add menu item." << std::endl;
                                }
                                break;
                            }
                            case 2: {
                                std::vector<MenuItem> items = getAllMenuItems();
                                if (items.empty()) {
                                    std::cout << "No menu items found." << std::endl;
                                } else {
                                    std::cout << "\n--- All Menu Items ---" << std::endl;
                                    std::cout << std::left << std::setw(5) << "ID"
                                              << "Name" << std::endl;
                                    std::cout << std::string(40, '-') << std::endl;
                                    for (const auto& item : items) {
                                        std::cout << std::left << std::setw(5) << item.id
                                                  << item.name << std::endl;
                                    }
                                }
                                break;
                            }
                            case 3: {
                                int id;
                                std::string newName;
                                std::cout << "Enter menu item ID to edit: ";
                                id = getIntegerInput();
                                std::cout << "Enter new name for the menu item: ";
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                std::getline(std::cin, newName);
                                if (editMenuItem(id, newName)) {
                                    std::cout << "Menu item updated successfully!" << std::endl;
                                } else {
                                    std::cout << "Failed to update menu item." << std::endl;
                                }
                                break;
                            }
                            case 4: {
                                int id;
                                std::cout << "Enter menu item ID to delete: ";
                                id = getIntegerInput();
                                if (deleteMenuItem(id)) {
                                    std::cout << "Menu item deleted successfully!" << std::endl;
                                } else {
                                    std::cout << "Failed to delete menu item." << std::endl;
                                }
                                break;
                            }
                            case 5: {
                                std::string date;
                                std::cout << "Enter date for the menu (YYYY-MM-DD): ";
                                std::cin >> date;
                                if (!isValidDate(date)) {
                                    std::cout << "Invalid date format. Please use YYYY-MM-DD." << std::endl;
                                    break;
                                }
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // FIX: Consume newline here

                                // Helper lambda to get item IDs from user
                                auto getItemIds = [](const std::string& mealName) {
                                    std::vector<int> itemIds;
                                    std::string line;
                                    std::cout << "Enter menu item IDs for " << mealName << " (space-separated, e.g., 1 3 5): ";
                                    std::getline(std::cin, line);
                                    std::stringstream ss(line);
                                    int id;
                                    while (ss >> id) {
                                        itemIds.push_back(id);
                                    }
                                    return itemIds;
                                };

                                // First, show all available menu items so the user knows which IDs to enter.
                                std::vector<MenuItem> items = getAllMenuItems();
                                if (items.empty()) {
                                    std::cout << "No menu items found. Please add some items first." << std::endl;
                                    break;
                                }
                                std::cout << "\n--- Available Menu Items ---" << std::endl;
                                std::cout << std::left << std::setw(5) << "ID" << "Name" << std::endl;
                                std::cout << std::string(40, '-') << std::endl;
                                for (const auto& item : items) {
                                    std::cout << std::left << std::setw(5) << item.id << item.name << std::endl;
                                }
                                std::cout << std::endl;

                                std::vector<int> breakfastItems = getItemIds("Breakfast");
                                std::vector<int> lunchItems = getItemIds("Lunch");
                                std::vector<int> dinnerItems = getItemIds("Dinner");

                                if (setDailyMenu(date, breakfastItems, lunchItems, dinnerItems)) {
                                    std::cout << "Daily menu for " << date << " set successfully." << std::endl;
                                } else {
                                    std::cout << "Failed to set daily menu." << std::endl;
                                }
                                break;
                            }
                            case 6: {
                                std::string date;
                                std::cout << "Enter date to view menu (YYYY-MM-DD): ";
                                std::cin >> date;
                                if (!isValidDate(date)) {
                                    std::cout << "Invalid date format. Please use YYYY-MM-DD." << std::endl;
                                    break;
                                }

                                DailyMenu menu = getDailyMenu(date);

                                if (menu.breakfast.empty() && menu.lunch.empty() && menu.dinner.empty()) {
                                    std::cout << "No menu has been set for " << date << "." << std::endl;
                                } else {
                                    std::cout << "\n--- Menu for " << date << " ---" << std::endl;

                                    auto printMeal = [](const std::string& mealName, const std::vector<MenuItem>& items) {
                                        std::cout << "\n" << mealName << ":" << std::endl;
                                        if (items.empty()) {
                                            std::cout << "  (No items set)" << std::endl;
                                        } else {
                                            for (const auto& item : items) {
                                                std::cout << "  - " << item.name << std::endl;
                                            }
                                        }
                                    };

                                    printMeal("Breakfast", menu.breakfast);
                                    printMeal("Lunch", menu.lunch);
                                    printMeal("Dinner", menu.dinner);
                                }
                                break;
                            }
                            case 7: {
                                std::vector<DailyMenu> menuHistory = getMenuHistory();
                                if (menuHistory.empty()) {
                                    std::cout << "No menu history found." << std::endl;
                                } else {
                                    std::cout << "\n--- Menu History ---" << std::endl;
                                    for (const auto& menu : menuHistory) {
                                        std::cout << "\n--- Menu for " << menu.date << " ---" << std::endl;
                                        auto printMeal = [](const std::string& mealName, const std::vector<MenuItem>& items) {
                                            std::cout << "\n" << mealName << ":" << std::endl;
                                            if (items.empty()) {
                                                std::cout << "  (No items set)" << std::endl;
                                            } else {
                                                for (const auto& item : items) {
                                                    std::cout << "  - " << item.name << std::endl;
                                                }
                                            }
                                        };
                                        printMeal("Breakfast", menu.breakfast);
                                        printMeal("Lunch", menu.lunch);
                                        printMeal("Dinner", menu.dinner);
                                    }
                                }
                                break;
                            }
                            case 0: break;
                            default: std::cout << "Invalid choice or feature not yet implemented." << std::endl;
                        }
                    }
}

void handleExpenseManagement(const User& currentUser) {
    if (currentUser.role != UserRole::Admin && currentUser.role != UserRole::Staff) {
        std::cout << "Access Denied. You must be an Admin or Staff." << std::endl;
        return;
    }

    int expenseChoice = -1;
                        while (expenseChoice != 0) {
                            std::cout << "\n--- Expense Tracking ---" << std::endl;
                            std::cout << "1. Add Expense" << std::endl;
                            std::cout << "2. View All Expenses" << std::endl;
                            std::cout << "3. View Expenses by Category" << std::endl;
                            std::cout << "4. Delete Expense" << std::endl;
                            std::cout << "5. Edit Expense" << std::endl;
                            std::cout << "0. Back" << std::endl;
                            std::cout << "Enter your choice: ";
                            expenseChoice = getIntegerInput();

                            switch (expenseChoice) {
                                case 1:
                                    {
                                        std::string date, item, category;
                                        double price;
                                        int userId;

                                        std::cout << "Enter purchase date (YYYY-MM-DD): ";
                                        std::cin >> date;
                                        if (!isValidDate(date)) {
                                            std::cout << "Invalid date format. Please use YYYY-MM-DD." << std::endl;
                                            break;
                                        }
                                        std::cout << "Enter item name: ";
                                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                        std::getline(std::cin, item);
                                        std::cout << "Enter price: ";
                                        price = getDoubleInput();
                                        std::cout << "Enter ID of user who paid: ";
                                        userId = getIntegerInput();
                                        std::cout << "Enter category (e.g., Groceries, Supplies): ";
                                        std::cin >> category;

                                        if (addExpense(date, item, price, userId, category)) {
                                            std::cout << "Expense added successfully!" << std::endl;
                                        } else {
                                            std::cout << "Failed to add expense." << std::endl;
                                        }
                                    }
                                    break;
                                case 2:
                                    {
                                        std::vector<Expense> expenses = getAllExpenses();
                                        if (expenses.empty()) {
                                            std::cout << "No expenses found." << std::endl;
                                        } else {
                                            std::cout << "\n--- All Expenses ---" << std::endl;
                                            std::cout << std::left << std::setw(5) << "ID"
                                                      << std::setw(12) << "Date"
                                                      << std::setw(30) << "Item"
                                                      << std::setw(15) << "Category"
                                                      << std::setw(20) << "Paid By"
                                                      << std::right << std::setw(10) << "Price" << std::endl;
                                            std::cout << std::string(92, '-') << std::endl;
                                            for (const auto& exp : expenses) {
                                                std::cout << std::left << std::setw(5) << exp.id
                                                          << std::setw(12) << exp.purchase_date
                                                          << std::setw(30) << exp.item_name
                                                          << std::setw(15) << exp.category
                                                          << std::setw(20) << exp.paid_by_user_name
                                                          << std::right << std::setw(10) << std::fixed << std::setprecision(2) << exp.price << std::endl;
                                            }
                                        }
                                    }
                                    break;
                                case 3:
                                    {
                                        std::string category;
                                        std::cout << "Enter category to view expenses: ";
                                        std::cin >> category;
                                        std::vector<Expense> expenses = getExpensesByCategory(category);
                                        if (expenses.empty()) {
                                            std::cout << "No expenses found for category: " << category << std::endl;
                                        } else {
                                            std::cout << "\n--- Expenses for Category: " << category << " ---" << std::endl;
                                            std::cout << std::left << std::setw(5) << "ID"
                                                        << std::setw(12) << "Date"
                                                        << std::setw(30) << "Item"
                                                        << std::setw(15) << "Category"
                                                        << std::setw(20) << "Paid By"
                                                        << std::right << std::setw(10) << "Price" << std::endl;
                                            std::cout << std::string(92, '-') << std::endl;
                                            for (const auto& exp : expenses) {
                                                std::cout << std::left << std::setw(5) << exp.id
                                                          << std::setw(12) << exp.purchase_date
                                                          << std::setw(30) << exp.item_name
                                                          << std::setw(15) << exp.category
                                                          << std::setw(20) << exp.paid_by_user_name
                                                          << std::right << std::setw(10) << std::fixed << std::setprecision(2) << exp.price << std::endl;
                                            }
                                        }
                                    }
                                    break;
                                case 4:
                                    {
                                        int id;
                                        std::cout << "Enter the ID of the expense to delete: ";
                                        id = getIntegerInput();
                                        if (deleteExpense(id)) {
                                            std::cout << "Expense with ID " << id << " deleted successfully." << std::endl;
                                        } else {
                                            std::cout << "Failed to delete expense. It might not exist." << std::endl;
                                        }
                                    }
                                    break;
                                case 5:
                                    {
                                        int id;
                                        std::string item, category;
                                        double price;

                                        std::cout << "Enter the ID of the expense to edit: ";
                                        id = getIntegerInput();
                                        std::cout << "Enter new item name: ";
                                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                        std::getline(std::cin, item);
                                        std::cout << "Enter new price: ";
                                        price = getDoubleInput();
                                        std::cout << "Enter new category: ";
                                        std::cin >> category;
                                        if (editExpense(id, item, price, category)) {
                                            std::cout << "Expense updated successfully!" << std::endl;
                                        } else {
                                            std::cout << "Failed to update expense. It might not exist." << std::endl;
                                        }
                                    }
                                    break;
                                case 0: break;
                                default: std::cout << "Invalid choice." << std::endl;
                            }
                        }
}

void handleAttendanceManagement(const User& currentUser) {
    if (currentUser.role != UserRole::Admin && currentUser.role != UserRole::Staff) {
        std::cout << "Access Denied. You must be an Admin or Staff." << std::endl;
        return;
    }

    int attendanceChoice = -1;
                        while (attendanceChoice != 0) {
                            std::cout << "\n--- Meal Consumption Tracking ---" << std::endl;
                            std::cout << "1. Record Meal Attendance" << std::endl;
                            std::cout << "2. View Attendance for a Date" << std::endl;
                            std::cout << "0. Back" << std::endl;
                            std::cout << "Enter your choice: ";
                            attendanceChoice = getIntegerInput();

                            switch (attendanceChoice) {
                                case 1: {
                                    std::string date, mealType;
                                    int userId;
                                    std::cout << "Enter date (YYYY-MM-DD): ";
                                    std::cin >> date;
                                    if (!isValidDate(date)) {
                                        std::cout << "Invalid date format. Please use YYYY-MM-DD." << std::endl;
                                        break;
                                    }
                                    std::cout << "Enter user ID: ";
                                    userId = getIntegerInput();
                                    std::cout << "Enter meal type (Breakfast, Lunch, Dinner): ";
                                    std::cin >> mealType;

                                    if (recordAttendance(userId, date, mealType)) {
                                        std::cout << "Attendance recorded successfully." << std::endl;
                                    } else {
                                        std::cout << "Failed to record attendance." << std::endl;
                                    }
                                    break;
                                }
                                case 2: {
                                    std::string date;
                                    std::cout << "Enter date to view attendance (YYYY-MM-DD): ";
                                    std::cin >> date;
                                    if (!isValidDate(date)) {
                                        std::cout << "Invalid date format. Please use YYYY-MM-DD." << std::endl;
                                        break;
                                    }

                                    std::vector<MealAttendance> attendanceList = getAttendanceForDate(date);

                                    if (attendanceList.empty()) {
                                        std::cout << "No attendance records found for " << date << "." << std::endl;
                                    } else {
                                        std::cout << "\n--- Attendance for " << date << " ---" << std::endl;

                                        auto printAttendeesForMeal = [&](const std::string& mealType) {
                                            std::cout << "\n" << mealType << ":" << std::endl;
                                            bool found = false;
                                            for (const auto& att : attendanceList) {
                                                if (att.meal_type == mealType) {
                                                    std::cout << "  - " << att.user_name << " (ID: " << att.user_id << ")" << std::endl;
                                                    found = true;
                                                }
                                            }
                                            if (!found) {
                                                std::cout << "  (No one attended)" << std::endl;
                                            }
                                        };

                                        printAttendeesForMeal("Breakfast");
                                        printAttendeesForMeal("Lunch");
                                        printAttendeesForMeal("Dinner");
                                    }
                                    break;
                                }
                                case 0: break;
                                default: std::cout << "Invalid choice or feature not yet implemented." << std::endl;
                            }
                        }
}

void handleSettingsManagement(const User& currentUser) {
    if (currentUser.role != UserRole::Admin) {
        std::cout << "Access Denied. Only Admins can manage settings." << std::endl;
        return;
    }

    int settingsChoice = -1;
                    while (settingsChoice != 0) {
                        std::cout << "\n--- Information Editing & Settings ---" << std::endl;
                        std::cout << "1. View System Settings" << std::endl;
                        std::cout << "2. Update System Settings" << std::endl;
                        std::cout << "0. Back" << std::endl;
                        std::cout << "Enter your choice: ";
                        settingsChoice = getIntegerInput();

                        switch (settingsChoice) {
                            case 1: {
                                SystemSettings settings = getSystemSettings();
                                std::cout << "\n--- Current System Settings ---" << std::endl;
                                std::cout << "Currency: " << settings.currency << std::endl;
                                break;
                            }
                            case 2: {
                                SystemSettings newSettings;
                                std::cout << "Enter new currency symbol (e.g., EUR, JPY): ";
                                std::cin >> newSettings.currency;
                                if (updateSystemSettings(newSettings)) {
                                    std::cout << "Settings updated successfully." << std::endl;
                                } else {
                                    std::cout << "Failed to update settings." << std::endl;
                                }
                                break;
                            }
                            case 0:
                                break;
                            default:
                                std::cout << "Invalid choice. Please try again." << std::endl;
                        }
                    }
}

void displaySettlementReport(double meal_rate, const std::vector<SettlementReport>& settlement_reports) {
    if (settlement_reports.empty()) {
        std::cout << "Could not generate a report. Check if the period ID is valid and if there is data for that period." << std::endl;
    } else {
        std::cout << "\n\n--- Monthly Settlement Report ---" << std::endl;
        std::cout << "Meal Rate for this period: " << std::fixed << std::setprecision(2) << meal_rate << "/meal" << std::endl;
        std::cout << std::string(120, '-') << std::endl;
        std::cout << std::left
                    << std::setw(20) << "User"
                    << std::setw(15) << "Total Meals"
                    << std::setw(15) << "Meal Cost"
                    << std::setw(20) << "Contributions"
                    << std::setw(20) << "Shopping Paid"
                    << std::setw(20) << "Final Balance" << std::endl;
        std::cout << std::string(120, '-') << std::endl;
        for (const auto& report : settlement_reports) {
            std::cout << std::left << std::fixed << std::setprecision(2)
                        << std::setw(20) << report.user_name << std::setw(15) << report.total_meals
                        << std::setw(15) << report.total_meal_cost << std::setw(20) << report.total_payments
                        << std::setw(20) << report.total_shopping_expenses << std::setw(20) << report.final_balance << std::endl;
        }
    }
}

void handleFinanceManagement(const User& currentUser) {
    if (currentUser.role != UserRole::Admin) {
        std::cout << "Access Denied. Only Admins can manage finances." << std::endl;
        return;
    }

    int financeChoice = -1;
                    while (financeChoice != 0) {
                        std::cout << "\n--- Financial Analysis & Settlement ---" << std::endl;
                        std::cout << "1. Record a Payment" << std::endl;
                        std::cout << "2. View Payments for a User" << std::endl;
                        std::cout << "3. View All Financial Reports" << std::endl;
                        std::cout << "4. Generate Monthly Settlement" << std::endl;
                        std::cout << "0. Back" << std::endl;
                        std::cout << "Enter your choice: ";
                        financeChoice = getIntegerInput();

                        switch (financeChoice) {
                            case 1: {
                                int userId;
                                double amount;
                                std::string date;
                                std::cout << "Enter user ID: ";
                                userId = getIntegerInput();
                                std::cout << "Enter payment amount: ";
                                amount = getDoubleInput();
                                std::cout << "Enter payment date (YYYY-MM-DD): ";
                                std::cin >> date;
                                if (!isValidDate(date)) {
                                    std::cout << "Invalid date format. Please use YYYY-MM-DD." << std::endl;
                                    break;
                                }
                                if (recordPayment(userId, amount, date)) {
                                    std::cout << "Payment recorded successfully." << std::endl;
                                } else {
                                    std::cout << "Failed to record payment." << std::endl;
                                }
                                break;
                            }
                            case 2: {
                                int userId;
                                std::cout << "Enter user ID to view payments: ";
                                userId = getIntegerInput();
                                std::vector<Payment> payments = getPaymentsByUser(userId);
                                if (payments.empty()) {
                                    std::cout << "No payments found for this user." << std::endl;
                                } else {
                                    std::cout << "\n--- Payments for User ID " << userId << " ---" << std::endl;
                                    std::cout << std::left << std::setw(10) << "Payment ID"
                                              << std::setw(15) << "Date"
                                              << "Amount" << std::endl;
                                    std::cout << std::string(40, '-') << std::endl;
                                    for (const auto& p : payments) {
                                        std::cout << std::left << std::setw(10) << p.id
                                                  << std::setw(15) << p.date
                                                  << std::fixed << std::setprecision(2) << p.amount << std::endl;
                                    }
                                }
                                break;
                            }
                            case 3: {
                                std::vector<FinancialReport> reports = getAllFinancialReports();
                                if (reports.empty()) {
                                    std::cout << "No financial reports available." << std::endl;
                                } else {
                                    std::cout << "\n--- All Financial Reports ---" << std::endl;
                                    std::cout << std::left << std::setw(5) << "ID"
                                              << std::setw(20) << "Username"
                                              << std::setw(20) << "Total Payments"
                                              << std::setw(20) << "Total Expenses"
                                              << "Balance" << std::endl;
                                    std::cout << std::string(80, '-') << std::endl;
                                    for (const auto& report : reports) {
                                        std::cout << std::left << std::setw(5) << report.user_id
                                                  << std::setw(20) << report.user_name
                                                  << std::setw(20) << std::fixed << std::setprecision(2) << report.total_contributions
                                                  << std::setw(20) << std::fixed << std::setprecision(2) << report.total_expenses
                                                  << std::fixed << std::setprecision(2) << report.debt_or_surplus << std::endl;
                                    }
                                }
                                break;
                            }
                            case 4: {
                                // 1. Show available meal periods
                                std::vector<MealPeriod> periods = getAllMealPeriods();
                                if (periods.empty()) {
                                    std::cout << "No meal periods found. Please set one up first." << std::endl;
                                    break;
                                }
                                std::cout << "\n--- Available Meal Periods ---" << std::endl;
                                std::cout << std::left << std::setw(5) << "ID" << std::setw(15) << "Month" << "Year" << std::endl;
                                std::cout << std::string(30, '-') << std::endl;
                                for (const auto& period : periods) {
                                    std::cout << std::left << std::setw(5) << period.id << std::setw(15) << period.month << period.year << std::endl;
                                }

                                // 2. Get user input
                                std::cout << "\nEnter the ID of the period to generate a settlement for: ";
                                int period_id = getIntegerInput();

                                // 3. Generate and display the report
                                auto [meal_rate, settlement_reports] = generateMonthlySettlement(period_id);

                                displaySettlementReport(meal_rate, settlement_reports);
                                break;
                            }
                            case 0:
                                break;
                            default:
                                std::cout << "Invalid choice. Please try again." << std::endl;
                        }
                    }
}

int main() {
    sql::Connection* con = getConnection();
    if (con) {
        std::cout << "Connection is active. Ready to proceed." << std::endl;
    } else {
        std::cerr << "Failed to connect to database." << std::endl;
        return 1;
    }

    std::unique_ptr<User> currentUser = nullptr;
    int choice = -1;

    while (choice != 99) { // Use a non-zero exit code for the main loop
        if (!currentUser) {
            // --- Pre-Login Menu ---
            std::cout << "\n--- Welcome to the Meal Management System ---" << std::endl;
            std::cout << "1. Login" << std::endl;
            std::cout << "2. Register" << std::endl;
            std::cout << "0. Exit" << std::endl;
            std::cout << "Enter your choice: ";
            choice = getIntegerInput();

            switch (choice) {
                case 1: { // Login
                    std::string username, password;
                    std::cout << "Enter username: "; std::cin >> username;
                    std::cout << "Enter password: "; std::cin >> password;
                    currentUser = loginUser(username, password);
                    if (currentUser) {
                        std::cout << "Login successful! Welcome, " << currentUser->name << std::endl;
                    } else {
                        std::cout << "Login failed. Invalid credentials." << std::endl;
                    }
                    break;
                }
                case 2: { // Register
                    std::string username, password, name;
                    std::cout << "Enter username: "; std::cin >> username;
                    std::cout << "Enter password: "; std::cin >> password;
                    std::cout << "Enter full name: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::getline(std::cin, name);
                    // For security, all public registrations default to the Student role.
                    // Admins can change roles later via the User Management menu.
                    UserRole role = UserRole::Student;
                    if (registerUser(username, password, name, role)) {
                        std::cout << "User registered successfully! Please login." << std::endl;
                    } else {
                        std::cout << "User registration failed." << std::endl;
                    }
                    break;
                }
                case 0: { // Exit
                    choice = 99; // Set to exit loop
                    break;
                }
                default:
                    std::cout << "Invalid choice. Please try again." << std::endl;
            }
        } else {
            // --- Post-Login Main Menu ---
            std::cout << "\n========== Meal Management System ==========" << std::endl;
            std::cout << "Logged in as: " << currentUser->name << " (" << roleToString(currentUser->role) << ")" << std::endl;
            std::cout << "--------------------------------------------" << std::endl;
            std::cout << "1. Monthly Meal Period Management" << std::endl;
            std::cout << "2. User Management" << std::endl;
            std::cout << "3. Menu Management" << std::endl;
            std::cout << "4. Expense Tracking" << std::endl;
            std::cout << "5. Meal Consumption Tracking" << std::endl;
            std::cout << "6. Information Editing & Settings" << std::endl;
            std::cout << "7. Financial Analysis & Settlement" << std::endl;
            std::cout << "8. Logout" << std::endl;
            std::cout << "0. Exit" << std::endl;
            std::cout << "============================================" << std::endl;
            std::cout << "Enter your choice: ";
            choice = getIntegerInput();

            switch (choice) {
                case 1: handlePeriodManagement(*currentUser); break;
                case 2: handleUserManagement(*currentUser); break;
                case 3: handleMenuManagement(*currentUser); break;
                case 4: handleExpenseManagement(*currentUser); break;
                case 5: handleAttendanceManagement(*currentUser); break;
                case 6: handleSettingsManagement(*currentUser); break;
                case 7: handleFinanceManagement(*currentUser); break;
                case 8: currentUser = nullptr; std::cout << "You have been logged out." << std::endl; break;
                case 0: choice = 99; break; // Set to exit loop
                default: std::cout << "Invalid choice. Please try again." << std::endl;
            }
        }
    }

    std::cout << "Exiting..." << std::endl;
    return 0;
}
