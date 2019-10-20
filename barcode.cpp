#include "barcode.h"
#include "myexception.h"

// Стандартний конструктор
CBarcode::CBarcode() {
    for(int i = 0; i < 13; ++i)
        numbers[i] = 0;
}

// Конструктор, що створює штрих-код,
// використовуючи масив об'єктів класу CCodeNumber
CBarcode::CBarcode(CCodeNumber arr[13]) {
    try {
        for(int i = 0; i < 13; ++i)
            numbers[i] = arr[i];
    } catch(CMyException e) {
        throw e;
    }
}

// Конструктор, що створює штрих-код,
// використовуючи масив беззнакових цілих цифр
CBarcode::CBarcode(unsigned arr[13]) {
    for(int i = 0; i < 13; ++i)
        numbers[i] = arr[i];
}

// Конструктор, що створює штрих-код,
// використовуючи стрічку QString. Також
// конструктор перевіряє чи стрічка
// містить недозволені символи
CBarcode::CBarcode(QString str) {
    for(int i = 0; i < 13; ++i) {
        numbers[i] = str[i].toLatin1() - '0';
        if(numbers[i] < 0 || numbers[i] > 9)
            throw CMyException("Штрих-код містить заборонені символи.");
    }
}

// Оператор для отримання певної цифри з штрих-коду
unsigned& CBarcode::operator[](int i) {
    return numbers[i];
}

// Оператор для порівняння двох штрих-кодів на рівність
bool CBarcode::operator==(const CBarcode &other) const {
    for(int i = 0; i < 13; ++i)
        if(numbers[i] != other.numbers[i])
            return false;
    return true;
}

// Оператор для порівняння двох штрих-кодів
// (використовується в сортуванні)
bool CBarcode::operator<(const CBarcode &other) const {
    for(int i = 0; i < 13; ++i)
        if(numbers[i] < other.numbers[i])
            return true;
        else if(numbers[i] > other.numbers[i])
            return false;
    return false;
}

// Перевірка правильності штрих-коду
CBarcode::operator bool() const {
    // для цього вираховуємо та перевіряємо контрольну цифру
    unsigned uControlNumber;
    uControlNumber = 0;
    for(int i = 0; i < 6; ++i)
        uControlNumber += numbers[i * 2 + 1];
    uControlNumber *= 3;
    for(int i = 0; i < 6; ++i)
        uControlNumber += numbers[i * 2];
    uControlNumber = uControlNumber % 10;
    return 10 - uControlNumber == numbers[12];
}

// Перетворення штрих-коду на стрічку
CBarcode::operator QString() const {
    QString str;
    for(int i = 0; i < 13; ++i)
        str += QString::number(numbers[i]);
    return str;
}
