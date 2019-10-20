#include "mynumberitem.h"

CMyNumberItem::CMyNumberItem() : QTableWidgetItem() {}

CMyNumberItem::CMyNumberItem(QString str) : QTableWidgetItem(str) {}

bool CMyNumberItem::operator<(const QTableWidgetItem &other) const {
    return text().toFloat() < other.text().toFloat();
}
