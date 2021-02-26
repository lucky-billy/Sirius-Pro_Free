#include "result_setting_dialog.h"
#include "ui_result_setting_dialog.h"
#include <QLayout>
#include <QRadioButton>
#include <QLabel>
#include <QDebug>

Result_setting_dialog::Result_setting_dialog(QString title, QString name, QStringList list, QMap<QString, int> map, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Result_setting_dialog)
{
    ui->setupUi(this);

    setWindowTitle(title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMaximumSize(this->width(), this->height());
    setMinimumSize(this->width(), this->height());

    ui->sureBtn->setText(name);

    ui->scrollArea->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    ui->scrollArea->setWidgetResizable(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSizeConstraint(QVBoxLayout::SetMinAndMaxSize);

    QString style = "background-color: rgb(170, 85, 0); color: rgb(255, 255, 255);font: 15pt \"Agency FB\";";

    m_group = new QButtonGroup(this);
    m_group->setExclusive(false);
    for ( int i = 0; i < list.size(); ++i )
    {
        QRadioButton *btn = new QRadioButton(list.at(i), this);
        btn->setChecked(map.value(list.at(i)) == 1);
        m_group->addButton(btn);
    }

    for ( int i = 0; i < list.size(); ++i )
    {
        if ( i == 0 ) {
            QLabel* label = new QLabel("PV & RMS");
            label->setStyleSheet(style);
            layout->addWidget(label);
        } else if ( i == 12 ) {
            QLabel* label = new QLabel("Zernike & Seidel");
            label->setStyleSheet(style);
            layout->addWidget(label);
        } else if ( i == 31 ) {
            QLabel* label = new QLabel("ISO10110");
            label->setStyleSheet(style);
            layout->addWidget(label);
        } else if ( i == 37 ) {
            QLabel* label = new QLabel("Other");
            label->setStyleSheet(style);
            layout->addWidget(label);
        } else if ( i == 46 ) {
            QLabel* label = new QLabel("Zernike Coefficients");
            label->setStyleSheet(style);
            layout->addWidget(label);
        }

        QRadioButton *btn = static_cast<QRadioButton*>(m_group->buttons().at(i));
        layout->addWidget(btn);
    }

    QWidget* widget = new QWidget(this);
    widget->setLayout(layout);
    ui->scrollArea->setWidget(widget);
}

Result_setting_dialog::~Result_setting_dialog()
{
    delete ui;
}

void Result_setting_dialog::on_sureBtn_clicked()
{
    QStringList str;
    for ( int i = 0; i < m_group->buttons().size(); ++i )
    {
        QRadioButton *btn = static_cast<QRadioButton*>(m_group->buttons().at(i));
        if ( btn->isChecked() ) {
            str << btn->text();
        }
    }
    changeValue(str);
    this->close();
}
