#include "readbarcodedialog.h"
#include "ui_readbarcodedialog.h"
#include <QImageReader>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include "codenumber.h"
#include "myexception.h"
#include "barcode.h"

CReadBarcodeDialog::CReadBarcodeDialog(CBarcode &_barcode, QWidget *parent) :
        QDialog(parent), ui(new Ui::CReadBarcodeDialog), _code(_barcode) {
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setFixedSize(size());
    ui->ReadFromFile->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
}

CReadBarcodeDialog::~CReadBarcodeDialog() {
    delete ui;
}

void CReadBarcodeDialog::on_CancelButton_clicked() {
    reject();
}

// Функція, що викликається при натисканні кнопки "ОК"
void CReadBarcodeDialog::on_AcceptButton_clicked() {
    // якщо на полі вводу штрих-коду вручну є якийсь текст
    if(ui->lineEditBarcode->text().size() != 0) {
        if(ui->lineEditBarcode->text().size() < 13) {
            QMessageBox::critical(this, "Помилка", "Введено недостатньо цифр (потрібно 13).");
            return;
        }
        try {
            _code = (CBarcode) ui->lineEditBarcode->text();
            if(_code) // перевірка чи штрих-код правильний
                accept();
            else {
                QMessageBox::critical(this, "Помилка", "Зчитаний штрих-код не відповідає правилам стандарту EAN-13.");
                return;
            }
        } catch(CMyException e) {
            QMessageBox::critical(this, "Помилка", QString(e.what()));
            return;
        }
    } else {
        // перевірка чи зчитаний з камери штрих-код правильний
        if(isOk)
            accept();
        else
            QMessageBox::critical(this, "Помилка", "Штрих-код не зчитано зовсім або зчитано не правильно.");
    }
}

