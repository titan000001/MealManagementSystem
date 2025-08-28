#include "userprofilepage.h"
#include "user.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

// Helper function to convert UserRole enum to QString
QString userRoleToString(UserRole role)
{
    switch (role) {
        case UserRole::Student: return "Student";
        case UserRole::Staff: return "Staff";
        case UserRole::Admin: return "Admin";
        default: return "Unknown";
    }
}

UserProfilePage::UserProfilePage(User* user, QWidget *parent)
    : QWidget(parent), currentUser(user)
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // Title
    auto titleLabel = new QLabel("User Profile", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);

    // Username
    auto usernameLayout = new QHBoxLayout();
    usernameLayout->addWidget(new QLabel("Username:"));
    usernameLabel = new QLabel(QString::fromStdString(currentUser->name), this);
    usernameLayout->addWidget(usernameLabel);
    mainLayout->addLayout(usernameLayout);

    // Role
    auto roleLayout = new QHBoxLayout();
    roleLayout->addWidget(new QLabel("Role:"));
    roleLabel = new QLabel(userRoleToString(currentUser->role), this);
    roleLayout->addWidget(roleLabel);
    mainLayout->addLayout(roleLayout);

    // Save Button (placeholder for now)
    auto saveButton = new QPushButton("Save Profile", this);
    mainLayout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &UserProfilePage::saveProfile);

    setLayout(mainLayout);
}

void UserProfilePage::saveProfile()
{
    QMessageBox::information(this, "Save Profile", "Save functionality not yet implemented.");
    // Future: Implement saving logic here
}

void UserProfilePage::updatePassword()
{
    QMessageBox::information(this, "Update Password", "Update password functionality not yet implemented.");
    // Future: Implement password update logic here
}