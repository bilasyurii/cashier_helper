#include "showdatabasedialog.h"
#include "ui_showdatabasedialog.h"
#include "mynumberitem.h"
#include "myexception.h"
#include <QMessageBox>

CShowDatabaseDialog::CShowDatabaseDialog(CShop &shop, QWidget *parent) :
    QDialog(parent), ui(new Ui::CShowDatabaseDialog), m_shop(shop) {
    ui->setupUi(this);
    ui->dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setFixedSize(size());
    fillTable();
    ui->dataTable->setSortingEnabled(true);
    ui->addGoodBlock->setVisible(false);
    ui->applyGood->setVisible(false);
    ui->addGoodCode->setFixedWidth(ui->addGoodCode->width());
    // створення "гарячих клавіш"
    ui->newGood->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    ui->editGood->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    ui->deleteGood->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
}

CShowDatabaseDialog::~CShowDatabaseDialog() {
    delete ui;
}

// Заповнення таблиця даними про товари
void CShowDatabaseDialog::fillTable() {
    ui->dataTable->setRowCount((int)m_shop.goodsSize());
    for(unsigned i = 0; i < m_shop.goodsSize(); ++i) {
        ui->dataTable->setItem(i, 0,
                               new QTableWidgetItem(QString::fromLocal8Bit(m_shop.getGoods()[i].getName().c_str())));
        ui->dataTable->setItem(i, 1,
                               new CMyNumberItem(m_shop.getGoods()[i].getBarcode()));
        ui->dataTable->setItem(i, 2,
                               new CMyNumberItem(QString::number(m_shop.getGoods()[i].getPrice(), 'f', 2)));
    }
}

