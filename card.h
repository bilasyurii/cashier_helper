#ifndef CARD_H
#define CARD_H

#include <map>

class CCard
{
private:
    unsigned m_uId;
    float m_fSum;
    float m_fBonus;
public:
    CCard();
    CCard(unsigned id);
    CCard(unsigned id, float sum, float bonus);

    float getDiscount() const;
    unsigned getId() const;
    float getSum() const;
    float getBonus() const;
    void setId(const unsigned &uId);
    void setSum(float fSum);
    void setBonus(float fBonus);
};

#endif // CARD_H
