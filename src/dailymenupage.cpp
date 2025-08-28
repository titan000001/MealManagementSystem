#include "dailymenupage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateEdit>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include "database.h"

DailyMenuPage::DailyMenuPage(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);

    // Title
    auto titleLabel = new QLabel("Daily Menu Management", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);

    // Date selection
    auto dateLayout = new QHBoxLayout();
    menuDateEdit = new QDateEdit(QDate::currentDate(), this);
    menuDateEdit->setCalendarPopup(true);
    dateLayout->addWidget(new QLabel("Date:"));
    dateLayout->addWidget(menuDateEdit);
    dateLayout->addStretch();
    mainLayout->addLayout(dateLayout);

    // Main content area: Available items and meal lists
    auto contentLayout = new QHBoxLayout();

    // Available Menu Items
    auto availableLayout = new QVBoxLayout();
    availableLayout->addWidget(new QLabel("Available Menu Items:"));
    availableMenuItemsList = new QListWidget(this);
    availableLayout->addWidget(availableMenuItemsList);
    contentLayout->addLayout(availableLayout);

    // Buttons to move items
    auto buttonColumnLayout = new QVBoxLayout();
    buttonColumnLayout->addStretch();
    QPushButton *addBreakfastBtn = new QPushButton("-> Breakfast", this);
    QPushButton *removeBreakfastBtn = new QPushButton("<- Breakfast", this);
    QPushButton *addLunchBtn = new QPushButton("-> Lunch", this);
    QPushButton *removeLunchBtn = new QPushButton("<- Lunch", this);
    QPushButton *addDinnerBtn = new QPushButton("-> Dinner", this);
    QPushButton *removeDinnerBtn = new QPushButton("<- Dinner", this);

    buttonColumnLayout->addWidget(addBreakfastBtn);
    buttonColumnLayout->addWidget(removeBreakfastBtn);
    buttonColumnLayout->addWidget(addLunchBtn);
    buttonColumnLayout->addWidget(removeLunchBtn);
    buttonColumnLayout->addWidget(addDinnerBtn);
    buttonColumnLayout->addWidget(removeDinnerBtn);
    buttonColumnLayout->addStretch();
    contentLayout->addLayout(buttonColumnLayout);

    // Meal Lists
    auto mealListsLayout = new QVBoxLayout();
    auto breakfastLayout = new QVBoxLayout();
    breakfastLayout->addWidget(new QLabel("Breakfast:"));
    breakfastList = new QListWidget(this);
    breakfastLayout->addWidget(breakfastList);
    mealListsLayout->addLayout(breakfastLayout);

    auto lunchLayout = new QVBoxLayout();
    lunchLayout->addWidget(new QLabel("Lunch:"));
    lunchList = new QListWidget(this);
    lunchLayout->addWidget(lunchList);
    mealListsLayout->addLayout(lunchLayout);

    auto dinnerLayout = new QVBoxLayout();
    dinnerLayout->addWidget(new QLabel("Dinner:"));
    dinnerList = new QListWidget(this);
    dinnerLayout->addWidget(dinnerList);
    mealListsLayout->addLayout(dinnerLayout);

    contentLayout->addLayout(mealListsLayout);
    mainLayout->addLayout(contentLayout);

    // Save button
    saveMenuButton = new QPushButton("Save Daily Menu", this);
    mainLayout->addWidget(saveMenuButton);

    // Connections
    connect(menuDateEdit, &QDateEdit::dateChanged, this, &DailyMenuPage::loadDailyMenu);
    connect(saveMenuButton, &QPushButton::clicked, this, &DailyMenuPage::saveDailyMenuClicked);
    connect(addBreakfastBtn, &QPushButton::clicked, this, &DailyMenuPage::addBreakfastItem);
    connect(removeBreakfastBtn, &QPushButton::clicked, this, &DailyMenuPage::removeBreakfastItem);
    connect(addLunchBtn, &QPushButton::clicked, this, &DailyMenuPage::addLunchItem);
    connect(removeLunchBtn, &QPushButton::clicked, this, &DailyMenuPage::removeLunchItem);
    connect(addDinnerBtn, &QPushButton::clicked, this, &DailyMenuPage::addDinnerItem);
    connect(removeDinnerBtn, &QPushButton::clicked, this, &DailyMenuPage::removeDinnerItem);

    // Initial load
    loadAvailableMenuItems();
    loadDailyMenu();

    setLayout(mainLayout);
}

