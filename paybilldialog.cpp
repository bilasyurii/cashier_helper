#include "paybilldialog.h"
#include "ui_paybilldialog.h"
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QMessageBox>
#include <QTime>
#include <QDate>
#include "myexception.h"

CPayBillDialog::CPayBillDialog(CShop &shop, QWidget *parent) :
        QDialog(parent), ui(new Ui::CPayBillDialog), m_shop(shop) {
    ui->setupUi(this);
    QFont font = ui->costLabel->font();
    font.setPointSize(20);
    ui->costLabel->setFont(font);
    ui->paidLine->setStyleSheet("border: none; background-color: rgba(255, 255, 255, 0)");
    ui->paidLine->setFont(font);
    ui->changeLabel->setFont(font);
    ui->costLabel->setText(QString::number(shop.getCartCost(), 'f', 2));
    font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->receipt->setFont(font);
    setFixedWidth(width());
    setFixedHeight(176);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->bonusButton->setEnabled(false);
    ui->receipt->setVisible(false);
    ui->createCard->setVisible(false);
    ui->okButton->setVisible(false);
    // тут валідатор потрібен для того, щоб дозволяти користувачу
    // вводити лише дробові числа і лише в правильному форматі
    validator = new QDoubleValidator(0.0, 10000.0, 2);
    validator->setNotation(QDoubleValidator::StandardNotation);
    QLocale locale(QLocale::C);
    locale.setNumberOptions(QLocale::RejectGroupSeparator);
    validator->setLocale(locale);
    ui->paidLine->setValidator(validator);
    // створення "гарячих клавіш"
    ui->bonusButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    ui->printButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
}

CPayBillDialog::~CPayBillDialog() {
    delete validator;
    delete ui;
}

// Функція для введення номера картки клієнта
void CPayBillDialog::on_cardButton_clicked() {
    unsigned id = ui->cardLineEdit->text().toUInt();
    if(ui->cardLineEdit->text().size() != 8) {
        QMessageBox::critical(this, "Помилка", "Номер картки має містити 8 цифр.");
        return;
    }
    // пошук картки в базі карток
    std::map<unsigned, CCard>::iterator it = m_shop.getCards().find(id);
    if(it == m_shop.getCards().end()) {
        QMessageBox::critical(this, "Помилка", "Номер картки не знайдено.");
        return;
    } else {
        usesCard = true;
        card = (*it).second;
        ui->discountLabel->setText(QString::number(card.getDiscount(), 'f', 2));
        ui->costLabel->setText(QString::number(m_shop.getCartCost() *
                                               (100.0f - ui->discountLabel->text().toFloat()) / 100, 'f', 2));
        card.setBonus(QString::number(card.getBonus() + ui->costLabel->text().toFloat() / 100).toFloat());
        ui->bonusLabel->setText(QString::number(card.getBonus(), 'f', 2));
        ui->cardLineEdit->setEnabled(false);
        ui->cardButton->setEnabled(false);
        // знімати бонуси можна лише досягнувши позначки 10 у.о.
        if(card.getBonus() >= 10.0f)
            ui->bonusButton->setEnabled(true);
        // виклик функції, що рахує решту для клієнта
        on_paidLine_textEdited(ui->paidLine->text());
    }
}

