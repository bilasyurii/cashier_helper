#ifndef FINDGOODDIALOG_H
#define FINDGOODDIALOG_H

#include <QDialog>
#include "shop.h"

namespace Ui {
class CFindGoodDialog;
}

class CFindGoodDialog : public QDialog {
    Q_OBJECT

public:
    explicit CFindGoodDialog(CShop &shop, CBarcode &_barcode, QWidget *parent = nullptr);
    ~CFindGoodDialog();

private slots:
    void on_cancelButton_clicked();
    void on_okButton_clicked();
    void on_clearName_clicked();
    void on_clearBarcode_clicked();
    void on_byNameEdit_textChanged(const QString &arg1);
    void on_byBarcodeEdit_textChanged(const QString &arg1);

private:
    Ui::CFindGoodDialog *ui;
    CShop &m_shop;
    CBarcode &barcode;

    void fillTable();
};

#endif // FINDGOODDIALOG_H
