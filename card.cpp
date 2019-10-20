#include "card.h"

unsigned CCard::getId() const {
    return m_uId;
}

void CCard::setId(const unsigned &uId) {
    m_uId = uId;
}

float CCard::getSum() const {
    return m_fSum;
}

void CCard::setSum(float fSum) {
    m_fSum = fSum;
}

float CCard::getBonus() const {
    return m_fBonus;
}

void CCard::setBonus(float fBonus) {
    m_fBonus = fBonus;
}

CCard::CCard() : m_fSum(0.0f), m_fBonus(0.0f) {}

CCard::CCard(unsigned id) : m_uId(id), m_fSum(0.0f), m_fBonus(0.0f) {}

CCard::CCard(unsigned id, float sum, float bonus) : m_uId(id), m_fSum(sum), m_fBonus(bonus) {}

// Ця функція повертає знижку (відсотки), яка залежить
// від суми, яку клієнт витратив за весь час користування
// карткою. Мінімальний відсоток - 1, максимальний - 3
// (доступний, якщо сума витрат >= 1000 у.о.
float CCard::getDiscount() const {
    return ((m_fSum < 1000.0f)?(1.0f + m_fSum / 1000.0f * 2.0f):(3.0f));
}
