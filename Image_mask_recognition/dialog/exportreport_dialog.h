#ifndef EXPORTREPORT_DIALOG_H
#define EXPORTREPORT_DIALOG_H

#include <QDialog>

namespace Ui {
class ExportReport_Dialog;
}

class ExportReport_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportReport_Dialog(QWidget *parent = nullptr);
    ~ExportReport_Dialog();

    QStringList getData();

private slots:
    void on_sureBtn_clicked();

private:
    Ui::ExportReport_Dialog *ui;
};

#endif // EXPORTREPORT_DIALOG_H
