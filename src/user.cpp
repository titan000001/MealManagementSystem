#include "user.h"
#include "database.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <memory>
 
bool registerUser(const std::string& username, const std::string& password, const std::string& name, UserRole role) {
    try {
        std::string salt = generateSalt();
        std::string password_hash = hashPassword(password, salt);

        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO users (username, password_hash, salt, name, role) VALUES (?, ?, ?, ?, ?)")
        );
        pstmt->setString(1, username);
        pstmt->setString(2, password_hash);
        pstmt->setString(3, salt);
        pstmt->setString(4, name);
        pstmt->setInt(5, static_cast<int>(role));
        pstmt->execute();
        return true;
    } catch (sql::SQLException& e) {
        if (e.getErrorCode() == 1062) { // ER_DUP_ENTRY
            std::cerr << "Error: Username '" << username << "' already exists." << std::endl;
        } else {
            std::cerr << "SQL Error in registerUser: " << e.what() << std::endl;
        }
        return false;
    }
}

std::unique_ptr<User> loginUser(const std::string& username, const std::string& password) {
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT id, username, password_hash, salt, name, role FROM users WHERE username = ?")
        );
        pstmt->setString(1, username);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            std::string db_password_hash = res->getString("password_hash");
            std::string db_salt = res->getString("salt");
            if (db_password_hash == hashPassword(password, db_salt)) {
                auto user = std::make_unique<User>();
                user->id = res->getInt("id");
                user->username = res->getString("username");
                user->password_hash = db_password_hash;
                user->salt = db_salt;
                user->name = res->getString("name");
                user->role = static_cast<UserRole>(res->getInt("role"));
                return user;
            }
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in loginUser: " << e.what() << std::endl;
    }
    return nullptr;
}

std::vector<User> getAllUsers() {
    std::vector<User> users;
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT id, username, name, role FROM users ORDER BY id"));

        while (res->next()) {
            User user;
            user.id = res->getInt("id");
            user.username = res->getString("username");
            user.name = res->getString("name");
            user.role = static_cast<UserRole>(res->getInt("role"));
            users.push_back(user);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in getAllUsers: " << e.what() << std::endl;
    }
    return users;
}

std::unique_ptr<User> getUserById(int id) {
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT id, username, password_hash, salt, name, role FROM users WHERE id = ?")
        );
        pstmt->setInt(1, id);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            auto user = std::make_unique<User>();
            user->id = res->getInt("id");
            user->username = res->getString("username");
            user->password_hash = res->getString("password_hash");
            user->salt = res->getString("salt");
            user->name = res->getString("name");
            user->role = static_cast<UserRole>(res->getInt("role"));
            return user;
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in getUserById: " << e.what() << std::endl;
    }
    return nullptr;
}

bool updateUserProfile(int id, const std::string& name) {
    try {
        sql::Connection* con = getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE users SET name = ? WHERE id = ?"));
        pstmt->setString(1, name);
        pstmt->setInt(2, id);
        // executeUpdate returns the number of affected rows
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in updateUserProfile: " << e.what() << std::endl;
        return false;
    }
}
