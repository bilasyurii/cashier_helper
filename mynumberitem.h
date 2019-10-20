#ifndef MYNUMBERITEM_H
#define MYNUMBERITEM_H

#include <QObject>
#include <QTableWidget>
#include <QString>

/*
Цей клас є замінником QTableWidgetItem. Я
використовую його, коли потрібно вставити
в табличку числове значення. Якщо вставляти
звичайні QTableWidgetItem, то вони
сприйматимуться, як стрічки і сортування
буде відбуватись некоректно.
*/

class CMyNumberItem : public QTableWidgetItem {
public:
    CMyNumberItem();
    CMyNumberItem(QString str);

    bool operator<(const QTableWidgetItem &other) const;
};

#endif // MYNUMBERITEM_H
