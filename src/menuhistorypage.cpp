#include "menuhistorypage.h"
#include "database.h"
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include "menu.h"

MenuHistoryPage::MenuHistoryPage(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);

    // Title
    auto titleLabel = new QLabel("Menu History", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);

    // Table for displaying historical menus
    historyTable = new QTableWidget(this);
    historyTable->setColumnCount(4); // Date, Breakfast, Lunch, Dinner
    historyTable->setHorizontalHeaderLabels({"Date", "Breakfast", "Lunch", "Dinner"});
    historyTable->horizontalHeader()->setStretchLastSection(true);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(historyTable);

    // Initial load
    loadMenuHistory();

    setLayout(mainLayout);
}

void MenuHistoryPage::loadMenuHistory()
{
    historyTable->setRowCount(0); // Clear existing rows
    std::vector<DailyMenu> history = getMenuHistory();
    historyTable->setRowCount(history.size());

    for (size_t i = 0; i < history.size(); ++i) {
        historyTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(history[i].date)));

        QString breakfastItems;
        for (const auto& item : history[i].breakfast) {
            breakfastItems += QString::fromStdString(item.name) + "; ";
        }
        if (!breakfastItems.isEmpty()) breakfastItems.chop(2); // Remove trailing "; "
        historyTable->setItem(i, 1, new QTableWidgetItem(breakfastItems));

        QString lunchItems;
        for (const auto& item : history[i].lunch) {
            lunchItems += QString::fromStdString(item.name) + "; ";
        }
        if (!lunchItems.isEmpty()) lunchItems.chop(2);
        historyTable->setItem(i, 2, new QTableWidgetItem(lunchItems));

        QString dinnerItems;
        for (const auto& item : history[i].dinner) {
            dinnerItems += QString::fromStdString(item.name) + "; ";
        }
        if (!dinnerItems.isEmpty()) dinnerItems.chop(2);
        historyTable->setItem(i, 3, new QTableWidgetItem(dinnerItems));
    }
}