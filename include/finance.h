#ifndef FINANCE_H
#define FINANCE_H

#include <string>
#include <vector>

struct Payment {
    int id;
    int user_id;
    double amount;
    std::string date;
};

struct FinancialReport {
    int user_id;
    std::string user_name;
    double total_contributions;
    double total_expenses;
    double debt_or_surplus;
};

struct SettlementReport {
    int user_id;
    std::string user_name;
    int total_meals;
    double total_meal_cost;
    double total_payments;
    double total_shopping_expenses;
    double total_contributions;
    double final_balance; // Positive is surplus, negative is debt
};

bool recordPayment(int user_id, double amount, const std::string& date);
std::vector<Payment> getPaymentsByUser(int user_id);
FinancialReport getUserFinancialReport(int user_id);
std::vector<FinancialReport> getAllFinancialReports();
std::pair<double, std::vector<SettlementReport>> generateMonthlySettlement(int period_id);

#endif // FINANCE_H
