#ifndef DAILYMENUPAGE_H
#define DAILYMENUPAGE_H

#include <QWidget>
#include <vector>
#include "menu.h"

class QDateEdit;
class QListWidget;
class QPushButton;

class DailyMenuPage : public QWidget
{
    Q_OBJECT

public:
    explicit DailyMenuPage(QWidget *parent = nullptr);

private slots:
    void loadDailyMenu();
    void saveDailyMenuClicked();
    void addBreakfastItem();
    void removeBreakfastItem();
    void addLunchItem();
    void removeLunchItem();
    void addDinnerItem();
    void removeDinnerItem();

private:
    void loadAvailableMenuItems();
    std::vector<int> getMenuItemIds(QListWidget* listWidget);

    QDateEdit *menuDateEdit;
    QListWidget *availableMenuItemsList;
    QListWidget *breakfastList;
    QListWidget *lunchList;
    QListWidget *dinnerList;

    QPushButton *saveMenuButton;
};

#endif // DAILYMENUPAGE_H