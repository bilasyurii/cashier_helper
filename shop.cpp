#include "shop.h"

#include <fstream>
#include <string>
#include "myexception.h"
#include <algorithm>
#include <QMessageBox>

CShop::CShop() {}

// Очищення кошика
void CShop::clearCart() {
    m_cart = std::vector<CGood>();
}

// Очищення бази товарів
void CShop::clearGoods() {
    m_goods = std::vector<CGood>();
}

// Завантаження товарів з файлу
void CShop::loadGoods() {
    std::ifstream fin;
    fin.open("goods.txt");
    if(fin.is_open()) {
        std::string str;
        CGood good;
        unsigned numbers[13];
        bool status;
        while(std::getline(fin, str)) { // зчитуємо один рядок з файлу
            if(str.length() == 0) // перевіряємо на пустоту
                continue; // якщо пустий - пробуємо заново, але з іншим рядком
            if(str.length() > 30)
                str.resize(30); // обрізаємо назву товару до 30 символів
            good.setName(str);
            if(!std::getline(fin, str))
                continue;
            if(str.length() != 13) // перевірка довжини штрих-коду
                continue;
            // перевіряємо штрих-код на наявність чого-небудь, крім цифр
            status = false;
            for(int i = 0; i < 13; ++i)
                if(str[i] < '0' || str[i] > '9') {
                    status = true;
                    break;
                } else
                    numbers[i] = str[i] - '0';
            if(status)
                continue;
            good.setBarcode(CBarcode(numbers));
            if(!good.getBarcode()) // перевірка правильності штрих-коду
                continue;
            if(!std::getline(fin, str)) // зчитуємо ціну
                continue;
            if(str.length() == 0)
                continue;
            good.setPrice(QString::fromStdString(str).toFloat(&status));
            if(!status)
                continue;
            good.setAmount(1.0f);
            m_goods.push_back(good);
        }
        fin.close();
    } else // якщо файл не відкрився
        QMessageBox::warning(nullptr, "Увага", "Файл з товарами (goods.txt) не знайдений або "
                                               "відбулась помилка при його відкритті.");
}

std::vector<CGood> &CShop::getGoods() {
    return m_goods;
}

std::vector<CGood> &CShop::getCart() {
    return m_cart;
}

std::map<unsigned, CCard>& CShop::getCards() {
    return m_cards;
}

unsigned CShop::goodsSize() const {
    return m_goods.size();
}

// Підрахунок вартості кошика
float CShop::getCartCost() const{
    float cost = 0;
    for(unsigned i = 0; i < m_cart.size(); ++i)
        cost += m_cart[i].getCost();
    return cost;
}

// Функція, що повертає вільний номер картки для видачі клієнтові
unsigned CShop::getNewCardID() {
    // в m_uAvailable зберігається останній вільний номер;
    // спробуємо його пошукати в базі карток клієнтів
    std::map<unsigned, CCard>::iterator it = m_cards.find(m_uAvailable);
    // якщо його там не знайдено, то повертаємо його
    if(it == m_cards.end())
        return m_uAvailable;
    // інакше шукаємо вільний ID, якого нема в базі
    ++it;
    // в m_uAvailable зберігаємо попередній номер, а в it зберігаємо наступний знайдений
    while(it != m_cards.end() && it->first - m_uAvailable == 1) {
        ++m_uAvailable;
        ++it;
    }
    // якщо розрив між знайденим і m_uAvailable буде
    // більше 1, то картка m_uAvailable + 1 вільна
    ++m_uAvailable;
    // але якщо m_uAvailable переварив за допустиму межу, то видача картки заборонена
    if(m_uAvailable >= 100000000)
        throw CMyException("Неможливо створити нову картку, бо їх кількість вичерпана.");
    return m_uAvailable;
}

// Збільшення суми, заробленої за сесію
void CShop::addToCash(float sum) {
    m_fSum += sum;
}

float CShop::getCash() const {
    return m_fSum;
}

// Збереження бази товарів в файл
void CShop::saveGoods() {
    std::sort(m_goods.begin(), m_goods.end());
    std::ofstream fout;
    fout.open("goods.txt");
    for(int i = 0; i < m_goods.size(); ++i) {
        fout << m_goods[i].getName() << "\n" <<
                ((QString)m_goods[i].getBarcode()).toStdString() << "\n" <<
                m_goods[i].getPrice() << "\n";
    }
    fout.close();
}

// Збереження бази карток клієнтів в файл
void CShop::saveCards() {
    std::ofstream fout;
    fout.open("customer.txt");
    for(std::map<unsigned, CCard>::const_iterator it = m_cards.begin(); it != m_cards.end(); ++it) {
        fout << QString::number(it->second.getId()).rightJustified(8, '0').toStdString() << "\n" <<
                it->second.getSum() << "\n" <<
                it->second.getBonus() << "\n";
    }
    fout.close();
}

// Завантаження бази карток клієнтів з файлу
void CShop::loadCards() {
    // очищаємо базу про всяк випадок
    m_cards = std::map<unsigned, CCard>();
    std::ifstream fin;
    m_uAvailable = 0;
    fin.open("customer.txt");
    if(fin.is_open()) {
        std::string str;
        CCard card;
        unsigned id;
        bool status;
        while(std::getline(fin, str)) { // зчитуємо один рядок файлу
            if(str.length() != 8) // номер картки має містити 8 символів
                continue; // читаємо наступний рядок, якщо не містить
            id = 0;
            // перевірка на вміст в номері сторонніх
            // символів (будь-яких, крім цифр)
            status = false;
            for(int i = 0; i < 8; ++i)
                if(str[i] < '0' || str[i] > '9') {
                    status = true;
                    break;
                } else // та водночас переведення цифр в число
                    id += ((unsigned)(pow(10, 7 - i))) * (str[i] - '0');
            if(status)
                continue;
            card.setId(id);
            if(!std::getline(fin, str)) // зчитуємо суму, яку витратив клієнт
                continue;
            card.setSum(QString::fromStdString(str).toFloat(&status));
            if(!status)
                continue;
            if(!std::getline(fin, str)) // зчитуємо накопичені бонуси
                continue;
            card.setBonus(QString::fromStdString(str).toFloat(&status));
            if(!status)
                continue;
            m_cards[id] = card;
        }
        fin.close();
    } else // якщо файл не відкрився
        QMessageBox::warning(nullptr, "Увага", "Файл з товарами (customer.txt) не знайдений або "
                                               "відбулась помилка при його відкритті.");
}
