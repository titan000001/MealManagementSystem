#ifndef USER_H
#define USER_H

#include <string>
#include <vector>

enum class UserRole { Student, Staff, Admin };

struct User {
    int id;
    std::string username;
    std::string password_hash;
    std::string name;
    UserRole role;
    double total_contributions;
    double total_expenses;
    double debt_or_surplus;
};

bool registerUser(const std::string& username, const std::string& password, const std::string& name, UserRole role);
User* loginUser(const std::string& username, const std::string& password);
std::vector<User> getAllUsers();
User* getUserById(int id);
bool updateUserProfile(int id, const std::string& name);

#endif // USER_H
