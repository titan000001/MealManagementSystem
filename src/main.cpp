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
            std::cin >> choice;

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
                        std::cout << "0. Back" << std::endl;
                        std::cout << "Enter your choice: ";
                        std::cin >> userChoice;
                        switch (userChoice) {
                            case 1: {
                                std::string username, password, name;
                                int roleInt;
                                std::cout << "Enter username: "; std::cin >> username;
                                std::cout << "Enter password: "; std::cin >> password;
                                std::cout << "Enter name: "; std::cin >> name;
                                std::cout << "Select role (0: Student, 1: Staff, 2: Admin): "; std::cin >> roleInt;
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
                                User* user = loginUser(username, password);
                                if (user) {
                                    std::cout << "Login successful! Welcome, " << user->name << std::endl;
                                } else {
                                    std::cout << "Login failed. Invalid credentials." << std::endl;
                                }
                                break;
                            }
                            case 3: {
                                int id;
                                std::cout << "Enter user ID: "; std::cin >> id;
                                User* user = getUserById(id);
                                if (user) {
                                    std::cout << "User Profile:" << std::endl;
                                    std::cout << "Username: " << user->username << std::endl;
                                    std::cout << "Name: " << user->name << std::endl;
                                    std::cout << "Role: " << static_cast<int>(user->role) << std::endl;
                                } else {
                                    std::cout << "User not found." << std::endl;
                                }
                                break;
                            }
                            case 4: {
                                int id;
                                std::string newName;
                                std::cout << "Enter user ID: "; std::cin >> id;
                                std::cout << "Enter new name: "; std::cin >> newName;
                                if (updateUserProfile(id, newName)) {
                                    std::cout << "Profile updated successfully!" << std::endl;
                                } else {
                                    std::cout << "Profile update failed." << std::endl;
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
                case 3:
                    std::cout << "[Stub] Menu Management" << std::endl;
                    // TODO: Call menu management functions
                    break;
                case 4:
                    std::cout << "[Stub] Expense Tracking" << std::endl;
                    // TODO: Call expense tracking functions
                    break;
                case 5:
                    std::cout << "[Stub] Meal Consumption Tracking" << std::endl;
                    // TODO: Call attendance functions
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

        closeConnection(con);
        return 0;
}