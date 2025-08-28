#include "expensetrackingpage.h"
#include "database.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateEdit>
#include <QComboBox>
#include <QLabel>
#include <QInputDialog> // Added for editing
#include "expense.h"
#include "user.h"

ExpenseTrackingPage::ExpenseTrackingPage(User* currentUser, QWidget *parent)
    : QWidget(parent), loggedInUser(currentUser)
{
    auto mainLayout = new QVBoxLayout(this);

    // Title
    auto titleLabel = new QLabel("Expense Tracking", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);

    // Input fields for new expense
    auto inputLayout = new QHBoxLayout();
    purchaseDateEdit = new QDateEdit(QDate::currentDate(), this);
    purchaseDateEdit->setCalendarPopup(true);
    inputLayout->addWidget(purchaseDateEdit);

    itemNameLineEdit = new QLineEdit(this);
    itemNameLineEdit->setPlaceholderText("Item Name");
    inputLayout->addWidget(itemNameLineEdit);

    priceLineEdit = new QLineEdit(this);
    priceLineEdit->setPlaceholderText("Price");
    priceLineEdit->setValidator(new QDoubleValidator(0.00, 1000000.00, 2, this)); // Allow only valid prices
    inputLayout->addWidget(priceLineEdit);

    categoryComboBox = new QComboBox(this);
    categoryComboBox->addItem("Food");
    categoryComboBox->addItem("Utilities");
    categoryComboBox->addItem("Rent");
    categoryComboBox->addItem("Other");
    // Add more categories as needed
    inputLayout->addWidget(categoryComboBox);

    addExpenseButton = new QPushButton("Add Expense", this);
    inputLayout->addWidget(addExpenseButton);
    mainLayout->addLayout(inputLayout);

    // Filter by category
    auto filterLayout = new QHBoxLayout();
    filterLayout->addWidget(new QLabel("Filter by Category:"));
    filterCategoryComboBox = new QComboBox(this);
    filterCategoryComboBox->addItem("All");
    filterCategoryComboBox->addItem("Food");
    filterCategoryComboBox->addItem("Utilities");
    filterCategoryComboBox->addItem("Rent");
    filterCategoryComboBox->addItem("Other");
    filterLayout->addWidget(filterCategoryComboBox);
    filterLayout->addStretch();
    mainLayout->addLayout(filterLayout);

    // Table for displaying expenses
    expenseTable = new QTableWidget(this);
    expenseTable->setColumnCount(6);
    expenseTable->setHorizontalHeaderLabels({"ID", "Date", "Item", "Price", "Paid By", "Category"});
    expenseTable->horizontalHeader()->setStretchLastSection(true);
    expenseTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    expenseTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(expenseTable);

    // Action buttons (Edit, Delete, Refresh)
    auto buttonLayout = new QHBoxLayout();
    editExpenseButton = new QPushButton("Edit Selected", this);
    deleteExpenseButton = new QPushButton("Delete Selected", this);
    auto refreshButton = new QPushButton("Refresh", this);

    buttonLayout->addWidget(editExpenseButton);
    buttonLayout->addWidget(deleteExpenseButton);
    buttonLayout->addWidget(refreshButton);
    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(addExpenseButton, &QPushButton::clicked, this, &ExpenseTrackingPage::addExpenseClicked);
    connect(editExpenseButton, &QPushButton::clicked, this, &ExpenseTrackingPage::editExpenseClicked);
    connect(deleteExpenseButton, &QPushButton::clicked, this, &ExpenseTrackingPage::deleteExpenseClicked);
    connect(refreshButton, &QPushButton::clicked, this, &ExpenseTrackingPage::refreshExpenses);
    connect(filterCategoryComboBox, &QComboBox::currentTextChanged, this, &ExpenseTrackingPage::filterExpensesByCategory);

    // Initial load
    loadExpenses();

    setLayout(mainLayout);
}

