# Meal Management System 🍲

A comprehensive desktop GUI application built with C++ and Qt to efficiently manage dining operations for hostels, messes, or shared living spaces. Say goodbye to messy spreadsheets and manual calculations!

## Overview

Managing a shared dining facility can be chaotic. Tracking who ate what, who bought the groceries, and who owes money at the end of the month often involves confusing ledgers, lost receipts, and complicated calculations. This project was built to solve that exact problem.

The Meal Management System provides a centralized, secure, and user-friendly desktop application to track users, daily menus, meal attendance, and expenses. Its core feature is the ability to generate a detailed financial settlement report with a single click, ensuring fairness and transparency for all members.

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
/
## Tech Stack 🛠️

*   **Backend & Core Logic:** C++ (17)
*   **GUI Framework:** Qt6
*   **Database:** MySQL
*   **Database Connector:** MySQL Connector/C++
*   **Cryptography:** OpenSSL (for SHA-256 hashing and salt generation)
*   **Build System:** CMake

## Getting Started on Linux (Debian/Ubuntu) 🚀

Follow these instructions to get a copy of the project up and running on your local machine.

### 1. Prerequisites

You will need a C++ compiler, CMake, MySQL, and the necessary development libraries for Qt6, OpenSSL, and MySQL.

```bash
# Update package lists
sudo apt update

# Install essential build tools (includes g++, make, and cmake)
sudo apt install build-essential cmake

# Install Qt6 development tools
sudo apt install qt6-base-dev

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

The application connects to the database using the details specified in a `config.ini` file. You must create this file before running the application.

1.  **Copy the Example File**:
    In the project's root directory, make a copy of the example configuration file and name it `config.ini`.
    ```bash
    cp config.ini.example config.ini
    ```

2.  **Edit the Configuration**:
    Open the new `config.ini` file in a text editor. It will look like this:
    ```ini
    [Database]
    host=tcp://127.0.0.1:3306
    user=meal_user
    password=your_password
    database=meal_management
    ```
    Replace `your_password` with the actual password you created for the `meal_user` during the database setup.

### 4. Build and Run

1.  **Navigate to the project's root directory**.

2.  **Create a build directory**:
    ```bash
    mkdir build && cd build
    ```

3.  **Configure the project with CMake**:
    ```bash
    cmake ..
    ```

4.  **Compile the project**:
    ```bash
    make
    ```

5.  **Run the application**:
    ```bash
    ./MealManagementSystem
    ```

You should now see the login window for the Meal Management System!

## Project Structure 📂
```
.
├── build/                # Build files will be generated here
├── include/              # C++ header files (.h)
├── src/                  # C++ source files (.cpp)
├── CMakeLists.txt        # The build script for CMake
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
