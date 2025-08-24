#pragma once

#include <string>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>

void connectToDatabase();
sql::Connection* getConnection();
void closeConnection(sql::Connection* con);
std::string hashPassword(const std::string& password);