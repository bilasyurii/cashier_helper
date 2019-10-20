#include "findgooddialog.h"
#include "ui_findgooddialog.h"
#include "mynumberitem.h"

CFindGoodDialog::CFindGoodDialog(CShop &shop, CBarcode &_barcode, QWidget *parent) :
    QDialog(parent), ui(new Ui::CFindGoodDialog), m_shop(shop), barcode(_barcode) {
    ui->setupUi(this);
    ui->dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setFixedWidth(width());
    fillTable();
    ui->dataTable->setSortingEnabled(true);
}

CFindGoodDialog::~CFindGoodDialog() {
    delete ui;
}

void CFindGoodDialog::on_cancelButton_clicked() {
    reject();
}

// Функція-клікер для кнопки "Ок", що додає знайдений товар в кошик.
void CFindGoodDialog::on_okButton_clicked() {
    // якщо не обрано жодного товару, то вихід
    if(!ui->dataTable->selectionModel()->hasSelection())
        return;
    // інакше - запам'ятовуємо штрих-код товару і виходимо
    barcode = CBarcode(ui->dataTable->item(ui->dataTable->currentRow(), 1)->text());
    accept();
}

// Функція для заповнення таблиці товарами з бази
void CFindGoodDialog::fillTable() {
    ui->dataTable->setRowCount((int)m_shop.goodsSize());
    // виводжу всі дані товарів на таблицю
    for(unsigned i = 0; i < m_shop.goodsSize(); ++i) {
        ui->dataTable->setItem(i, 0,
                               new QTableWidgetItem(QString::fromLocal8Bit(m_shop.getGoods()[i].getName().c_str())));
        ui->dataTable->setItem(i, 1,
                               new CMyNumberItem(m_shop.getGoods()[i].getBarcode()));
        ui->dataTable->setItem(i, 2,
                               new CMyNumberItem(QString::number(m_shop.getGoods()[i].getPrice(), 'f', 2)));
    }
}

// Очищення рядка пошуку за іменем
void CFindGoodDialog::on_clearName_clicked() {
    ui->byBarcodeEdit->setReadOnly(false);
    ui->byNameEdit->clear();
    fillTable();
}

// Очищення рядка пошуку за штрих-кодом
void CFindGoodDialog::on_clearBarcode_clicked() {
    ui->byNameEdit->setReadOnly(false);
    ui->byBarcodeEdit->clear();
    fillTable();
}

// Користувач щось написав в полі пошуку за іменем
void CFindGoodDialog::on_byNameEdit_textChanged(const QString &arg1) {
    ui->byBarcodeEdit->setReadOnly(true);
    // якщо рядок пошуку пустий, то виводимо всі товари
    if(arg1.length() == 0) {
        ui->byBarcodeEdit->setReadOnly(false);
        fillTable();
    } else {
        ui->dataTable->setRowCount(0);
        for(unsigned i = 0; i < m_shop.getGoods().size(); ++i) {
            // інакше - виводимо лише ті, що містять символи з поля пошуку
            if(QString::fromLocal8Bit(m_shop.getGoods()[i].getName().c_str()).contains(arg1, Qt::CaseInsensitive)) {
                ui->dataTable->insertRow(ui->dataTable->rowCount());
                ui->dataTable->setItem(ui->dataTable->rowCount() - 1, 0,
                                       new QTableWidgetItem(QString::fromLocal8Bit(m_shop.getGoods()[i].getName().c_str())));
                ui->dataTable->setItem(ui->dataTable->rowCount() - 1, 1,
                                       new CMyNumberItem(m_shop.getGoods()[i].getBarcode()));
                ui->dataTable->setItem(ui->dataTable->rowCount() - 1, 2,
                                       new CMyNumberItem(QString::number(m_shop.getGoods()[i].getPrice(), 'f', 2)));
            }
        }
    }
}

// Ця ф-ція аналогічна до пошуку за іменем, але тут шукаємо за штрих-кодом
void CFindGoodDialog::on_byBarcodeEdit_textChanged(const QString &arg1) {
    ui->byNameEdit->setReadOnly(true);
    if(arg1.length() == 0) {
        ui->byNameEdit->setReadOnly(false);
        fillTable();
    } else {
        ui->dataTable->setRowCount(0);
        for(unsigned i = 0; i < m_shop.getGoods().size(); ++i) {
            // послідовна перевірка символів штрих-коду на рівність до символів пошуку
            bool appropriate = true;
            for(int j = 0; j < arg1.size(); ++j) {
                if(arg1[j].toLatin1() - '0' != m_shop.getGoods()[i].getBarcode()[j]) {
                    appropriate = false;
                    break;
                }
            }
            if(appropriate) {
                ui->dataTable->insertRow(ui->dataTable->rowCount());
                ui->dataTable->setItem(ui->dataTable->rowCount() - 1, 0,
                                       new QTableWidgetItem(QString::fromLocal8Bit(m_shop.getGoods()[i].getName().c_str())));
                ui->dataTable->setItem(ui->dataTable->rowCount() - 1, 1,
                                       new CMyNumberItem(m_shop.getGoods()[i].getBarcode()));
                ui->dataTable->setItem(ui->dataTable->rowCount() - 1, 2,
                                       new CMyNumberItem(QString::number(m_shop.getGoods()[i].getPrice(), 'f', 2)));
            }
        }
    }
}
