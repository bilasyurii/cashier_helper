#ifndef SHOP_H
#define SHOP_H
#include <vector>
#include <map>
#include "good.h"
#include "card.h"

class CShop {
private:
    std::vector<CGood> m_goods; // тут будуть зберігатись всі товари магазину
    std::vector<CGood> m_cart; // тут товари, які в кошику
    std::map<unsigned, CCard> m_cards; // тут будуть картки клієнтів
    unsigned m_uAvailable; // тут число, яке каже номер для нової картки
    float m_fSum = 0;

public:
    CShop();

    void clearCart();
    void clearGoods();
    void loadGoods();
    void loadCards();
    std::vector<CGood>& getGoods();
    std::vector<CGood>& getCart();
    std::map<unsigned, CCard>& getCards();
    unsigned goodsSize() const;
    float getCartCost() const;
    unsigned getNewCardID();
    void addToCash(float sum);
    float getCash() const;
    void saveGoods();
    void saveCards();
};

#endif // SHOP_H
