#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <memory>
#include "user.h" // To use the User struct
#include "userprofilepage.h"
#include "menumanagementpage.h"
#include "expensetrackingpage.h"
#include "mealattendancepage.h"
#include "usermanagementpage.h"
#include "financialoverviewpage.h"
#include "dailymenupage.h"
#include "menuhistorypage.h"

class QLabel;
class QListWidget;
class QStackedWidget;
class QHBoxLayout;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(User* user, QWidget *parent = nullptr);

private slots:
    void changePage(int index);

private:
    QListWidget *sidebar;
    QStackedWidget *stackedWidget;
};

#endif // MAINWINDOW_H