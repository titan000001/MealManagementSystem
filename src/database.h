#pragma once

#include <string>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>

sql::Connection* getConnection();
std::string generateSalt();
std::string hashPassword(const std::string& password, const std::string& salt);