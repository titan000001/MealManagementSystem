#include "financialoverviewpage.h"
#include "finance.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateEdit>
#include <QLabel> // Added missing include
#include "database.h"

FinancialOverviewPage::FinancialOverviewPage(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);

    // Title
    auto titleLabel = new QLabel("Financial Overview", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);

    // Table for displaying financial reports
    financialReportTable = new QTableWidget(this);
    financialReportTable->setColumnCount(4); // User Name, Total Contributions, Total Expenses, Debt/Surplus
    financialReportTable->setHorizontalHeaderLabels({"User Name", "Contributions", "Expenses", "Balance"});
    financialReportTable->horizontalHeader()->setStretchLastSection(true);
    financialReportTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    financialReportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(financialReportTable);

    // Refresh button for reports
    auto refreshReportsButton = new QPushButton("Refresh Reports", this);
    mainLayout->addWidget(refreshReportsButton);
    mainLayout->addSpacing(20);

    // Input fields for recording payments
    auto paymentLayout = new QHBoxLayout();
    paymentLayout->addWidget(new QLabel("Record Payment:"));

    paymentUserIdLineEdit = new QLineEdit(this);
    paymentUserIdLineEdit->setPlaceholderText("User ID");
    paymentUserIdLineEdit->setValidator(new QIntValidator(1, 999999, this));
    paymentLayout->addWidget(paymentUserIdLineEdit);

    paymentAmountLineEdit = new QLineEdit(this);
    paymentAmountLineEdit->setPlaceholderText("Amount");
    paymentAmountLineEdit->setValidator(new QDoubleValidator(0.01, 1000000.00, 2, this));
    paymentLayout->addWidget(paymentAmountLineEdit);

    paymentDateEdit = new QDateEdit(QDate::currentDate(), this);
    paymentDateEdit->setCalendarPopup(true);
    paymentLayout->addWidget(paymentDateEdit);

    recordPaymentButton = new QPushButton("Record Payment", this);
    paymentLayout->addWidget(recordPaymentButton);
    mainLayout->addLayout(paymentLayout);

    // Connections
    connect(refreshReportsButton, &QPushButton::clicked, this, &FinancialOverviewPage::loadFinancialReports);
    connect(recordPaymentButton, &QPushButton::clicked, this, &FinancialOverviewPage::recordPaymentClicked);

    // Initial load
    loadFinancialReports();

    setLayout(mainLayout);
}

void FinancialOverviewPage::loadFinancialReports()
{
    financialReportTable->setRowCount(0); // Clear existing rows
    std::vector<FinancialReport> reports = getAllFinancialReports();
    financialReportTable->setRowCount(reports.size());

    for (size_t i = 0; i < reports.size(); ++i) {
        financialReportTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(reports[i].user_name)));
        financialReportTable->setItem(i, 1, new QTableWidgetItem(QString::number(reports[i].total_contributions, 'f', 2)));
        financialReportTable->setItem(i, 2, new QTableWidgetItem(QString::number(reports[i].total_expenses, 'f', 2)));
        financialReportTable->setItem(i, 3, new QTableWidgetItem(QString::number(reports[i].debt_or_surplus, 'f', 2)));
    }
}

void FinancialOverviewPage::recordPaymentClicked()
{
    int userId = paymentUserIdLineEdit->text().toInt();
    double amount = paymentAmountLineEdit->text().toDouble();
    QString date = paymentDateEdit->date().toString("yyyy-MM-dd");

    if (userId <= 0 || amount <= 0) {
        QMessageBox::warning(this, "Input Error", "User ID and Amount must be valid.");
        return;
    }

    if (recordPayment(userId, amount, date.toStdString())) {
        QMessageBox::information(this, "Success", "Payment recorded successfully.");
        paymentUserIdLineEdit->clear();
        paymentAmountLineEdit->clear();
        loadFinancialReports(); // Refresh the table
    } else {
        QMessageBox::critical(this, "Error", "Failed to record payment.");
    }
}