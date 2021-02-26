#include "exportreport_dialog.h"
#include "ui_exportreport_dialog.h"
#include "globalfun.h"

ExportReport_Dialog::ExportReport_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportReport_Dialog)
{
    ui->setupUi(this);

    setWindowTitle("Export Report");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMaximumSize(this->width(), this->height());
    setMinimumSize(this->width(), this->height());

    if ( GlobalValue::file_pdt_name != "null" ) { ui->lineEdit_1->setText(GlobalValue::file_pdt_name); }
    if ( GlobalValue::file_pdt_id != "null" ) { ui->lineEdit_2->setText(GlobalValue::file_pdt_id); }
    if ( GlobalValue::file_qua_tester != "null" ) { ui->lineEdit_3->setText(GlobalValue::file_qua_tester); }
    if ( GlobalValue::file_company_name != "null" ) { ui->lineEdit_5->setText(GlobalValue::file_company_name); }

    ui->checkBox_2D->setChecked(true);
    ui->checkBox_3D->setChecked(false);
    connect(ui->checkBox_2D, &QCheckBox::toggled, [&](bool state){ ui->checkBox_3D->setChecked(!state); });
    connect(ui->checkBox_3D, &QCheckBox::toggled, [&](bool state){ ui->checkBox_2D->setChecked(!state); });
}

ExportReport_Dialog::~ExportReport_Dialog()
{
    delete ui;
}

QStringList ExportReport_Dialog::getData()
{
    QStringList list;
    QString line1 = ui->lineEdit_1->text();
    QString line2 = ui->lineEdit_2->text();
    QString line3 = ui->lineEdit_3->text();
    QString line4 = ui->lineEdit_4->text() == "" ? "nan" : ui->lineEdit_4->text();
    QString line5 = ui->lineEdit_5->text() == "" ? "nan" : ui->lineEdit_5->text();
    QString line6 = ui->checkBox_2D->isChecked() ? "2" : "3";
    list << line1 << line2 << line3 << line4 << line5 << line6;
    return list;
}

void ExportReport_Dialog::on_sureBtn_clicked()
{
    if ( ui->lineEdit_1->text() == "" ) {
        GlobalFun::showMessageBox(3, "Product Name can not be empty !");
    } else if ( ui->lineEdit_2->text() == "" ) {
        GlobalFun::showMessageBox(3, "Product ID can not be empty !");
    } else if ( ui->lineEdit_3->text() == "" ) {
        GlobalFun::showMessageBox(3, "Quality Tester can not be empty !");
    } else {
        GlobalValue::file_pdt_name = ui->lineEdit_1->text();
        GlobalValue::file_pdt_id = ui->lineEdit_2->text();
        GlobalValue::file_qua_tester = ui->lineEdit_3->text();
        GlobalValue::file_company_name = ui->lineEdit_5->text();


        emit accept();
        this->close();
    }
}
