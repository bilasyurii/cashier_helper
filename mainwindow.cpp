#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSignalMapper>
#include "readbarcodedialog.h"
#include "showdatabasedialog.h"
#include "findgooddialog.h"
#include "paybilldialog.h"
#include "mynumberitem.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->CartTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_shop.loadGoods();
    m_shop.loadCards();
    // створення "гарячих клавіш"
    ui->ReadBarcode->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    ui->FindAddGoods->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    ui->ShowGoods_2->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    ui->OkButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    // задавання іконок
    setWindowIcon(QIcon(":/ico_logo.png"));
    ui->ReadBarcode->setIcon(QIcon(":/ico_read.png"));
    ui->FindAddGoods->setIcon(QIcon(":/ico_find.png"));
    ui->ShowGoods_2->setIcon(QIcon(":/ico_show.png"));
    ui->OkButton->setIcon(QIcon(":/ico_pay.png"));
    ui->ReadBarcode->setIconSize(QSize(32, 32));
    ui->FindAddGoods->setIconSize(QSize(32, 32));
    ui->ShowGoods_2->setIconSize(QSize(32, 32));
    ui->OkButton->setIconSize(QSize(32, 32));
}
MainWindow::~MainWindow() {
    delete ui;
}

// Функція, що запускає діалогове вікно для зчитування штрих-кодів
void MainWindow::on_ReadBarcode_clicked() {
    CBarcode barcode;
    CReadBarcodeDialog readBarcodeDialog(barcode, this);
    if(readBarcodeDialog.exec() == QDialog::Accepted)
        addToCart(barcode);
}

// Функція, що запускає діалогове вікно з списком товарів
void MainWindow::on_ShowGoods_2_clicked() {
    CShowDatabaseDialog showBaseDialog(m_shop, this);
    showBaseDialog.exec();
}

// Функція, що запускає діалогове вікно для пошуку товарів
void MainWindow::on_FindAddGoods_clicked() {
    CBarcode barcode;
    CFindGoodDialog findGoodDialog(m_shop, barcode, this);
    if(findGoodDialog.exec() == QDialog::Accepted)
        addToCart(barcode);
}

// Функція, що викликається при видаленні товару з кошика
void MainWindow::DeleteGood_clicked() {
    // отримуємо індекс (в кошику) адресанта сигналу
    int delIdx = sender()->property("deleteGoodID").toInt();
    // видаляємо кнопку для видалення, що зберігається в табличці
    delete ui->CartTable->cellWidget(delIdx, 5);
    ui->CartTable->removeRow(delIdx);
    m_shop.getCart().erase(m_shop.getCart().begin() + delIdx);
    // переназначуємо індекси для решти кнопок видалення
    for(int i = delIdx; i < ui->CartTable->rowCount(); ++i)
        ui->CartTable->cellWidget(i, 5)->setProperty("deleteGoodID", i);
    // перераховуємо вартість кошика
    ui->CartCost->setText(QString::number(m_shop.getCartCost(), 'f', 2));
    if(ui->CartTable->rowCount() == 0)
        ui->OkButton->setEnabled(false);
}