// Функція для зчитування штрих-коду з зображення
void CReadBarcodeDialog::on_ReadFromFile_clicked() {
    // встановлюємо початкове значення прапорця
    isOk = false;
    ui->lineEditBarcode->setEnabled(false);
    QFileDialog openImageDialog(this, tr("Завантажити штрих-код"));
    // ця змінна та провірка потрібні для того,
    // щоб встановлення початкової директорії відбулось лише раз
    static bool bFirstDialog = true;
    if (bFirstDialog) {
        bFirstDialog = false;
        // тут будуть зберігатись стандартний шлях до робочого столу
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
        // якщо список пустий (це станеться, якщо не вдасться
        // знайти робочий стіл), то ставимо директорію, де є
        // виконавчий файл цієї програми; інакше - шлях до роб. столу
        openImageDialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    // запускаємо діалог на виконання і провіряємо чи він виконався успішно
    if(openImageDialog.exec() != QDialog::Accepted)
        return;
    // зчитуємо файл, який було обрано
    QImageReader reader(openImageDialog.selectedFiles().first());
    QImage barcode = reader.read();

    // тут будуть зберігатись біти, які зчитані з штрих-коду
    static bool abBits[84];
    // довжина однієї чорної або білої смужки в пікселях
    static unsigned uLineLen;

    for(int i = 0; i < barcode.height(); ++i) {
        for(int j = 0; j < barcode.width(); ++j)
            if(barcode.pixelColor(j, i).value() < 120) {
                // робимо маленький стрибок вниз (бо штрих-код
                // зверху та знизу може бути дещо розмитим)
                i += 2;
                // вимірюємо довжину однієї чорної смужки
                uLineLen = 1;
                for(int k = j + 1; k < barcode.width(); ++k) {
                    if(barcode.pixelColor(k, i).value() < 120)
                        ++uLineLen;
                    else
                        break;
                }

                // пропускаємо контрольну ділянку зліва
                j += uLineLen * 3;

                for(int r = 0; r < 2; ++r) {
                    for(int t = 0; t < 6; ++t)
                        for(int u = 0; u < 7; ++u) {
                            // якщо довжини зображення не вистачає
                            // для того, щоб зчитати інформацію, то вихід

                            if(j >= barcode.width()) {
                                isOk = false;
                                QMessageBox::critical(this, "Помилка",
                                                      "Зображення зі штрих-кодом не має достатньо інформації.");
                                return;
                            }
                            // зчитуємо біт
                            abBits[(r * 6 + t) * 7 + u] = (barcode.pixelColor(j, i).value() < 120);
                            j += uLineLen;
                        }
                    // пропускаємо контрольну ділянку по центру
                    j += uLineLen * 5;
                }

                // цей рядок коду потрібен для того щоб обірвати й цикл змінної "i"
                i = barcode.height();
                break;
            }
        // якщо цикл дійшов до низу зображення, але так і
        // не знайшов темних пікселів з інформацією, то вихід
        if(barcode.height() - i == 1) {
            isOk = false;
            QMessageBox::critical(this, "Помилка", "Зображення зі штрих-кодом не містить інформації для зчитування.");
            return;
        }
    }

    // перетворюємо набір бітів на масив десяткових чисел
    static CCodeNumber aNumbers[13];
    try {
        for(int i = 0; i < 12; ++i)
            aNumbers[i + 1] = CCodeNumber(&abBits[i * 7]);
    } catch(CMyException e) {
        isOk = false;
        QMessageBox::critical(this, "Помилка", QString(e.what()));
        return;
    }

    // це коди для визначення першої цифри
    static const unsigned auCalculatedNumber[10] = {
        0, 11, 13, 14, 19, 25, 28, 21, 22, 26
    };

    // підраховуємо код першої цифри за допомогою
    // аналізу розсташування G-кодів в лівій секції
    // штрих-коду
    static unsigned uCalcNumberValue, uAddition;
    uCalcNumberValue = 0;
    for(int i = 0; i < 5; ++i) {
        if(aNumbers[i + 2].getType() == CCodeNumber::G) {
            uAddition = 1;
            for(int j = 0; j < 4 - i; ++j)
                uAddition *= 2;
            uCalcNumberValue += uAddition;
        } else if(aNumbers[i + 2].getType() == CCodeNumber::WRONG) {
            isOk = false;
            QMessageBox::critical(this, "Помилка", "Не вдається розкодувати біти в штрих-коді.");
            return;
        }
    }

    // проводимо пошук по списку кодів для першої цифри
    // та встановлюємо її
    for(int k = 0; k < 10; ++k)
        if(auCalculatedNumber[k] == uCalcNumberValue) {
            aNumbers[0] = CCodeNumber(k);
            break;
        }

    // якщо не вдалось підрахувати першу цифру, то вихід
    if(aNumbers[0].getType() == CCodeNumber::WRONG) {
        isOk = false;
        QMessageBox::critical(this, "Помилка", "Не вдалось розшифрувати першу цифру штрих-коду.");
        return;
    }

    try {
        CBarcode code(aNumbers);
        if(code) {
            // якщо штрих-код вдало зчитано, розкодовано, якщо він пройшов провірку контрольної цифри, то...
            isOk = true;
            ui->barcodeLabel->setText(code);
            _code = code;
        } else {
            isOk = false;
            QMessageBox::critical(this, "Помилка", "Зчитаний штрих-код не віповідає правилам стандарту EAN-13.");
            return;
        }
    } catch(CMyException e) {
        isOk = false;
        QMessageBox::critical(this, "Помилка", QString(e.what()));
        return;
    }
}

// Кнопка, що очищує зчитаний з зображення штрих-код
void CReadBarcodeDialog::on_clearReadBarcode_clicked() {
    ui->barcodeLabel->setText("-");
    ui->lineEditBarcode->setEnabled(true);
    isOk = false;
}

// Коли відбулись зміни в полі для штрих-коду
void CReadBarcodeDialog::on_lineEditBarcode_textChanged(const QString &arg1) {
    if(arg1.size() == 0) {
        ui->ReadFromFile->setEnabled(true);
        ui->clearReadBarcode->setEnabled(true);
    } else {
        ui->ReadFromFile->setEnabled(false);
        ui->clearReadBarcode->setEnabled(false);
    }
}
