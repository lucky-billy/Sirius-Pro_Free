#ifndef RESULT_SETTING_DIALOG_H
#define RESULT_SETTING_DIALOG_H

#include <QDialog>
#include <QButtonGroup>

namespace Ui {
class Result_setting_dialog;
}

class Result_setting_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Result_setting_dialog(QString title, QString name, QStringList list, QMap<QString, int> map, QWidget *parent = nullptr);
    ~Result_setting_dialog();

signals:
    void changeValue(QStringList list);

private slots:
    void on_sureBtn_clicked();

private:
    Ui::Result_setting_dialog *ui;
    QButtonGroup *m_group;
};

#endif // RESULT_SETTING_DIALOG_H
