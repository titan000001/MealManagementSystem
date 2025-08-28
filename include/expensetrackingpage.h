#ifndef EXPENSETRACKINGPAGE_H
#define EXPENSETRACKINGPAGE_H

#include <QWidget>
#include "user.h"

class QTableWidget;
class QLineEdit;
class QComboBox;
class QDateEdit;
class QPushButton;

class ExpenseTrackingPage : public QWidget
{
    Q_OBJECT

public:
    explicit ExpenseTrackingPage(User* currentUser, QWidget *parent = nullptr);

private slots:
    void addExpenseClicked();
    void editExpenseClicked();
    void deleteExpenseClicked();
    void refreshExpenses();
    void filterExpensesByCategory(const QString &category);

private:
    void loadExpenses(const QString &categoryFilter = "All");
    User* loggedInUser;

    QTableWidget *expenseTable;
    QDateEdit *purchaseDateEdit;
    QLineEdit *itemNameLineEdit;
    QLineEdit *priceLineEdit;
    QComboBox *categoryComboBox;
    QPushButton *addExpenseButton;
    QPushButton *editExpenseButton;
    QPushButton *deleteExpenseButton;
    QComboBox *filterCategoryComboBox;
};

#endif // EXPENSETRACKINGPAGE_H