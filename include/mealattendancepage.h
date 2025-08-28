#ifndef MEALATTENDANCEPAGE_H
#define MEALATTENDANCEPAGE_H

#include <QWidget>
#include <vector>
#include "user.h"

class QTableWidget;
class QDateEdit;
class QComboBox;
class QPushButton;
class QCheckBox;

class MealAttendancePage : public QWidget
{
    Q_OBJECT

public:
    explicit MealAttendancePage(QWidget *parent = nullptr);

private slots:
    void loadAttendanceForDate();
    void recordAttendanceClicked();

private:
    QDateEdit *attendanceDateEdit;
    QTableWidget *userAttendanceTable;
    QPushButton *recordAttendanceButton;

    std::vector<User> allUsers;
};

#endif // MEALATTENDANCEPAGE_H