void ExpenseTrackingPage::loadExpenses(const QString &categoryFilter)
{
    expenseTable->setRowCount(0); // Clear existing rows
    std::vector<Expense> expenses;

    if (categoryFilter == "All") {
        expenses = getAllExpenses();
    } else {
        expenses = getExpensesByCategory(categoryFilter.toStdString());
    }

    expenseTable->setRowCount(expenses.size());

    for (size_t i = 0; i < expenses.size(); ++i) {
        expenseTable->setItem(i, 0, new QTableWidgetItem(QString::number(expenses[i].id)));
        expenseTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(expenses[i].purchase_date)));
        expenseTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(expenses[i].item_name)));
        expenseTable->setItem(i, 3, new QTableWidgetItem(QString::number(expenses[i].price, 'f', 2)));
        expenseTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(expenses[i].paid_by_user_name)));
        expenseTable->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(expenses[i].category)));
    }
}

void ExpenseTrackingPage::addExpenseClicked()
{
    QString date = purchaseDateEdit->date().toString("yyyy-MM-dd");
    QString itemName = itemNameLineEdit->text().trimmed();
    double price = priceLineEdit->text().toDouble();
    QString category = categoryComboBox->currentText();

    if (itemName.isEmpty() || price <= 0) {
        QMessageBox::warning(this, "Input Error", "Item name cannot be empty and price must be greater than 0.");
        return;
    }

    if (addExpense(date.toStdString(), itemName.toStdString(), price, loggedInUser->id, category.toStdString())) {
        QMessageBox::information(this, "Success", "Expense added successfully.");
        itemNameLineEdit->clear();
        priceLineEdit->clear();
        loadExpenses(filterCategoryComboBox->currentText()); // Refresh the table with current filter
    } else {
        QMessageBox::critical(this, "Error", "Failed to add expense.");
    }
}

void ExpenseTrackingPage::editExpenseClicked()
{
    int selectedRow = expenseTable->currentRow();
    if (selectedRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select an expense to edit.");
        return;
    }

    int id = expenseTable->item(selectedRow, 0)->text().toInt();
    QString currentItemName = expenseTable->item(selectedRow, 2)->text();
    double currentPrice = expenseTable->item(selectedRow, 3)->text().toDouble();
    QString currentCategory = expenseTable->item(selectedRow, 5)->text();

    bool ok;
    QString newItemName = QInputDialog::getText(this, "Edit Expense",
                                                "New item name:",
                                                QLineEdit::Normal, currentItemName, &ok);
    if (!ok || newItemName.isEmpty()) return; // User cancelled or entered empty name

    double newPrice = QInputDialog::getDouble(this, "Edit Expense",
                                              "New price:",
                                              currentPrice, 0.01, 1000000.00, 2, &ok);
    if (!ok || newPrice <= 0) return; // User cancelled or entered invalid price

    QStringList categories;
    for (int i = 0; i < categoryComboBox->count(); ++i) {
        if (categoryComboBox->itemText(i) != "All") // Exclude "All" from editable categories
            categories << categoryComboBox->itemText(i);
    }
    QString newCategory = QInputDialog::getItem(this, "Edit Expense",
                                                "New category:",
                                                categories, categories.indexOf(currentCategory), false, &ok);
    if (!ok) return; // User cancelled

    if (editExpense(id, newItemName.toStdString(), newPrice, newCategory.toStdString())) {
        QMessageBox::information(this, "Success", "Expense updated successfully.");
        loadExpenses(filterCategoryComboBox->currentText());
    } else {
        QMessageBox::critical(this, "Error", "Failed to update expense.");
    }
}

void ExpenseTrackingPage::deleteExpenseClicked()
{
    int selectedRow = expenseTable->currentRow();
    if (selectedRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select an expense to delete.");
        return;
    }

    int id = expenseTable->item(selectedRow, 0)->text().toInt();
    QString itemName = expenseTable->item(selectedRow, 2)->text();

    if (QMessageBox::question(this, "Confirm Delete",
                              "Are you sure you want to delete expense '" + itemName + "'?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (deleteExpense(id)) {
            QMessageBox::information(this, "Success", "Expense deleted successfully.");
            loadExpenses(filterCategoryComboBox->currentText());
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete expense.");
        }
    }
}

void ExpenseTrackingPage::refreshExpenses()
{
    loadExpenses(filterCategoryComboBox->currentText());
    QMessageBox::information(this, "Refresh", "Expenses refreshed.");
}

void ExpenseTrackingPage::filterExpensesByCategory(const QString &category)
{
    loadExpenses(category);
}