// Якщо користувач змінив поле пошуку за іменем
void CShowDatabaseDialog::on_byNameEdit_textChanged(const QString &arg1) {
    ui->byBarcodeEdit->setReadOnly(true);
    if(arg1.length() == 0) { // якщо воно пусте
        ui->byBarcodeEdit->setReadOnly(false);
        fillTable(); // виводимо всі товари
    } else { // інакше виводимо лише ті, що містять символи для пошуку
        ui->dataTable->setRowCount(0);
        for(unsigned i = 0; i < m_shop.getGoods().size(); ++i) {
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

// Аналогічно до функції пошуку за іменем, але цього разу за штрих-кодом
void CShowDatabaseDialog::on_byBarcodeEdit_textChanged(const QString &arg1) {
    ui->byNameEdit->setReadOnly(true);
    if(arg1.length() == 0) {
        ui->byNameEdit->setReadOnly(false);
        fillTable();
    } else {
        ui->dataTable->setRowCount(0);
        for(unsigned i = 0; i < m_shop.getGoods().size(); ++i) {
            // послідовна перевірка на рівність з символами з шуканого штрих-коду
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

// Скасування пошуку за іменем
void CShowDatabaseDialog::on_clearName_clicked() {
    ui->byBarcodeEdit->setReadOnly(false);
    ui->byNameEdit->clear();
    fillTable();
}

// Скасування пошуку за штрих-кодом
void CShowDatabaseDialog::on_clearBarcode_clicked() {
    ui->byNameEdit->setReadOnly(false);
    ui->byBarcodeEdit->clear();
    fillTable();
}

// Видалення товару
void CShowDatabaseDialog::on_deleteGood_clicked() {
    if(!ui->dataTable->selectionModel()->hasSelection()) // якщо нічого не виділено, то вихід
        return;
    QMessageBox sureQuestion(QMessageBox::Question, "Підтвердження",
                             "Ви впевнені, що хочете видалити цей товар?",
                             QMessageBox::Yes | QMessageBox::No, this);
    sureQuestion.setButtonText(QMessageBox::Yes, tr("Так"));
    sureQuestion.setButtonText(QMessageBox::No, tr("Ні"));
    if(sureQuestion.exec() == QMessageBox::Yes) { // якщо користувач впевнений, що хоче видалити товар
        for(unsigned i = 0; i < m_shop.goodsSize(); ++i) // шукаємо його в базі товарів за штрих-кодом та видаляємо
            if(m_shop.getGoods()[i].getBarcode() == CBarcode(ui->dataTable->item(ui->dataTable->currentRow(), 1)->text())) {
                m_shop.getGoods().erase(m_shop.getGoods().begin() + i);
                ui->dataTable->removeRow(ui->dataTable->currentRow());
                break;
            }
    }
}

// Початок створення нового товару
void CShowDatabaseDialog::on_newGood_clicked() {
    ui->addGoodBlock->setVisible(true);
    ui->newGood->setEnabled(false);
    ui->deleteGood->setEnabled(false);
    ui->editGood->setEnabled(false);
}

// Відміна створення нового товару
void CShowDatabaseDialog::on_cancelGoodButton_clicked() {
    ui->addGoodName->clear();
    ui->addGoodCode->clear();
    ui->addGoodPrice->clear();
    ui->addGoodBlock->setVisible(false);
    ui->newGood->setEnabled(true);
    ui->deleteGood->setEnabled(true);
    ui->editGood->setEnabled(true);
}

// Підтвердження створення нового товару
void CShowDatabaseDialog::on_addGoodButton_clicked() {
    float price;
    CBarcode barcode;
    // перевіряємо чи всі поля заповнено правильно
    if(checkName(ui->addGoodName->text()) &&
       checkBarcode(barcode, ui->addGoodCode->text()) &&
       checkPrice(price, ui->addGoodPrice->text())) {
        for(int i = 0; i < m_shop.getGoods().size(); ++i) // дивимось чи є товари з ідентичним штрих-кодом
            if(m_shop.getGoods()[i].getBarcode() == barcode) {
                QMessageBox::critical(this, "Помилка", "В базі не може бути товарів з однаковими штрих-кодами.");
                return;
            }
        m_shop.getGoods().push_back(CGood(barcode, ui->addGoodName->text().toLocal8Bit().constData(), price));
        int index = ui->dataTable->rowCount();
        // перед тим, як додавати товар в список, потрібно відімкнути
        // сортування таблиці (бо він відразу "пригне" десь)
        ui->dataTable->setSortingEnabled(false);
        ui->dataTable->insertRow(index);
        ui->dataTable->setItem(index, 0,
                               new QTableWidgetItem(ui->addGoodName->text()));
        ui->dataTable->setItem(index, 1,
                               new CMyNumberItem(barcode));
        ui->dataTable->setItem(index, 2,
                               new CMyNumberItem(QString::number(price, 'f', 2)));
        ui->dataTable->setSortingEnabled(true); // вмикаємо сорт.
        ui->addGoodBlock->setVisible(false);
        ui->newGood->setEnabled(true);
        ui->deleteGood->setEnabled(true);
        ui->editGood->setEnabled(true);
        ui->addGoodName->clear();
        ui->addGoodCode->clear();
        ui->addGoodPrice->clear();
    }
}

// Перевірка правильності назви товару
bool CShowDatabaseDialog::checkName(QString source) {
    if(source.length() == 0 || source.length() > 30) {
        QMessageBox::critical(this, "Помилка", "Назва має мати довжину не менше одного та не більше 30 символів.");
        return false;
    }
    return true;
}

// Первірка правильності штрих-коду
bool CShowDatabaseDialog::checkBarcode(CBarcode &dest, QString source) {
    if(source.length() != 13) {
        QMessageBox::critical(this, "Помилка", "Штрих-код має містити 13 цифр.");
        return false;
    }
    try {
        if(!(dest = CBarcode(source))) {
            QMessageBox::critical(this, "Помилка", "Введений штрихкод не правильний.");
            return false;
        }
    } catch(CMyException e) {
        QMessageBox::critical(this, "Помилка", e.what());
        return false;
    }
    return true;
}

// Перевірка правильності ціни товару
bool CShowDatabaseDialog::checkPrice(float &dest, QString source) {
    bool result;
    dest = source.toFloat(&result);
    if(!result) {
        QMessageBox::critical(this, "Помилка", "Помилка під час зчитування ціни.");
        return false;
    }
    if(dest <= 0) {
        QMessageBox::critical(this, "Помилка", "Ціна має бути додатнім числом.");
        return false;
    }
    return true;
}

// Функція для початку редагування товару
void CShowDatabaseDialog::on_editGood_clicked() {
    if(!ui->dataTable->selectionModel()->hasSelection()) // якщо нічого не вибрано, то вихід
        return;
    int iEditIdx = ui->dataTable->currentRow();
    ui->dataTable->setSortingEnabled(false); // вимикаємо сортування таблиці, поки відбувається редагування
    // дозволяємо редагувати таблицю
    ui->dataTable->setEditTriggers(QTableView::DoubleClicked | QTableView::EditKeyPressed | QTableView::AnyKeyPressed);
    ui->newGood->setEnabled(false);
    ui->deleteGood->setEnabled(false);
    ui->applyGood->setVisible(true);
    ui->editGood->setVisible(false);
    ui->byNameEdit->setEnabled(false);
    ui->byBarcodeEdit->setEnabled(false);
    ui->clearName->setEnabled(false);
    ui->clearBarcode->setEnabled(false);
    // проходимо по рядках
    for(int i = 0; i < ui->dataTable->rowCount(); ++i)
        for(int j = 0; j < 3; ++j)
            if(i != iEditIdx) // всім забороняємо інтеракцію користувача
                ui->dataTable->item(i, j)->setFlags(ui->dataTable->item(i, j)->flags() & ~Qt::ItemIsEnabled);
            else // а тому, який редагується, дозволяємо
                ui->dataTable->item(i, j)->setFlags(ui->dataTable->item(i, j)->flags() | Qt::ItemIsEnabled);
    for(unsigned i = 0; i < m_shop.getGoods().size(); ++i) // шукаємо в базі товар, який редагуємо
        if((QString) m_shop.getGoods()[i].getBarcode() == ui->dataTable->item(iEditIdx, 1)->text()) {
            targetIndex = i; // і зберігаємо його індекс
            break;
        }
}

// Підтвердження редагування товару
void CShowDatabaseDialog::on_applyGood_clicked() {
    float price;
    CBarcode barcode;
    // перевіряємо на правильність введені дані
    if(checkName(ui->dataTable->item(ui->dataTable->currentRow(), 0)->text()) &&
       checkBarcode(barcode, ui->dataTable->item(ui->dataTable->currentRow(), 1)->text()) &&
       checkPrice(price, ui->dataTable->item(ui->dataTable->currentRow(), 2)->text())) {
        for(int i = 0; i < m_shop.getGoods().size(); ++i) // перевірка чи є товари з ідентичним штрих-кодом
            if(i != targetIndex && m_shop.getGoods()[i].getBarcode() == barcode) {
                QMessageBox::critical(this, "Помилка", "В базі не може бути товарів з однаковими штрих-кодами.");
                return;
            }
        // зберігаємо зміни в базі товарів
        m_shop.getGoods()[targetIndex].setName(
                    ui->dataTable->item(ui->dataTable->currentRow(), 0)->text().toLocal8Bit().constData());
        m_shop.getGoods()[targetIndex].setBarcode(barcode);
        m_shop.getGoods()[targetIndex].setPrice(price);
        ui->applyGood->setVisible(false);
        ui->editGood->setVisible(true);
        ui->newGood->setEnabled(true);
        ui->deleteGood->setEnabled(true);
        ui->byNameEdit->setEnabled(true);
        ui->byBarcodeEdit->setEnabled(true);
        ui->clearName->setEnabled(true);
        ui->clearBarcode->setEnabled(true);
        // забороняємо редагування таблиці
        ui->dataTable->setEditTriggers(QTableView::NoEditTriggers);
        ui->dataTable->setSortingEnabled(true);
        // дозволяємо інтеракцію (наприклад виділення) з всіма рядками таблиці
        for(int i = 0; i < ui->dataTable->rowCount(); ++i)
            if(!ui->dataTable->item(i, 0)->flags().testFlag(Qt::ItemIsEnabled))
                for(int j = 0; j < 3; ++j)
                        ui->dataTable->item(i, j)->setFlags(ui->dataTable->item(i, j)->flags() | Qt::ItemIsEnabled);
    }
}
