#include "database.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <mysql_driver.h>

sql::Driver* driver;
sql::Connection* con;

void connectToDatabase() {
    try {
        driver = sql::mysql::get_driver_instance(); // Remove sql::mysql::
        driver = sql::mysql::get_driver_instance(); // Remove sql::mysql::
        driver = get_driver_instance();
    // TODO: Replace "your_mysql_password" with your actual MySQL password
    con = driver->connect("tcp://127.0.0.1:3306", "root", "newpassword");
        con->setSchema("meal_management");
        std::cout << "Successfully connected to the database." << std::endl;
    } catch (sql::SQLException &e) {
        std::cerr << "Could not connect to the database. Error: " << e.what() << std::endl;
        exit(1);
    }
}

sql::Connection* getConnection() {
    if (!con || con->isClosed()) {
        connectToDatabase();
    }
    return con;
}

void closeConnection(sql::Connection* con) {
    if (con) {
        delete con;
        con = nullptr;
    }
}

std::string hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}