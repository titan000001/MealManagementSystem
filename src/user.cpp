#include "user.h"
#include "database.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <memory>

namespace { // Anonymous namespace for file-local helpers
    std::string roleToString(UserRole role) {
        switch (role) {
            case UserRole::Admin:   return "Admin";
            case UserRole::Staff:   return "Staff";
            case UserRole::Student: return "Student";
            default:                return "Student"; // Safe default
        }
    }

    UserRole stringToRole(const std::string& roleStr) {
        if (roleStr == "Admin") return UserRole::Admin;
        if (roleStr == "Staff") return UserRole::Staff;
        return UserRole::Student; // Default to student
    }
} // namespace

bool registerUser(const std::string& username, const std::string& password, const std::string& name, UserRole role) {
    try {
        std::string salt = generateSalt();
        std::string password_hash = hashPassword(password, salt);

        std::unique_ptr<sql::Connection> con(getConnection());
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO users (username, password_hash, salt, name, role) VALUES (?, ?, ?, ?, ?)")
        );
        pstmt->setString(1, username);
        pstmt->setString(2, password_hash);
        pstmt->setString(3, salt);
        pstmt->setString(4, name);
        pstmt->setString(5, roleToString(role));
        pstmt->execute();
        return true;
    } catch (sql::SQLException& e) {
        if (e.getErrorCode() == 1062) { // ER_DUP_ENTRY
            std::cerr << "Error: Username '" << username << "' already exists." << std::endl;
        }
        return false;
    }
}

std::unique_ptr<User> loginUser(const std::string& username, const std::string& password) {
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
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
                user->role = stringToRole(res->getString("role"));
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
        std::unique_ptr<sql::Connection> con(getConnection());
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT id, username, name, role FROM users ORDER BY id"));

        while (res->next()) {
            User user;
            user.id = res->getInt("id");
            user.username = res->getString("username");
            user.name = res->getString("name");
            user.role = stringToRole(res->getString("role"));
            users.push_back(user);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in getAllUsers: " << e.what() << std::endl;
    }
    return users;
}

std::unique_ptr<User> getUserById(int id) {
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
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
            user->role = stringToRole(res->getString("role"));
            return user;
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in getUserById: " << e.what() << std::endl;
    }
    return nullptr;
}

bool updateUserProfile(int id, const std::string& name) {
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
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

bool updateUserPassword(int id, const std::string& oldPassword, const std::string& newPassword) {
    try {
        std::unique_ptr<sql::Connection> con(getConnection());
        std::unique_ptr<sql::PreparedStatement> pstmt_select(
            con->prepareStatement("SELECT password_hash, salt FROM users WHERE id = ?")
        );
        pstmt_select->setInt(1, id);
        std::unique_ptr<sql::ResultSet> res(pstmt_select->executeQuery());

        if (!res->next()) {
            std::cerr << "Error: User with ID " << id << " not found." << std::endl;
            return false;
        }

        std::string db_password_hash = res->getString("password_hash");
        std::string db_salt = res->getString("salt");

        if (db_password_hash != hashPassword(oldPassword, db_salt)) {
            std::cerr << "Error: Old password does not match." << std::endl;
            return false;
        }

        // Old password matches, proceed with updating
        std::string new_salt = generateSalt();
        std::string new_password_hash = hashPassword(newPassword, new_salt);

        std::unique_ptr<sql::PreparedStatement> pstmt_update(
            con->prepareStatement("UPDATE users SET password_hash = ?, salt = ? WHERE id = ?")
        );
        pstmt_update->setString(1, new_password_hash);
        pstmt_update->setString(2, new_salt);
        pstmt_update->setInt(3, id);
        return pstmt_update->executeUpdate() > 0;

    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in updateUserPassword: " << e.what() << std::endl;
        return false;
    }
}