// Функція виводу чеку та його друку
void CPayBillDialog::on_printButton_clicked() {
    QString text, line, temp;
    std::string name;
    CGood good;
    // формуємо чек
    text = "Назва товару                  |  Ціна | К-сть | Вартість\n";
    for(unsigned i = 0; i < m_shop.getCart().size(); ++i) {
        line = "";
        line.resize(56, ' ');
        line += "\n";
        good = m_shop.getCart()[i];
        name = good.getName();
        if(name.size() != 30)
            name.resize(30, ' ');
        temp = QString::fromLocal8Bit(name.c_str());
        for(int j = 0; j < 30; ++j)
            line[j] = temp[j];
        temp = QString::number(good.getPrice(), 'f', 2).rightJustified(7);
        for(int j = 0; j < 7; ++j)
            line[j + 31] = temp[j];
        temp = QString::number(good.getAmount(), 'f', 2).rightJustified(7);
        for(int j = 0; j < 7; ++j)
            line[j + 39] = temp[j];
        temp = QString::number(good.getCost(), 'f', 2).rightJustified(9);
        for(int j = 0; j < 9; ++j)
            line[j + 47] = temp[j];
        text += line;
    }
    text += "--------------------------------------------------------\n";
    line = QString::number(m_shop.getCartCost(), 'f', 2).rightJustified(56);
    temp = "СУМА";
    for(int i = 0; i < 4; ++i)
        line[i] = temp[i];
    text += line + "\n";
    if(usesCard) {
        line = QString::number(ui->costLabel->text().toFloat(), 'f', 2).rightJustified(56);
        temp = "СУМА ЗІ ЗНИЖКОЮ ТА БОНУСАМИ";
        for(int i = 0; i < 27; ++i)
            line[i] = temp[i];
        text += line + "\n";
    }
    line = QString::number(ui->paidLine->text().toFloat(), 'f', 2).rightJustified(56);
    temp = "ОПЛАЧЕНО";
    for(int i = 0; i < 8; ++i)
        line[i] = temp[i];
    text += line + "\n";
    line = QString::number(ui->changeLabel->text().toFloat(), 'f', 2).rightJustified(56);
    temp = "РЕШТА";
    for(int i = 0; i < 5; ++i)
        line[i] = temp[i];
    text += line + "\n";
    text += "--------------------------------------------------------\n";
    line = QString("%1 %2").arg(QTime::currentTime().toString()).
                            arg(QDate::currentDate().toString("dd.MM.yyyy")).rightJustified(56, ' ');
    temp = "ЧАС ТА ДАТА";
    for(int i = 0; i < 11; ++i)
        line[i] = temp[i];
    text += line + "\n";

    // зберігаємо зміни на картці клієнта в базі
    m_shop.addToCash(ui->costLabel->text().toFloat());
    card.setSum(card.getSum() + ui->costLabel->text().toFloat());
    m_shop.getCards()[card.getId()] = card;

    success = true;
    ui->paySection->setVisible(false);
    ui->receipt->setVisible(true);
    ui->okButton->setVisible(true);
    ui->createCard->setVisible(!usesCard);
    setFixedHeight(250);
    setFixedWidth(450);
    ui->receipt->setPlainText(text);

    // створюємо новий об'єкт принтера
    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer);
    dialog->setWindowTitle("Друк чеку");
    if (dialog->exec() == QDialog::Accepted) {
        // клас QPainter надає функціонал для зручного малювання
        // на різноманітних об'єктах (до яких входить QPrinter)
        QPainter painter;
        painter.begin(&printer);
        painter.setFont(ui->receipt->font());
        painter.drawText(100, 100, 500, 500, Qt::AlignLeft|Qt::AlignTop, text);
        painter.end();
    }
}

// Якщо натиснута кнопка використання бонусів
void CPayBillDialog::on_bonusButton_clicked() {
    ui->bonusButton->setEnabled(false);
    float cost = ui->costLabel->text().toFloat();
    // якщо сума до оплати менша, ніж кількість бонусів
    if(cost < card.getBonus()) {
        card.setBonus(card.getBonus() - cost);
        ui->costLabel->setText("0.00");
    } else { // якщо сума до оплати більша, ніж кількість бонусів
        ui->costLabel->setText(QString::number(cost - card.getBonus(), 'f', 2));
        card.setBonus(0.0f);
    }
    ui->bonusLabel->setText(QString::number(card.getBonus(), 'f', 2));
    on_paidLine_textEdited(ui->paidLine->text());
}

void CPayBillDialog::on_okButton_clicked() {
    accept();
}

// Перезаписування стандартної функції діалогів, що
// викликається, якщо натиснута кнопка "скасувати" або
// "закрити вікно"
void CPayBillDialog::reject() {
    // якщо оплата пройшла успішно, то повертаємо код Accepted, а не Rejected
    if(success)
        accept();
    else
        QDialog::reject();
}

// Функція для створення нової картки клієнта
void CPayBillDialog::on_createCard_clicked() {
    try {
        // отримуємо вільний ID картки
        unsigned cardID = m_shop.getNewCardID();
        QString cardString = QString::number(cardID).rightJustified(8, '0');
        QMessageBox::information(this, "Картка створена", QString("Номер вашої картки: %1").arg(cardString));
        // додаємо картку в базу
        m_shop.getCards()[cardID] = CCard(cardID);
    } catch(CMyException &e) {
        QMessageBox::critical(this, "Помилка", e.what());
    }
    ui->createCard->setEnabled(false);
}

// Функція, що викликається, коли користувач
// вводить суму, яку заплатив клієнт
void CPayBillDialog::on_paidLine_textEdited(const QString &arg1) {
    // якщо рядок "оплачено" пустий...
    if(arg1.length() == 0) {
        ui->printButton->setEnabled(false);
        ui->changeLabel->setText("0.00");
    } else { // інакше підраховуємо решту, та виводимо на екран
        float change = ui->paidLine->text().toFloat() - ui->costLabel->text().toFloat();
        ui->printButton->setEnabled(change >= 0);
        ui->changeLabel->setText((change > 0)?QString::number(change, 'f', 2):"0.0");
    }
}
