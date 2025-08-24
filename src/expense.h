#ifndef EXPENSE_H
#define EXPENSE_H

#include <string>
#include <vector>

struct Expense {
    int id;
    std::string date;
    std::string item;
    double price;
    int paid_by_user_id;
    std::string category;
};

bool addExpense(const std::string& date, const std::string& item, double price, int paid_by_user_id, const std::string& category);
bool editExpense(int id, const std::string& item, double price, const std::string& category);
bool deleteExpense(int id);
std::vector<Expense> getAllExpenses();
std::vector<Expense> getExpensesByCategory(const std::string& category);

#endif // EXPENSE_H
