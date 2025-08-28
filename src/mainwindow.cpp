#include "mainwindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QApplication>
#include "user.h"
#include "menumanagementpage.h"
#include "expensetrackingpage.h"
#include "mealattendancepage.h"
#include "usermanagementpage.h"
#include "financialoverviewpage.h"
#include "dailymenupage.h"
#include "menuhistorypage.h"

MainWindow::MainWindow(User* userPtr, QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Meal Management Dashboard");
    setMinimumSize(800, 600);

    // Main horizontal layout for sidebar and content
    auto mainLayout = new QHBoxLayout(this);

    // --- Sidebar ---
    auto sidebarLayout = new QVBoxLayout();
    sidebar = new QListWidget(this);
    sidebar->setFixedWidth(150);
    sidebar->addItem("Dashboard");
    sidebar->addItem("User Profile");
    sidebar->addItem("Menu Management");
    sidebar->addItem("Expense Tracking");
    sidebar->addItem("Meal Attendance");
    sidebar->addItem("Daily Menu");
    sidebar->addItem("Menu History");
    sidebar->addItem("User Management");
    sidebar->addItem("Financial Overview");
    
    auto exitButton = new QPushButton("Exit", this);
    connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    sidebarLayout->addWidget(sidebar);
    sidebarLayout->addStretch(); // Pushes the exit button to the bottom
    sidebarLayout->addWidget(exitButton);


    // Stacked Widget for content pages
    stackedWidget = new QStackedWidget(this);

    // --- Placeholder Pages ---
    // Dashboard Page
    auto dashboardPage = new QWidget();
    auto dashboardLayout = new QVBoxLayout(dashboardPage);
    auto welcomeLabel = new QLabel("Welcome, " + QString::fromStdString(userPtr->name) + "!", dashboardPage);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    dashboardLayout->addWidget(welcomeLabel);
    dashboardPage->setLayout(dashboardLayout);
    stackedWidget->addWidget(dashboardPage);

    // User Profile Page
    auto userProfilePage = new UserProfilePage(userPtr);
    stackedWidget->addWidget(userProfilePage);

    // Menu Management Page
    auto menuManagementPage = new MenuManagementPage();
    stackedWidget->addWidget(menuManagementPage);

    // Expense Tracking Page
    auto expenseTrackingPage = new ExpenseTrackingPage(userPtr);
    stackedWidget->addWidget(expenseTrackingPage);

    // Meal Attendance Page
    auto mealAttendancePage = new MealAttendancePage();
    stackedWidget->addWidget(mealAttendancePage);

    // Daily Menu Page
    auto dailyMenuPage = new DailyMenuPage();
    stackedWidget->addWidget(dailyMenuPage);

    // Menu History Page
    auto menuHistoryPage = new MenuHistoryPage();
    stackedWidget->addWidget(menuHistoryPage);

    // User Management Page
    auto userManagementPage = new UserManagementPage();
    stackedWidget->addWidget(userManagementPage);

    // Financial Overview Page
    auto financialOverviewPage = new FinancialOverviewPage();
    stackedWidget->addWidget(financialOverviewPage);

    // Connect sidebar selection to stacked widget page change
    connect(sidebar, &QListWidget::currentRowChanged, this, &MainWindow::changePage);

    // Set initial selection
    sidebar->setCurrentRow(0);

    // Add sidebar and stacked widget to the main layout
    mainLayout->addLayout(sidebarLayout);
    mainLayout->addWidget(stackedWidget);

    setLayout(mainLayout);
}

void MainWindow::changePage(int index)
{
    stackedWidget->setCurrentIndex(index);
}
