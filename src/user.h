#ifndef USER_H
#define USER_H

#include <string>
#include <memory>
#include <vector>

enum class UserRole { Student, Staff, Admin };

struct User {
    int id;
    std::string username;
    std::string password_hash;
    std::string salt;
    std::string name;
    UserRole role;
};

std::string roleToString(UserRole role);

bool registerUser(const std::string& username, const std::string& password, const std::string& name, UserRole role);
std::unique_ptr<User> loginUser(const std::string& username, const std::string& password);
std::vector<User> getAllUsers();
std::unique_ptr<User> getUserById(int id);
bool updateUserProfile(int id, const std::string& name);

#endif // USER_H
