#ifndef FINANCIALOVERVIEWPAGE_H
#define FINANCIALOVERVIEWPAGE_H

#include <QWidget>

class QTableWidget;
class QPushButton;
class QLineEdit;
class QDateEdit;

class FinancialOverviewPage : public QWidget
{
    Q_OBJECT

public:
    explicit FinancialOverviewPage(QWidget *parent = nullptr);

private slots:
    void loadFinancialReports();
    void recordPaymentClicked();

private:
    QTableWidget *financialReportTable;
    QLineEdit *paymentUserIdLineEdit;
    QLineEdit *paymentAmountLineEdit;
    QDateEdit *paymentDateEdit;
    QPushButton *recordPaymentButton;
};

#endif // FINANCIALOVERVIEWPAGE_H