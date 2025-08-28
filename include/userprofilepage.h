#ifndef USERPROFILEPAGE_H
#define USERPROFILEPAGE_H

#include <QWidget>
#include "user.h"

class QLabel;
class QLineEdit;
class QPushButton;

class UserProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit UserProfilePage(User* user, QWidget *parent = nullptr);

private slots:
    void saveProfile();
    void updatePassword();

private:
    User* currentUser;
    QLabel* usernameLabel;
    QLabel* roleLabel;
    // Add QLineEdit for editable fields if needed later
};

#endif // USERPROFILEPAGE_H