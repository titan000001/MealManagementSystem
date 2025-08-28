#pragma once

#include <string>
#include <memory>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>

std::unique_ptr<sql::Connection> getConnection();
std::string generateSalt();
std::string hashPassword(const std::string& password, const std::string& salt);