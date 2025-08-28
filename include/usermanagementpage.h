#ifndef USERMANAGEMENTPAGE_H
#define USERMANAGEMENTPAGE_H

#include <QWidget>

class QTableWidget;
class QLineEdit;
class QComboBox;
class QPushButton;

class UserManagementPage : public QWidget
{
    Q_OBJECT

public:
    explicit UserManagementPage(QWidget *parent = nullptr);

private slots:
    void registerUserClicked();
    void refreshUsers();

private:
    void loadUsers();

    QTableWidget *userTable;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QLineEdit *nameLineEdit;
    QComboBox *roleComboBox;
    QPushButton *registerButton;
};

#endif // USERMANAGEMENTPAGE_H