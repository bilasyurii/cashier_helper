#ifndef PAYBILLDIALOG_H
#define PAYBILLDIALOG_H

#include <QDialog>
#include <QDoubleValidator>
#include "shop.h"

namespace Ui {
class CPayBillDialog;
}

class CPayBillDialog : public QDialog {
    Q_OBJECT
public:
    explicit CPayBillDialog(CShop &shop, QWidget *parent = nullptr);
    ~CPayBillDialog();

private slots:
    void on_cardButton_clicked();
    void on_printButton_clicked();
    void on_bonusButton_clicked();
    void on_okButton_clicked();
    void reject();
    void on_createCard_clicked();
    void on_paidLine_textEdited(const QString &arg1);

private:
    Ui::CPayBillDialog *ui;
    bool success = false;
    bool usesCard = false;
    QDoubleValidator *validator;
    CShop &m_shop;
    CCard card;
};

#endif // PAYBILLDIALOG_H
