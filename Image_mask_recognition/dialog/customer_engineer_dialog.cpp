#include "customer_engineer_dialog.h"
#include "ui_customer_engineer_dialog.h"
#include "globalfun.h"

Customer_engineer_dialog::Customer_engineer_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Customer_engineer_dialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMaximumSize(this->width(), this->height());
    setMinimumSize(this->width(), this->height());

    initView();
}

Customer_engineer_dialog::~Customer_engineer_dialog()
{
    delete ui;
}

void Customer_engineer_dialog::initView()
{
    if ( GlobalValue::lgn_tp == 1 ) {
        setWindowTitle(QStringLiteral("设置"));

        ui->groupBox_1->setTitle(QStringLiteral("功能选项"));
        ui->label_1->setText(QStringLiteral("移相:"));
        ui->label_2->setText(QStringLiteral("解包裹:"));
        ui->label_3->setText(QStringLiteral("滤波:"));
        ui->label_4->setText(QStringLiteral("去毛刺:"));
        ui->label_5->setText(QStringLiteral("去残差:"));
        ui->label_6->setText(QStringLiteral("比例因子:"));
        ui->label_7->setText(QStringLiteral("测试波长:"));
        ui->label_8->setText(QStringLiteral("ISO波长:"));
        ui->label_9->setText(QStringLiteral("折射率:"));
        ui->label_10->setText(QStringLiteral("滤波窗尺寸:"));
        ui->label_11->setText(QStringLiteral("去毛刺像素值:"));
        ui->label_12->setText(QStringLiteral("去毛刺阈值:"));

        ui->groupBox_2->setTitle(QStringLiteral("其他设置"));
        ui->label_13->setText(QStringLiteral("小数点位数:"));
        ui->label_14->setText(QStringLiteral("图像尺寸:"));
        ui->label_15->setText(QStringLiteral("残差点个数:"));
        ui->label_16->setText(QStringLiteral("设备名称:"));
        ui->label_17->setText(QStringLiteral("设备序列号:"));
    } else {
        setWindowTitle(QStringLiteral("Setting"));

        ui->groupBox_1->setTitle(QStringLiteral("Function options"));
        ui->label_1->setText(QStringLiteral("PSI:"));
        ui->label_2->setText(QStringLiteral("UNWRAP:"));
        ui->label_3->setText(QStringLiteral("Filter:"));
        ui->label_4->setText(QStringLiteral("Remove spikes:"));
        ui->label_5->setText(QStringLiteral("Remove residual:"));
        ui->label_6->setText(QStringLiteral("Scale factor:"));
        ui->label_7->setText(QStringLiteral("Test wavelength:"));
        ui->label_8->setText(QStringLiteral("ISO wavelength:"));
        ui->label_9->setText(QStringLiteral("Refractive index:"));
        ui->label_10->setText(QStringLiteral("Filter winSize:"));
        ui->label_11->setText(QStringLiteral("Slope rmSize:"));
        ui->label_12->setText(QStringLiteral("Slope thresh:"));

        ui->groupBox_2->setTitle(QStringLiteral("Other setting"));
        ui->label_13->setText(QStringLiteral("Decimal points:"));
        ui->label_14->setText(QStringLiteral("Graph size:"));
        ui->label_15->setText(QStringLiteral("Residual points:"));
        ui->label_16->setText(QStringLiteral("Device name:"));
        ui->label_17->setText(QStringLiteral("Serial number:"));
    }

    //------------------------------------------------------------------------------

    switch ( GlobalValue::par_psi )
    {
    case 1: ui->psi_box->setCurrentIndex(0); break;
    case 3: ui->psi_box->setCurrentIndex(1); break;
    case 5: ui->psi_box->setCurrentIndex(2); break;
    default: ui->psi_box->setCurrentIndex(0); break;
    }

    switch (GlobalValue::par_unw)
    {
    case 0: ui->unwrap_box->setCurrentIndex(0); break;
    case 2: ui->unwrap_box->setCurrentIndex(1); break;
    default: ui->unwrap_box->setCurrentIndex(0); break;
    }

    ui->filter_box->setCurrentIndex(GlobalValue::par_flt);

    ui->r_s_yes->setChecked(GlobalValue::par_rm_spk == 1);
    ui->r_s_no->setChecked(GlobalValue::par_rm_spk == 0);
    ui->r_r_yes->setChecked(GlobalValue::par_rm_rsl == 1);
    ui->r_r_no->setChecked(GlobalValue::par_rm_rsl == 0);
    connect(ui->r_s_yes, &QCheckBox::toggled, [&](bool state){ ui->r_s_no->setChecked(!state); });
    connect(ui->r_s_no, &QCheckBox::toggled, [&](bool state){ ui->r_s_yes->setChecked(!state); });
    connect(ui->r_r_yes, &QCheckBox::toggled, [&](bool state){ ui->r_r_no->setChecked(!state); });
    connect(ui->r_r_no, &QCheckBox::toggled, [&](bool state){ ui->r_r_yes->setChecked(!state); });

    ui->i_s_f_box->setValue(GlobalValue::par_i_s_f);
    ui->t_w_box->setValue(GlobalValue::par_t_w);
    ui->i_w_box->setValue(GlobalValue::par_i_w);
    ui->r_i_box->setValue(GlobalValue::par_ref_index);
    ui->fws_box->setCurrentText(QString::number(GlobalValue::par_fws));
    ui->srs_box->setCurrentIndex(GlobalValue::par_srs);
    ui->st_box->setCurrentIndex(GlobalValue::par_sth);

    //------------------------------------------------------------------------------

    ui->decimal_box->setCurrentText(QString::number(GlobalValue::com_dcl));
    ui->def_size_box->setValue(GlobalValue::gra_def_size);
    ui->res_num_box->setValue(GlobalValue::com_rnt);
    ui->deviceName->setText(GlobalValue::file_device_name);
    ui->serialNum->setText(GlobalValue::file_device_id);
}

