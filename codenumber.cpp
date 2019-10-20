#include "codenumber.h"
#include "myexception.h"

CCodeNumber::CCodeNumber() : m_type(WRONG), m_uNum(0) {}

CCodeNumber::CCodeNumber(unsigned n, TYPE type) : m_type(type), m_uNum(n) {}

// Конструктор, що створює цифру з масиву бітів
CCodeNumber::CCodeNumber(bool *bits) : m_type(WRONG), m_uNum(0) {
    // таблиця для розпізнавання цифри
    static const unsigned auNumbers[10][3] = {
        {13, 114, 39},
        {25, 102, 51},
        {19, 108, 27},
        {61, 66,  33},
        {35, 92,  29},
        {49, 78,  57},
        {47, 80,  5 },
        {59, 68,  17},
        {55, 72,  9 },
        {11, 116, 23}
    };

    // перетворюємо число з двійкової системи в десяткову
    static unsigned n, p;
    n = 0;
    for(int i = 0; i < 7; ++i) {
        if(bits[i]) {
            p = 1;
            for(int j = 0; j < 6 - i; ++j)
                p *= 2;
            n += p;
        }
    }

    // робимо пошук по масиву auNumbers та
    // дізнаємось тип та значення коду
    for(int i = 0; i < 10; ++i)
        for(int j = 0; j < 3; ++j) {
            if(auNumbers[i][j] == n) {
                m_type = (TYPE) j;
                m_uNum = i;
                return;
            }
        }
    // до цього рядка ми дійдемо лише, якщо
    // штрих-код буде неправильним і задана
    // комбінація бітів не відповідає жодному
    // числу з таблиці auNumbers
    throw CMyException("Не вдається розкодувати біти в штрих-коді.");
}

// Перетворення об'єкту CCodeNumber в беззнакове ціле число
CCodeNumber::operator unsigned() const {
    if(m_type == WRONG)
        throw CMyException("Не вдається розкодувати біти в штрих-коді.");
    return m_uNum;
}

CCodeNumber::TYPE CCodeNumber::getType() const {
    return m_type;
}
