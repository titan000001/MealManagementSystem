#include "loginwindow.h"
#include "user.h" // Your existing user logic

// Qt includes for UI elements and layout
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    // Create UI elements
    auto *usernameLabel = new QLabel("Username:");
    usernameEdit = new QLineEdit();
    auto *passwordLabel = new QLabel("Password:");
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password); // Hides password text

    auto *loginButton = new QPushButton("Login");

    // Create layouts
    auto *mainLayout = new QVBoxLayout();
    auto *formLayout = new QHBoxLayout();

    // Assemble the form
    auto *formLeft = new QVBoxLayout();
    formLeft->addWidget(usernameLabel);
    formLeft->addWidget(passwordLabel);

    auto *formRight = new QVBoxLayout();
    formRight->addWidget(usernameEdit);
    formRight->addWidget(passwordEdit);

    formLayout->addLayout(formLeft);
    formLayout->addLayout(formRight);

    // Add form and button to the main layout
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(loginButton);

    setLayout(mainLayout);
    setWindowTitle("Meal Management System - Login");

    // Connect the button's "clicked" signal to our "handleLoginAttempt" slot
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::handleLoginAttempt);
}

void LoginWindow::handleLoginAttempt() {
    std::string username = usernameEdit->text().toStdString();
    std::string password = passwordEdit->text().toStdString();

    // Call your existing backend function!
    if (auto user = loginUser(username, password)) { // `user` is a std::unique_ptr<User>
        emit loginSuccess(user.release()); // Emit the signal with the user data
        close();
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }
}