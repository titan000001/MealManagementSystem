#include "usermanagementpage.h"
#include "user.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel> // Added missing include
#include "database.h"

// Helper function to convert UserRole enum to QString (re-using from UserProfilePage)
QString userRoleToString(UserRole role);

UserManagementPage::UserManagementPage(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);

    // Title
    auto titleLabel = new QLabel("User Management", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);

    // Input fields for new user registration
    auto registerLayout = new QVBoxLayout();
    registerLayout->addWidget(new QLabel("Register New User:"));

    usernameLineEdit = new QLineEdit(this);
    usernameLineEdit->setPlaceholderText("Username");
    registerLayout->addWidget(usernameLineEdit);

    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setPlaceholderText("Password");
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    registerLayout->addWidget(passwordLineEdit);

    nameLineEdit = new QLineEdit(this);
    nameLineEdit->setPlaceholderText("Full Name");
    registerLayout->addWidget(nameLineEdit);

    roleComboBox = new QComboBox(this);
    roleComboBox->addItem("Student", QVariant::fromValue(UserRole::Student));
    roleComboBox->addItem("Staff", QVariant::fromValue(UserRole::Staff));
    roleComboBox->addItem("Admin", QVariant::fromValue(UserRole::Admin));
    registerLayout->addWidget(roleComboBox);

    registerButton = new QPushButton("Register User", this);
    registerLayout->addWidget(registerButton);
    mainLayout->addLayout(registerLayout);
    mainLayout->addSpacing(20);

    // Table for displaying users
    userTable = new QTableWidget(this);
    userTable->setColumnCount(4); // ID, Username, Name, Role
    userTable->setHorizontalHeaderLabels({"ID", "Username", "Name", "Role"});
    userTable->horizontalHeader()->setStretchLastSection(true);
    userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(userTable);

    // Refresh button
    auto refreshButton = new QPushButton("Refresh Users", this);
    mainLayout->addWidget(refreshButton);

    // Connections
    connect(registerButton, &QPushButton::clicked, this, &UserManagementPage::registerUserClicked);
    connect(refreshButton, &QPushButton::clicked, this, &UserManagementPage::refreshUsers);

    // Initial load
    loadUsers();

    setLayout(mainLayout);
}

void UserManagementPage::loadUsers()
{
    userTable->setRowCount(0); // Clear existing rows
    std::vector<User> users = getAllUsers();
    userTable->setRowCount(users.size());

    for (size_t i = 0; i < users.size(); ++i) {
        userTable->setItem(i, 0, new QTableWidgetItem(QString::number(users[i].id)));
        userTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(users[i].username)));
        userTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(users[i].name)));
        userTable->setItem(i, 3, new QTableWidgetItem(userRoleToString(users[i].role)));
    }
}

void UserManagementPage::registerUserClicked()
{
    QString username = usernameLineEdit->text().trimmed();
    QString password = passwordLineEdit->text();
    QString name = nameLineEdit->text().trimmed();
    UserRole role = roleComboBox->currentData().value<UserRole>();

    if (username.isEmpty() || password.isEmpty() || name.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "All fields must be filled to register a user.");
        return;
    }

    if (registerUser(username.toStdString(), password.toStdString(), name.toStdString(), role)) {
        QMessageBox::information(this, "Success", "User registered successfully.");
        usernameLineEdit->clear();
        passwordLineEdit->clear();
        nameLineEdit->clear();
        loadUsers(); // Refresh the table
    } else {
        QMessageBox::critical(this, "Error", "Failed to register user. Username might already exist.");
    }
}

void UserManagementPage::refreshUsers()
{
    loadUsers();
    QMessageBox::information(this, "Refresh", "User list refreshed.");
}