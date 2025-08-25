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

int main() {
    sql::Connection* con = getConnection();
        if (con) {
            std::cout << "Connection is active. Ready to proceed." << std::endl;
        } else {
            std::cerr << "Failed to connect to database." << std::endl;
            return 1;
        }

        int choice = -1;
        while (choice != 0) {
            std::cout << "========== Meal Management System ==========" << std::endl;
            std::cout << "1. Monthly Meal Period Setup" << std::endl;
            std::cout << "2. User Management" << std::endl;
            std::cout << "3. Menu Management" << std::endl;
            std::cout << "4. Expense Tracking" << std::endl;
            std::cout << "5. Meal Consumption Tracking" << std::endl;
            std::cout << "6. Information Editing & Settings" << std::endl;
            std::cout << "7. Financial Analysis & Settlement" << std::endl;
            std::cout << "0. Exit" << std::endl;
            std::cout << "============================================" << std::endl;
            std::cout << "Enter your choice: ";
            choice = getIntegerInput();

            switch (choice) {
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
                    int userChoice = -1;
                    while (userChoice != 0) {
                        std::cout << "--- User Management ---" << std::endl;
                        std::cout << "1. Register User" << std::endl;
                        std::cout << "2. Login" << std::endl;
                        std::cout << "3. View Profile" << std::endl;
                        std::cout << "4. Update Profile" << std::endl;
                        std::cout << "5. View All Users" << std::endl;
                        std::cout << "0. Back" << std::endl;
                        std::cout << "Enter your choice: ";
                        userChoice = getIntegerInput();
                        switch (userChoice) {
                            case 1: {
                                std::string username, password, name;
                                int roleInt;
                                std::cout << "Enter username: "; std::cin >> username;
                                std::cout << "Enter password: "; std::cin >> password;
                                std::cout << "Enter full name: ";
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume the leftover newline
                                std::getline(std::cin, name);
                                std::cout << "Select role (0: Student, 1: Staff, 2: Admin): ";
                                roleInt = getIntegerInput();
                                UserRole role = static_cast<UserRole>(roleInt);
                                if (registerUser(username, password, name, role)) {
                                    std::cout << "User registered successfully!" << std::endl;
                                } else {
                                    std::cout << "User registration failed." << std::endl;
                                }
                                break;
                            }
                            case 2: {
                                std::string username, password;
                                std::cout << "Enter username: "; std::cin >> username;
                                std::cout << "Enter password: "; std::cin >> password;
                                auto user = loginUser(username, password);
                                if (user) {
                                    std::cout << "Login successful! Welcome, " << user->name << std::endl;
                                    // Memory is now managed automatically by unique_ptr
                                } else {
                                    std::cout << "Login failed. Invalid credentials." << std::endl;
                                }
                                break;
                            }
                            case 3: {
                                int id;
                                std::cout << "Enter user ID: "; std::cin >> id;
                                auto user = getUserById(id);
                                if (user) {
                                    std::cout << "User Profile:" << std::endl;
                                    std::cout << "Username: " << user->username << std::endl;
                                    std::cout << "Name: " << user->name << std::endl;
                                    std::string roleStr = (user->role == UserRole::Admin) ? "Admin" : (user->role == UserRole::Staff) ? "Staff" : "Student";
                                    std::cout << "Role: " << roleStr << std::endl;
                                    // Memory is now managed automatically by unique_ptr
                                } else {
                                    std::cout << "User not found." << std::endl;
                                }
                                break;
                            }
                            case 4: {
                                int id;
                                std::string newName;
                                std::cout << "Enter user ID: "; id = getIntegerInput();
                                std::cout << "Enter new full name: ";
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume the leftover newline
                                std::getline(std::cin, newName);
                                if (updateUserProfile(id, newName)) {
                                    std::cout << "Profile updated successfully!" << std::endl;
                                } else {
                                    std::cout << "Profile update failed." << std::endl;
                                }
                                break;
                            }
                            case 5: {
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
                                        std::string roleStr = (user.role == UserRole::Admin) ? "Admin" : (user.role == UserRole::Staff) ? "Staff" : "Student";
                                        std::cout << std::left << std::setw(5) << user.id
                                                  << std::setw(20) << user.username
                                                  << std::setw(30) << user.name
                                                  << roleStr << std::endl;
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
                    break;
                }
                case 3: {
                    int menuChoice = -1;
                    while (menuChoice != 0) {
                        std::cout << "\n--- Menu Management ---" << std::endl;
                        std::cout << "1. Add Menu Item" << std::endl;
                        std::cout << "2. View All Menu Items" << std::endl;
                        std::cout << "3. Set Daily Menu" << std::endl;
                        std::cout << "4. View Daily Menu" << std::endl;
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
                                std::string date;
                                std::cout << "Enter date for the menu (YYYY-MM-DD): ";
                                std::cin >> date;
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
                            case 4: {
                                std::string date;
                                std::cout << "Enter date to view menu (YYYY-MM-DD): ";
                                std::cin >> date;

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
                            case 0: break;
                            default: std::cout << "Invalid choice or feature not yet implemented." << std::endl;
                        }
                    }
                }
                    break;
                case 4:
                    {
                        int expenseChoice = -1;
                        while (expenseChoice != 0) {
                            std::cout << "\n--- Expense Tracking ---" << std::endl;
                            std::cout << "1. Add Expense" << std::endl;
                            std::cout << "2. View All Expenses" << std::endl;
                            std::cout << "3. Delete Expense" << std::endl;
                            std::cout << "4. Edit Expense" << std::endl;
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
                                case 4:
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
                    break;
                case 5:
                    {
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
                    break;
                case 6:
                    std::cout << "[Stub] Information Editing & Settings" << std::endl;
                    // TODO: Call settings functions
                    break;
                case 7:
                    std::cout << "[Stub] Financial Analysis & Settlement" << std::endl;
                    // TODO: Call finance functions
                    break;
                case 0:
                    std::cout << "Exiting..." << std::endl;
                    break;
                default:
                    std::cout << "Invalid choice. Please try again." << std::endl;
            }
        }

        return 0;
}