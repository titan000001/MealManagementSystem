#ifndef MENUHISTORYPAGE_H
#define MENUHISTORYPAGE_H

#include <QWidget>

class QTableWidget;

class MenuHistoryPage : public QWidget
{
    Q_OBJECT

public:
    explicit MenuHistoryPage(QWidget *parent = nullptr);

private:
    void loadMenuHistory();

    QTableWidget *historyTable;
};

#endif // MENUHISTORYPAGE_H