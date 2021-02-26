#include "limit_setting_dialog.h"
#include "ui_limit_setting_dialog.h"
#include <QDoubleSpinBox>

Limit_setting_dialog::Limit_setting_dialog(QString title, QString name, QStringList list, QMap<QString, int> state,
                                           QMap<QString, double> value, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Limit_setting_dialog)
{
    ui->setupUi(this);

    setWindowTitle(title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMaximumSize(this->width(), this->height());
    setMinimumSize(this->width(), this->height());

    ui->sureBtn->setText(name);

    initView(list, state, value);
}

Limit_setting_dialog::~Limit_setting_dialog()
{
    delete ui;
}

void Limit_setting_dialog::initView(QStringList list, QMap<QString, int> state, QMap<QString, double> value)
{
    // clear
    ui->tableWidget->clear();

    // column count
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "   Item   " << "   Min   " << "   Max   ");

    // row count
    int rowCount = 0;
    for ( int i = 0; i < list.size() - 14; ++i )
    {
        if ( state.value( list.at(i) ) == 1 ) {
            ui->tableWidget->setRowCount(++rowCount);
            QTableWidgetItem *item = new QTableWidgetItem(list.at(i));
            ui->tableWidget->setItem(rowCount-1, 0, item);

            QDoubleSpinBox *box_min = new QDoubleSpinBox(ui->tableWidget);
            box_min->setRange(-100, 100);
            box_min->setSingleStep(0.1);
            box_min->setDecimals(3);
            box_min->setValue( value.value(list.at(i) + "_min") );
            ui->tableWidget->setCellWidget(rowCount-1, 1, box_min);

            QDoubleSpinBox *box_max = new QDoubleSpinBox(ui->tableWidget);
            box_max->setRange(-100, 100);
            box_max->setSingleStep(0.1);
            box_max->setDecimals(3);
            box_max->setValue( value.value(list.at(i) + "_max") );
            ui->tableWidget->setCellWidget(rowCount-1, 2, box_max);
        }
    }

    // setting
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setFrameShape(QFrame::NoFrame);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
}

void Limit_setting_dialog::on_sureBtn_clicked()
{
    QStringList list;
    QList<double> minList;
    QList<double> maxList;

    for ( int i = 0; i < ui->tableWidget->rowCount(); ++i )
    {
        QString str = ui->tableWidget->item(i, 0)->text();
        double min = static_cast<QDoubleSpinBox *>(ui->tableWidget->cellWidget(i, 1))->value();
        double max = static_cast<QDoubleSpinBox *>(ui->tableWidget->cellWidget(i, 2))->value();
        list << str;
        minList.append(min);
        maxList.append(max);
    }

    emit changeValue(list, minList, maxList);
    this->close();
}
