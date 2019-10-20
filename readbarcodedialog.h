#ifndef READBARCODEDIALOG_H
#define READBARCODEDIALOG_H

#include <QDialog>
#include "barcode.h"

namespace Ui {
class CReadBarcodeDialog;
}

class CReadBarcodeDialog : public QDialog {
    Q_OBJECT
public:
    explicit CReadBarcodeDialog(CBarcode &_barcode, QWidget *parent = nullptr);
    ~CReadBarcodeDialog();

private slots:
    void on_CancelButton_clicked();
    void on_AcceptButton_clicked();
    void on_ReadFromFile_clicked();
    void on_clearReadBarcode_clicked();
    void on_lineEditBarcode_textChanged(const QString &arg1);

private:
    Ui::CReadBarcodeDialog *ui;
    bool isOk = false;
    CBarcode &_code;
};

#endif // READBARCODEDIALOG_H
