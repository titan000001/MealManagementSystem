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
    double total_contributions;
    double total_expenses;
    double debt_or_surplus;
};

bool recordPayment(int user_id, double amount, const std::string& date);
std::vector<Payment> getPaymentsByUser(int user_id);
FinancialReport getUserFinancialReport(int user_id);
std::vector<FinancialReport> getAllFinancialReports();

#endif // FINANCE_H
