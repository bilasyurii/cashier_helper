#ifndef SHOWDATABASEDIALOG_H
#define SHOWDATABASEDIALOG_H

#include <shop.h>
#include <QDialog>
#include <vector>

namespace Ui {
class CShowDatabaseDialog;
}

class CShowDatabaseDialog : public QDialog {
    Q_OBJECT
public:
    explicit CShowDatabaseDialog(CShop &shop, QWidget *parent = nullptr);
    ~CShowDatabaseDialog();

private slots:
    void on_byNameEdit_textChanged(const QString &arg1);
    void on_byBarcodeEdit_textChanged(const QString &arg1);
    void on_clearName_clicked();
    void on_clearBarcode_clicked();
    void on_deleteGood_clicked();
    void on_newGood_clicked();
    void on_cancelGoodButton_clicked();
    void on_addGoodButton_clicked();
    void on_editGood_clicked();
    void on_applyGood_clicked();

private:
    Ui::CShowDatabaseDialog *ui;
    CShop &m_shop;
    int targetIndex;

    void fillTable();
    bool checkName(QString source);
    bool checkBarcode(CBarcode &dest, QString source);
    bool checkPrice(float &dest, QString source);
};

#endif // SHOWDATABASEDIALOG_H
