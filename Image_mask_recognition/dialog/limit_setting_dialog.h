#ifndef LIMIT_SETTING_DIALOG_H
#define LIMIT_SETTING_DIALOG_H

#include <QDialog>

namespace Ui {
class Limit_setting_dialog;
}

class Limit_setting_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Limit_setting_dialog(QString title, QString name, QStringList list, QMap<QString, int> state,
                                  QMap<QString, double> value, QWidget *parent = nullptr);
    ~Limit_setting_dialog();

    void initView(QStringList list, QMap<QString, int> state, QMap<QString, double> value);

signals:
    void changeValue(QStringList list, QList<double> minList, QList<double> maxList);

private slots:
    void on_sureBtn_clicked();

private:
    Ui::Limit_setting_dialog *ui;
};

#endif // LIMIT_SETTING_DIALOG_H
