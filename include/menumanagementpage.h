#ifndef MENUMANAGEMENTPAGE_H
#define MENUMANAGEMENTPAGE_H

#include <QWidget>

class QTableWidget;
class QLineEdit;
class QPushButton;

class MenuManagementPage : public QWidget
{
    Q_OBJECT

public:
    explicit MenuManagementPage(QWidget *parent = nullptr);

private slots:
    void addMenuItemClicked();
    void editMenuItemClicked();
    void deleteMenuItemClicked();
    void refreshMenuItems();

private:
    void loadMenuItems();

    QTableWidget *menuTable;
    QLineEdit *menuItemNameLineEdit;
    QPushButton *addMenuItemButton;
    QPushButton *editMenuItemButton;
    QPushButton *deleteMenuItemButton;
};

#endif // MENUMANAGEMENTPAGE_H