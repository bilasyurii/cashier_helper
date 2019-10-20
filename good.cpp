#include "good.h"

CGood::CGood() : m_strName(""), m_fPrice(0), m_fAmount(0) {}

CGood::CGood(const CBarcode &barcode, const std::string &strName, const float fPrice, const float &fAmount) :
    m_barcode(barcode), m_strName(strName), m_fPrice(fPrice), m_fAmount(fAmount) {}

void CGood::setBarcode(const CBarcode &barcode) {
    m_barcode = barcode;
}

void CGood::setName(const std::string &strName) {
    m_strName = strName;
}

void CGood::setPrice(const float fPrice) {
    m_fPrice = fPrice;
}

void CGood::setAmount(const float fAmount) {
    m_fAmount = fAmount;
}

CBarcode CGood::getBarcode() const {
    return m_barcode;
}

std::string CGood::getName() const {
    return m_strName;
}

float CGood::getPrice() const {
    return m_fPrice;
}

float CGood::getAmount() const {
    return m_fAmount;
}

float CGood::getCost() const {
    return m_fPrice * m_fAmount;
}

bool CGood::operator<(const CGood &other) const {
    return m_barcode < other.m_barcode;
}
