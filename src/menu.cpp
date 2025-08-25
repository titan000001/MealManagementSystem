#include "menu.h"
#include "database.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <memory>
#include <vector>

bool addMenuItem(const std::string& name) {
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO menu_items (name) VALUES (?)")
        );
        pstmt->setString(1, name);
        pstmt->execute();
        return true;
    } catch (sql::SQLException& e) {
        // Handle unique constraint violation gracefully
        if (e.getErrorCode() == 1062) { // 1062 = ER_DUP_ENTRY
            std::cerr << "Error: Menu item '" << name << "' already exists." << std::endl;
        } else {
            std::cerr << "SQL Error in addMenuItem: " << e.what() << std::endl;
        }
        return false;
    }
}

// Stubs for other functions to be implemented later
bool editMenuItem(int id, const std::string& name) { return false; }
bool deleteMenuItem(int id) { return false; }
std::vector<MenuItem> getAllMenuItems() {
    std::vector<MenuItem> items;
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery("SELECT id, name FROM menu_items ORDER BY name ASC")
        );

        while (res->next()) {
            MenuItem item;
            item.id = res->getInt("id");
            item.name = res->getString("name");
            items.push_back(item);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in getAllMenuItems: " << e.what() << std::endl;
    }
    return items;
}

bool setDailyMenu(const std::string& date, const std::vector<int>& breakfastItems, const std::vector<int>& lunchItems, const std::vector<int>& dinnerItems) {
    sql::Connection* con = getConnection();
    if (!con) {
        std::cerr << "Failed to get database connection in setDailyMenu." << std::endl;
        return false;
    }
    try {
        con->setAutoCommit(false); // Start transaction

        std::unique_ptr<sql::PreparedStatement> pstmt_del(con->prepareStatement("DELETE FROM daily_menus WHERE menu_date = ?"));
        pstmt_del->setString(1, date);
        pstmt_del->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> pstmt_ins(con->prepareStatement("INSERT INTO daily_menus (menu_date, meal_type, menu_item_id) VALUES (?, ?, ?)"));

        auto insertItems = [&](const std::string& mealType, const std::vector<int>& items) {
            for (int itemId : items) {
                pstmt_ins->setString(1, date);
                pstmt_ins->setString(2, mealType);
                pstmt_ins->setInt(3, itemId);
                pstmt_ins->executeUpdate();
            }
        };

        insertItems("Breakfast", breakfastItems);
        insertItems("Lunch", lunchItems);
        insertItems("Dinner", dinnerItems);

        con->commit();
        con->setAutoCommit(true);
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Error in setDailyMenu: " << e.what() << std::endl;
        try {
            con->rollback();
        } catch (sql::SQLException &ex) {
            std::cerr << "SQL Error on rollback: " << ex.what() << std::endl;
        }
        con->setAutoCommit(true);
        return false;
    }
}

DailyMenu getDailyMenu(const std::string& date) {
    DailyMenu dailyMenu;
    dailyMenu.date = date; // Set the date for the returned struct

    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT dm.meal_type, mi.id, mi.name "
                "FROM daily_menus dm "
                "JOIN menu_items mi ON dm.menu_item_id = mi.id "
                "WHERE dm.menu_date = STR_TO_DATE(?, '%Y-%m-%d') "
                "ORDER BY dm.meal_type"
            )
        );
        pstmt->setString(1, date);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            MenuItem item;
            item.id = res->getInt("id");
            item.name = res->getString("name");
            std::string mealType = res->getString("meal_type");

            if (mealType == "Breakfast")      dailyMenu.breakfast.push_back(item);
            else if (mealType == "Lunch")     dailyMenu.lunch.push_back(item);
            else if (mealType == "Dinner")    dailyMenu.dinner.push_back(item);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in getDailyMenu: " << e.what() << std::endl;
    }
    return dailyMenu;
}
std::vector<DailyMenu> getMenuHistory() {
    return {};
}