void DailyMenuPage::loadAvailableMenuItems()
{
    availableMenuItemsList->clear();
    std::vector<MenuItem> items = getAllMenuItems();
    for (const auto& item : items) {
        QListWidgetItem *listItem = new QListWidgetItem(QString::fromStdString(item.name));
        listItem->setData(Qt::UserRole, item.id); // Store ID in UserRole
        availableMenuItemsList->addItem(listItem);
    }
}

void DailyMenuPage::loadDailyMenu()
{
    breakfastList->clear();
    lunchList->clear();
    dinnerList->clear();

    QString selectedDate = menuDateEdit->date().toString("yyyy-MM-dd");
    DailyMenu dailyMenu = getDailyMenu(selectedDate.toStdString());

    for (const auto& item : dailyMenu.breakfast) {
        QListWidgetItem *listItem = new QListWidgetItem(QString::fromStdString(item.name));
        listItem->setData(Qt::UserRole, item.id);
        breakfastList->addItem(listItem);
    }
    for (const auto& item : dailyMenu.lunch) {
        QListWidgetItem *listItem = new QListWidgetItem(QString::fromStdString(item.name));
        listItem->setData(Qt::UserRole, item.id);
        lunchList->addItem(listItem);
    }
    for (const auto& item : dailyMenu.dinner) {
        QListWidgetItem *listItem = new QListWidgetItem(QString::fromStdString(item.name));
        listItem->setData(Qt::UserRole, item.id);
        dinnerList->addItem(listItem);
    }
}

std::vector<int> DailyMenuPage::getMenuItemIds(QListWidget* listWidget)
{
    std::vector<int> ids;
    for (int i = 0; i < listWidget->count(); ++i) {
        ids.push_back(listWidget->item(i)->data(Qt::UserRole).toInt());
    }
    return ids;
}

void DailyMenuPage::saveDailyMenuClicked()
{
    QString selectedDate = menuDateEdit->date().toString("yyyy-MM-dd");
    std::vector<int> breakfastIds = getMenuItemIds(breakfastList);
    std::vector<int> lunchIds = getMenuItemIds(lunchList);
    std::vector<int> dinnerIds = getMenuItemIds(dinnerList);

    if (setDailyMenu(selectedDate.toStdString(), breakfastIds, lunchIds, dinnerIds)) {
        QMessageBox::information(this, "Success", "Daily menu saved successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to save daily menu.");
    }
}

void DailyMenuPage::addBreakfastItem()
{
    QListWidgetItem *selectedItem = availableMenuItemsList->currentItem();
    if (selectedItem) {
        QListWidgetItem *newItem = selectedItem->clone();
        breakfastList->addItem(newItem);
        delete availableMenuItemsList->takeItem(availableMenuItemsList->row(selectedItem));
    }
}

void DailyMenuPage::removeBreakfastItem()
{
    QListWidgetItem *selectedItem = breakfastList->currentItem();
    if (selectedItem) {
        QListWidgetItem *newItem = selectedItem->clone();
        availableMenuItemsList->addItem(newItem);
        delete breakfastList->takeItem(breakfastList->row(selectedItem));
    }
}

void DailyMenuPage::addLunchItem()
{
    QListWidgetItem *selectedItem = availableMenuItemsList->currentItem();
    if (selectedItem) {
        QListWidgetItem *newItem = selectedItem->clone();
        lunchList->addItem(newItem);
        delete availableMenuItemsList->takeItem(availableMenuItemsList->row(selectedItem));
    }
}

void DailyMenuPage::removeLunchItem()
{
    QListWidgetItem *selectedItem = lunchList->currentItem();
    if (selectedItem) {
        QListWidgetItem *newItem = selectedItem->clone();
        availableMenuItemsList->addItem(newItem);
        delete lunchList->takeItem(lunchList->row(selectedItem));
    }
}

void DailyMenuPage::addDinnerItem()
{
    QListWidgetItem *selectedItem = availableMenuItemsList->currentItem();
    if (selectedItem) {
        QListWidgetItem *newItem = selectedItem->clone();
        dinnerList->addItem(newItem);
        delete availableMenuItemsList->takeItem(availableMenuItemsList->row(selectedItem));
    }
}

void DailyMenuPage::removeDinnerItem()
{
    QListWidgetItem *selectedItem = dinnerList->currentItem();
    if (selectedItem) {
        QListWidgetItem *newItem = selectedItem->clone();
        availableMenuItemsList->addItem(newItem);
        delete dinnerList->takeItem(dinnerList->row(selectedItem));
    }
}