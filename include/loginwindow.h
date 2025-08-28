#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include "user.h" // For User struct and unique_ptr
#include <QWidget>

// Forward declarations for Qt UI elements to reduce header includes
class QLineEdit;
class QPushButton;
class QLabel;

class LoginWindow : public QWidget {
    Q_OBJECT // Required macro for any class that uses Qt's signals and slots

public:
    explicit LoginWindow(QWidget *parent = nullptr);

signals:
    void loginSuccess(User* user); // Signal to emit on successful login

private slots:
    void handleLoginAttempt(); // This function is a "slot" that will be called on button click

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
};

#endif // LOGINWINDOW_H