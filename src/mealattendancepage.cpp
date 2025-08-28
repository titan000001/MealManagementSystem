#include "mealattendancepage.h"
#include "attendance.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QCheckBox>
#include <QLabel> // Added missing include
#include <set>    // For efficient lookups
#include <utility> // For std::pair
#include "database.h"
#include "user.h"

MealAttendancePage::MealAttendancePage(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);

    // Title
    auto titleLabel = new QLabel("Meal Attendance", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);

    // Date selection and refresh
    auto dateLayout = new QHBoxLayout();
    attendanceDateEdit = new QDateEdit(QDate::currentDate(), this);
    attendanceDateEdit->setCalendarPopup(true);
    dateLayout->addWidget(new QLabel("Date:"));
    dateLayout->addWidget(attendanceDateEdit);
    dateLayout->addStretch();
    mainLayout->addLayout(dateLayout);

    // User attendance table
    userAttendanceTable = new QTableWidget(this);
    userAttendanceTable->setColumnCount(4); // User Name, Breakfast, Lunch, Dinner
    userAttendanceTable->setHorizontalHeaderLabels({"User Name", "Breakfast", "Lunch", "Dinner"});
    userAttendanceTable->horizontalHeader()->setStretchLastSection(true);
    userAttendanceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    userAttendanceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(userAttendanceTable);

    // Record attendance button
    recordAttendanceButton = new QPushButton("Record Attendance", this);
    mainLayout->addWidget(recordAttendanceButton);

    // Connections
    connect(attendanceDateEdit, &QDateEdit::dateChanged, this, &MealAttendancePage::loadAttendanceForDate);
    connect(recordAttendanceButton, &QPushButton::clicked, this, &MealAttendancePage::recordAttendanceClicked);

    // Initial load
    allUsers = getAllUsers(); // Fetch all users once
    loadAttendanceForDate();

    setLayout(mainLayout);
}

void MealAttendancePage::loadAttendanceForDate()
{
    userAttendanceTable->setRowCount(0); // Clear existing rows
    userAttendanceTable->setSortingEnabled(false); // Disable sorting during population

    QString selectedDate = attendanceDateEdit->date().toString("yyyy-MM-dd");
    std::vector<MealAttendance> existingAttendance = getAttendanceForDate(selectedDate.toStdString());

    // Use a set for efficient O(log N) lookups instead of linear search
    std::set<std::pair<int, std::string>> existingAttendanceSet;
    for (const auto& att : existingAttendance) {
        existingAttendanceSet.insert({att.user_id, att.meal_type});
    }

    userAttendanceTable->setRowCount(allUsers.size());

    for (size_t i = 0; i < allUsers.size(); ++i) {
        User user = allUsers[i];

        // Create a single item for the user's name and store their ID in it.
        // This fixes the bug where the name was being overwritten.
        auto *userNameItem = new QTableWidgetItem(QString::fromStdString(user.name));
        userNameItem->setData(Qt::UserRole, user.id);
        userNameItem->setFlags(userNameItem->flags() & ~Qt::ItemIsEditable); // Make non-editable
        userAttendanceTable->setItem(i, 0, userNameItem);

        // Create checkboxes for each meal type
        auto *breakfastCb = new QCheckBox();
        auto *lunchCb = new QCheckBox();
        auto *dinnerCb = new QCheckBox();

        // Use the set for fast lookups
        breakfastCb->setChecked(existingAttendanceSet.count({user.id, "Breakfast"}));
        lunchCb->setChecked(existingAttendanceSet.count({user.id, "Lunch"}));
        dinnerCb->setChecked(existingAttendanceSet.count({user.id, "Dinner"}));

        userAttendanceTable->setCellWidget(i, 1, breakfastCb);
        userAttendanceTable->setCellWidget(i, 2, lunchCb);
        userAttendanceTable->setCellWidget(i, 3, dinnerCb);
    }
    userAttendanceTable->setSortingEnabled(true);
}

void MealAttendancePage::recordAttendanceClicked()
{
    QString selectedDate = attendanceDateEdit->date().toString("yyyy-MM-dd");
    std::string dateStr = selectedDate.toStdString();

    std::vector<AttendanceRecord> recordsToAdd;
    std::vector<AttendanceRecord> recordsToDelete;

    // 1. Get the original state from the database
    std::set<std::pair<int, std::string>> dbAttendance;
    for (const auto& att : getAttendanceForDate(dateStr)) {
        dbAttendance.insert({att.user_id, att.meal_type});
    }

    for (int i = 0; i < userAttendanceTable->rowCount(); ++i) {
        int userId = userAttendanceTable->item(i, 0)->data(Qt::UserRole).toInt();
        auto* breakfastCb = qobject_cast<QCheckBox*>(userAttendanceTable->cellWidget(i, 1));
        auto* lunchCb = qobject_cast<QCheckBox*>(userAttendanceTable->cellWidget(i, 2));
        auto* dinnerCb = qobject_cast<QCheckBox*>(userAttendanceTable->cellWidget(i, 3));

        // Helper lambda to check for changes
        auto checkChanges = [&](QCheckBox* cb, const std::string& mealType) {
            if (!cb) return;
            bool isCheckedInUi = cb->isChecked();
            bool wasCheckedInDb = dbAttendance.count({userId, mealType});

            if (isCheckedInUi && !wasCheckedInDb) {
                recordsToAdd.push_back({userId, mealType});
            } else if (!isCheckedInUi && wasCheckedInDb) {
                recordsToDelete.push_back({userId, mealType});
            }
        };

        checkChanges(breakfastCb, "Breakfast");
        checkChanges(lunchCb, "Lunch");
        checkChanges(dinnerCb, "Dinner");
    }

    bool addSuccess = addMultipleAttendance(dateStr, recordsToAdd);
    bool deleteSuccess = deleteMultipleAttendance(dateStr, recordsToDelete);

    if (addSuccess && deleteSuccess) {
        if (!recordsToAdd.empty() || !recordsToDelete.empty()) {
            QMessageBox::information(this, "Success", "Attendance updated successfully.");
        } else {
            QMessageBox::information(this, "No Changes", "No changes were made to the attendance records.");
        }
    } else {
        QMessageBox::critical(this, "Error", "Failed to update one or more attendance records. Please check the logs.");
    }
    loadAttendanceForDate(); // Reload to show updated state
}