void Customer_engineer_dialog::on_ok_btn_clicked()
{
    if ( GlobalValue::gra_def_size != ui->def_size_box->value() ) {
        if ( GlobalValue::gra_pixel == 0 ) {
            GlobalFun::showMessageBox(3, "The element size cannot be modified \nwhen the pixel calibration value is 0 !");
            return;
        }
    }

    switch ( ui->psi_box->currentIndex() )
    {
    case 0: GlobalValue::par_psi = 1; break;
    case 1: GlobalValue::par_psi = 3; break;
    case 2: GlobalValue::par_psi = 5; break;
    default: GlobalValue::par_psi = 1; break;
    }

    switch ( ui->unwrap_box->currentIndex() )
    {
    case 0: GlobalValue::par_unw = 0; break;
    case 1: GlobalValue::par_unw = 2; break;
    default: GlobalValue::par_unw = 0; break;
    }

    GlobalValue::par_flt = ui->filter_box->currentIndex();

    GlobalValue::par_rm_spk = ui->r_s_yes->isChecked() ? 1 : 0;
    GlobalValue::par_rm_rsl = ui->r_r_yes->isChecked() ? 1 : 0;

    GlobalValue::par_i_s_f = ui->i_s_f_box->value();
    GlobalValue::par_t_w = ui->t_w_box->value();
    GlobalValue::par_i_w = ui->i_w_box->value();
    GlobalValue::par_ref_index = ui->r_i_box->value();
    GlobalValue::par_fws = ui->fws_box->currentText().toInt();
    GlobalValue::par_srs = ui->srs_box->currentIndex();
    GlobalValue::par_sth = ui->st_box->currentIndex();

    //------------------------------------------------------------------------------

    GlobalValue::com_dcl = ui->decimal_box->currentText().toInt();
    GlobalValue::gra_def_size = ui->def_size_box->value();
    GlobalValue::com_rnt = ui->res_num_box->value();
    GlobalValue::file_device_name = ui->deviceName->text();
    GlobalValue::file_device_id = ui->serialNum->text();

    this->close();
}
