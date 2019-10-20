#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <shop.h>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ReadBarcode_clicked();
    void on_ShowGoods_2_clicked();
    void on_FindAddGoods_clicked();
    void on_CartTable_cellChanged(int row, int column);
    void DeleteGood_clicked();
    void on_OkButton_clicked();
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    CShop m_shop;

    void addToCart(CBarcode &barcode);
};

#endif // MAINWINDOW_H
