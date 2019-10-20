#ifndef BARCODE_H
#define BARCODE_H

#include "codenumber.h"
#include <QString>

class CBarcode {
private:
    unsigned numbers[13]; // цифри штрих-коду
public:
    CBarcode();
    CBarcode(CCodeNumber arr[13]);
    CBarcode(unsigned arr[13]);
    CBarcode(QString str);

    unsigned& operator[](int i);
    bool operator==(const CBarcode &other) const;
    bool operator<(const CBarcode &other) const;
    operator QString() const;
    operator bool() const;
};

#endif // BARCODE_H
