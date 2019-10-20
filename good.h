#ifndef GOOD_H
#define GOOD_H

#include "barcode.h"
#include <string>

class CGood {
private:
    CBarcode m_barcode;
    std::string m_strName;
    float m_fPrice;
    float m_fAmount;
public:
    CGood();
    CGood(const CBarcode &barcode, const std::string &strName, const float fPrice, const float &fAmount = 1.0f);

    void setBarcode(const CBarcode &barcode);
    void setName(const std::string &strName);
    void setPrice(const float fPrice);
    void setAmount(const float fAmount);
    CBarcode getBarcode() const;
    std::string getName() const;
    float getPrice() const;
    float getAmount() const;
    float getCost() const;
    bool operator<(const CGood &other) const;
};

#endif // GOOD_H
