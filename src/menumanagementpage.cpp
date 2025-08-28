#include "menumanagementpage.h"
#include "database.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QInputDialog> // Added for editing
#include "menu.h"

MenuManagementPage::MenuManagementPage(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);

    // Title
    auto titleLabel = new QLabel("Menu Management", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);

    // Input for new menu item
    auto inputLayout = new QHBoxLayout();
    menuItemNameLineEdit = new QLineEdit(this);
    menuItemNameLineEdit->setPlaceholderText("Enter new menu item name");
    inputLayout->addWidget(menuItemNameLineEdit);

    addMenuItemButton = new QPushButton("Add Item", this);
    inputLayout->addWidget(addMenuItemButton);
    mainLayout->addLayout(inputLayout);

    // Table for displaying menu items
    menuTable = new QTableWidget(this);
    menuTable->setColumnCount(2);
    menuTable->setHorizontalHeaderLabels({"ID", "Name"});
    menuTable->horizontalHeader()->setStretchLastSection(true);
    menuTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    menuTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(menuTable);

    // Action buttons (Edit, Delete, Refresh)
    auto buttonLayout = new QHBoxLayout();
    editMenuItemButton = new QPushButton("Edit Selected", this);
    deleteMenuItemButton = new QPushButton("Delete Selected", this);
    auto refreshButton = new QPushButton("Refresh", this);

    buttonLayout->addWidget(editMenuItemButton);
    buttonLayout->addWidget(deleteMenuItemButton);
    buttonLayout->addWidget(refreshButton);
    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(addMenuItemButton, &QPushButton::clicked, this, &MenuManagementPage::addMenuItemClicked);
    connect(editMenuItemButton, &QPushButton::clicked, this, &MenuManagementPage::editMenuItemClicked);
    connect(deleteMenuItemButton, &QPushButton::clicked, this, &MenuManagementPage::deleteMenuItemClicked);
    connect(refreshButton, &QPushButton::clicked, this, &MenuManagementPage::refreshMenuItems);

    // Initial load
    loadMenuItems();

    setLayout(mainLayout);
}

void MenuManagementPage::loadMenuItems()
{
    menuTable->setRowCount(0); // Clear existing rows
    std::vector<MenuItem> items = getAllMenuItems();
    menuTable->setRowCount(items.size());

    for (size_t i = 0; i < items.size(); ++i) {
        menuTable->setItem(i, 0, new QTableWidgetItem(QString::number(items[i].id)));
        menuTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(items[i].name)));
    }
}

void MenuManagementPage::addMenuItemClicked()
{
    QString itemName = menuItemNameLineEdit->text().trimmed();
    if (itemName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Menu item name cannot be empty.");
        return;
    }

    if (addMenuItem(itemName.toStdString())) {
        QMessageBox::information(this, "Success", "Menu item added successfully.");
        menuItemNameLineEdit->clear();
        loadMenuItems(); // Refresh the table
    } else {
        QMessageBox::critical(this, "Error", "Failed to add menu item. It might already exist.");
    }
}

void MenuManagementPage::editMenuItemClicked()
{
    int selectedRow = menuTable->currentRow();
    if (selectedRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select a menu item to edit.");
        return;
    }

    int id = menuTable->item(selectedRow, 0)->text().toInt();
    QString currentName = menuTable->item(selectedRow, 1)->text();

    bool ok;
    QString newName = QInputDialog::getText(this, "Edit Menu Item",
                                            "New name for '" + currentName + "':",
                                            QLineEdit::Normal, currentName, &ok);

    if (ok && !newName.isEmpty() && newName != currentName) {
        if (editMenuItem(id, newName.toStdString())) {
            QMessageBox::information(this, "Success", "Menu item updated successfully.");
            loadMenuItems();
        } else {
            QMessageBox::critical(this, "Error", "Failed to update menu item.");
        }
    } else if (ok && newName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Menu item name cannot be empty.");
    }
}

void MenuManagementPage::deleteMenuItemClicked()
{
    int selectedRow = menuTable->currentRow();
    if (selectedRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select a menu item to delete.");
        return;
    }

    int id = menuTable->item(selectedRow, 0)->text().toInt();
    QString name = menuTable->item(selectedRow, 1)->text();

    if (QMessageBox::question(this, "Confirm Delete",
                              "Are you sure you want to delete '" + name + "'?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (deleteMenuItem(id)) {
            QMessageBox::information(this, "Success", "Menu item deleted successfully.");
            loadMenuItems();
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete menu item.");
        }
    }
}

void MenuManagementPage::refreshMenuItems()
{
    loadMenuItems();
    QMessageBox::information(this, "Refresh", "Menu items refreshed.");
}