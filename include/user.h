#ifndef USER_H
#define USER_H

#include <string>
#include <memory>
#include <vector>

#include <QMetaType> // Required for Q_DECLARE_METATYPE

enum class UserRole { Student, Staff, Admin };

struct User {
    int id;
    std::string username;
    std::string password_hash;
    std::string salt;
    std::string name;
    UserRole role;
};

Q_DECLARE_METATYPE(std::unique_ptr<User>);

bool registerUser(const std::string& username, const std::string& password, const std::string& name, UserRole role);
std::unique_ptr<User> loginUser(const std::string& username, const std::string& password);
std::vector<User> getAllUsers();
std::unique_ptr<User> getUserById(int id);
bool updateUserProfile(int id, const std::string& name);
bool updateUserPassword(int id, const std::string& oldPassword, const std::string& newPassword);

#endif // USER_H
