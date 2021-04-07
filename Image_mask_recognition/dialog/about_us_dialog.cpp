#include "about_us_dialog.h"
#include "ui_about_us_dialog.h"

About_us_Dialog::About_us_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About_us_Dialog)
{
    ui->setupUi(this);

    setWindowTitle("About us");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMaximumSize(this->width(), this->height());
    setMinimumSize(this->width(), this->height());
}

About_us_Dialog::~About_us_Dialog()
{
    delete ui;
}