// Додавання товару в кошик (за штрих-кодом)
void MainWindow::addToCart(CBarcode &barcode) {
    unsigned i;
    // пошук в базі
    for(i = 0; i < m_shop.getGoods().size(); ++i)
        if(m_shop.getGoods()[i].getBarcode() == barcode) { // знайдено
            unsigned j;
            // шукаємо чи такий товар вже є в кошику
            for(j = 0; j < m_shop.getCart().size(); ++j) {
                if(m_shop.getCart()[j].getBarcode() == barcode) {
                    // збільшуємо кількість товару на одну одиницю
                    m_shop.getCart()[j].setAmount(m_shop.getCart()[j].getAmount() + 1.0f);
                    ui->CartTable->item(j, 2)->setText(QString::number(m_shop.getCart()[j].getAmount(), 'f', 3));
                    ui->CartTable->item(j, 4)->setText(QString::number(m_shop.getCart()[j].getCost(), 'f', 2));
                    // перерахунок вартості кошику
                    ui->CartCost->setText(QString::number(m_shop.getCartCost(), 'f', 2));
                    break;
                }
            }
            // якщо в кошику такого товару не було
            if(j >= m_shop.getCart().size()) {
                m_shop.getCart().push_back(m_shop.getGoods()[i]);
                ui->CartTable->insertRow(j);
                // блокую сигнали (події), що надходять від таблички, щоб, поки
                // заповнюю таблицю, не викликались такі ф-ції, як (зміна елементу таблиці)
                ui->CartTable->blockSignals(true);
                ui->CartTable->setItem(j, 0, new QTableWidgetItem(
                                           QString::fromLocal8Bit(m_shop.getCart()[j].getName().c_str())));
                ui->CartTable->setItem(j, 1, new QTableWidgetItem((QString) barcode));
                ui->CartTable->setItem(j, 2, new CMyNumberItem(QString::number(m_shop.getCart()[j].getAmount(), 'f', 3)));
                ui->CartTable->setItem(j, 3, new CMyNumberItem(QString::number(m_shop.getCart()[j].getPrice(), 'f', 2)));
                ui->CartTable->setItem(j, 4, new CMyNumberItem(QString::number(m_shop.getCart()[j].getCost(), 'f', 2)));
                for(int k = 0; k < 5; ++k)
                    ui->CartTable->item(j, k)->setFlags(ui->CartTable->item(j, k)->flags() & ~Qt::ItemIsEditable);
                ui->CartTable->item(j, 2)->setFlags(ui->CartTable->item(j, 2)->flags() | Qt::ItemIsEditable);
                ui->CartTable->blockSignals(false);
                // створюю нову кнопку
                QPushButton *delBtn = new QPushButton("X", ui->CartTable);
                // додаємо їй індекс
                delBtn->setProperty("deleteGoodID", j);
                delBtn->setVisible(true);
                // приєднуємо її сигнал кліку до відповідної функції-обробника
                connect(delBtn, SIGNAL(clicked()), this, SLOT(DeleteGood_clicked()));
                // додаємо її на табличку
                ui->CartTable->setCellWidget(j, 5, delBtn);
                ui->CartCost->setText(QString::number(m_shop.getCartCost(), 'f', 2));
            }
            ui->OkButton->setEnabled(true);
            break;
        }
    if(i >= m_shop.getGoods().size()) {
        QMessageBox::critical(this, "Помилка", "Товару зі заданим штрих-кодом не знайдено.");
        return;
    }
}

// Ця функція викликається, коли дані в табличці змінились
void MainWindow::on_CartTable_cellChanged(int row, int column) {
    bool ok;
    float amount = ui->CartTable->item(row, 2)->text().toFloat(&ok);
    if(!ok || amount <= 0.0f)
        amount = 1.0f;
    ui->CartTable->item(row, 2)->setText(QString::number(amount, 'f', 3));
    m_shop.getCart()[row].setAmount(amount);
    ui->CartTable->item(row, 4)->setText(QString::number(m_shop.getCart()[row].getCost(), 'f', 2));
}

// Натиснута кнопка "Розрахувати"
void MainWindow::on_OkButton_clicked() {
    CPayBillDialog payBillDialog(m_shop, this);
    if(payBillDialog.exec() == QDialog::Accepted) {
        ui->OkButton->setEnabled(false);
        ui->CartTable->setRowCount(0);
        m_shop.clearCart();
        ui->CartCost->setText("0.00");
        ui->Cash->setText(QString::number(m_shop.getCash(), 'f', 2));
    }
}

// Обробник закриття вікна програми. Зберігає всі дані в файли
void MainWindow::closeEvent(QCloseEvent *event) {
    m_shop.saveGoods();
    m_shop.saveCards();
}
