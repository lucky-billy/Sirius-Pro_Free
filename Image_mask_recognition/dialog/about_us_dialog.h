#ifndef ABOUT_US_DIALOG_H
#define ABOUT_US_DIALOG_H

#include <QDialog>

namespace Ui {
class About_us_Dialog;
}

class About_us_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit About_us_Dialog(QWidget *parent = nullptr);
    ~About_us_Dialog();

private:
    Ui::About_us_Dialog *ui;
};

#endif // ABOUT_US_DIALOG_H
