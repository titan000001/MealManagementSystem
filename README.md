# Meal Management System 🍲

A comprehensive, console-based application built with C++ and MySQL to efficiently manage dining operations for hostels, messes, or shared living spaces. Say goodbye to messy spreadsheets and manual calculations!

## Overview

Managing a shared dining facility can be chaotic. Tracking who ate what, who bought the groceries, and who owes money at the end of the month often involves confusing ledgers, lost receipts, and complicated calculations. This project was built to solve that exact problem.

The Meal Management System provides a centralized, secure, and automated solution to track users, daily menus, meal attendance, and expenses. Its core feature is the ability to generate a detailed financial settlement report with a single command, ensuring fairness and transparency for all members.

## Key Features ✨

*   **🔐 Secure User & Session Management**
    *   Secure user registration with industry-standard **salted SHA-256 password hashing**.
    *   **Role-Based Access Control (RBAC)** with three distinct roles: `Student`, `Staff`, and `Admin`.
    *   Stateful login system restricting access to sensitive features based on the logged-in user's role.
    *   Users can easily view and update their own profiles.

*   **💸 Financial Management (Admin-only)**
    *   Record user payments and track individual contributions.
    *   Generate a complete **monthly settlement report**, automatically calculating the cost-per-meal and each user's final balance (debt or surplus).
    *   View detailed financial summaries for all users at a glance.

*   **🛒 Expense & Shopping Tracking (Admin/Staff)**
    *   Log all purchases with details like date, item, price, category, and the user who paid.
    *   View, edit, and delete expense records.
    *   Filter expenses by category for better analysis.

*   **📅 Menu & Meal Management (Admin/Staff)**
    *   Manage a central list of all possible menu items.
    *   Set and view daily menus for breakfast, lunch, and dinner.
    *   Record meal attendance for each user.
    *   View historical menus and daily attendance records.

*   **⚙️ System Administration (Admin-only)**
    *   Set up and manage monthly meal periods.
    *   Register new users and view a complete list of all members.
    *   Configure system-wide settings like currency.

## System in Action 📸

Here's a glimpse of the user-friendly console interface:

```
--- Welcome to the Meal Management System ---
1. Login
2. Register
0. Exit
Enter your choice: 1

Enter username: admin
Enter password: [your_password]
Login successful! Welcome, Admin User

========== Meal Management System ==========
Logged in as: Admin User (Admin)
--------------------------------------------
1. Monthly Meal Period Management
2. User Management
3. Menu Management
4. Expense Tracking
5. Meal Consumption Tracking
6. Information Editing & Settings
7. Financial Analysis & Settlement
8. Logout
0. Exit
============================================
Enter your choice: 7
```

## Tech Stack 🛠️

*   **Backend:** C++ (17)
*   **Database:** MySQL
*   **Database Connector:** MySQL Connector/C++
*   **Cryptography:** OpenSSL (for SHA-256 hashing and salt generation)
*   **Build System:** Make

## Getting Started on Linux (Debian/Ubuntu) 🚀

Follow these instructions to get a copy of the project up and running on your local machine.

### 1. Prerequisites

You will need a C++ compiler, Make, MySQL, and the necessary development libraries.

```bash
# Update package lists
sudo apt update

# Install essential build tools (includes make and g++)
sudo apt install build-essential

# Install MySQL Server
sudo apt install mysql-server

# Install the OpenSSL development library
sudo apt install libssl-dev

# Install the MySQL C++ Connector development library
# Note: The package name may vary. It could be libmysqlcppconn-dev
sudo apt install libmysqlcppconn-dev
```

### 2. Database Setup

First, secure your MySQL installation and create a dedicated user and database for the application.

1.  **Secure MySQL (Recommended)**:
    ```bash
    sudo mysql_secure_installation
    ```
    Follow the on-screen prompts to set a root password and configure security settings.

2.  **Create Database and User**:
    Log in to MySQL as the root user:
    ```bash
    sudo mysql -u root -p
    ```
    Now, run the following SQL commands. Replace `'your_password'` with a strong password.

    ```sql
    CREATE DATABASE meal_management;
    CREATE USER 'meal_user'@'localhost' IDENTIFIED BY 'your_password';
    GRANT ALL PRIVILEGES ON meal_management.* TO 'meal_user'@'localhost';
    FLUSH PRIVILEGES;
    EXIT;
    ```

3.  **Create Tables**:
    From your project's root directory, use the provided schema file to create all the necessary tables.

    ```bash
    mysql -u meal_user -p meal_management < schema.sql
    ```
    Enter the password you created for `meal_user` when prompted.

### 3. Application Configuration

The application needs to know how to connect to your database. Open the `src/database.cpp` file and update the connection details with the password you set in the previous step.

```cpp
// src/database.cpp

// ...
// IMPORTANT: Replace "newpassword" with the actual password you created.
con.reset(driver->connect("tcp://127.0.0.1:3306", "meal_user", "newpassword"));
con->setSchema("meal_management");
// ...
```

### 4. Build and Run

1.  **Navigate to the project's root directory**.

2.  **Compile the project**:
    Use the provided `Makefile` for an efficient build. It will only recompile files that have changed.
    ```bash
    make
    ```

3.  **Run the application**:
    ```bash
    ./MealManagementSystem
    ```

You should now see the welcome menu for the Meal Management System!

## Project Structure 📂
```
.
├── src/                  # All C++ source and header files
├── Makefile              # The build script for compiling the project
├── schema.sql            # The complete SQL schema for setting up the database
└── README.md             # You are here!
```

## Authors & Acknowledgements 🙏

*   Titan
*   Tamim
*   Sumona

This project was developed as a comprehensive exercise in C++ application development and database management.

## License 📄

This project is open source and available under the MIT License.