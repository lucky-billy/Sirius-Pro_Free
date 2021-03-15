#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <thread>
#include <QMessageBox>
#include <windows.h>

#include "dialog/customer_engineer_dialog.h"
#include "dialog/unit_setting_dialog.h"
#include "dialog/limit_setting_dialog.h"
#include "dialog/result_setting_dialog.h"
#include "dialog/exportreport_dialog.h"
#include "dialog/slice_analysis_dialog.h"
#include "pztcalibrate.h"
#include "splice.h"

#include <QTableWidgetItem>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QListWidgetItem>
#include <QPixmap>
#include <QtTest/QTest>
#include <qvalidator.h>
#include <QMetaType>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initData();
    initView();
    initGraphicsScene();
}

MainWindow::~MainWindow()
{
    delete m_algorithm;
    delete m_graph;
    delete ui;
}

//******************************************************************************************************************
// [0] 初始化数据

void MainWindow::initData()
{
    qDebug() << "initData !";
    // pv_rms 11 -> 12  zernike 19    ISO1010 6    Other 9   zernike_coefficients 9    total 55
    m_algParameter << "PV" << "RMS" << "PVr" << "PV(res)" << "RMS(res)" << "PV(x)" << "RMS(x)" << "PV(y)" << "RMS(y)"
                   << "PV(xy)" << "RMS(xy)" << "GRMS" << "Zernike_Tilt" << "Power" << "Power_X" << "Power_Y"
                   << "Zernike_Ast" << "Zernike_Coma" << "Zernike_SA" << "RMS(Power)" << "RMS(Ast)" << "RMS(Coma)" << "RMS(SA)"
                   << "Seidel_Tilt" << "Seidel_Tilt_Clock" << "Seidel_Focus" << "Seidel_Ast" << "Seidel_Ast_Clock"
                   << "Seidel_Coma" << "Seidel_Coma_Clock" << "Seidel_SA" << "SAG" << "IRR" << "RSI" << "RMSt"
                   << "RMSa" << "RMSi" << "TTV" << "Fringes" << "Strehl_Ratio" << "Parallel_Theta"
                   << "Aperture" << "SizeX" << "SizeY" << "Concavity" << "Curvature_Radius"
                   << "Z1" << "Z2" << "Z3" << "Z4" << "Z5" << "Z6" << "Z7" << "Z8" << "Z9";

    for ( int i = 0; i < m_algParameter.size() - 14; ++i )
    {
        QString lower_limit = m_algParameter.at(i) + "_min";
        QString upper_limit = m_algParameter.at(i) + "_max";
        m_algParameterLimit << lower_limit << upper_limit;
    }

    GlobalString::table_style = "QHeaderView { font-size: 12px }"
                                "QHeaderView::section { background-color: #2c3e50;"
                                                        "color: white;"
                                                        "height: 24px;"
                                                        "border-left: 2px solid #FFFFFF;"
                                                        "border-right: 2px solid #FFFFFF; }"
                                "QHeaderView::section:checked { background-color: #8B0000; }";

    //------------------------------------------------------------------------------

    createDefaultConfig();
    loadConfig();
    changeLanguage(GlobalValue::lgn_tp);

    calcConvertMatrix();    // 转换矩阵为常量

    CONFIG_PARAMS config;
    m_algorithm = new AlgorithmMain(config);

    m_2DPlotType = 1;
    m_3DPlotType = 1;
    loadFileState = false;
    isSirius = false;

    calc_type = 7;
    asc_res = 1;

    //------------------------------------------------------------------------------

    QString fileName = GlobalFun::getCurrentPath() + "/log.txt";
    if ( GlobalFun::isFileExist(fileName) ) {
        m_log.openFile(fileName, QFile::ReadOnly);
        QString date = m_log.read(true);
        m_log.closeFile();

        QDate oldTime = QDate::fromString(date, "yyyy-MM-dd");
        QDate newTime = QDate::currentDate();
        qint64 day = oldTime.daysTo(newTime);
        if (day >= 7) {
            GlobalFun::removeFile(fileName);
            m_log.openFile(fileName, QFile::WriteOnly | QFile::Truncate);
            m_log.write(GlobalFun::getCurrentTime(3) + "\n");
        } else {
            m_log.openFile(fileName, QFile::WriteOnly | QFile::Append);
        }

    } else {
        m_log.openFile(fileName, QFile::WriteOnly | QFile::Truncate);
        m_log.write(GlobalFun::getCurrentTime(3) + "\n");
    }

    m_log.write(GlobalFun::getCurrentTime(2) + " - application power on\n");

    //------------------------------------------------------------------------------

    connect(this, &MainWindow::screenshot, [&](QString str){
        QString screenshot = str + "/screenshot.png";
        QPixmap::grabWindow(this->winId()).save(screenshot);
    });
}

void MainWindow::initView()
{
    qDebug() << "initView !";
    setWindowTitle("Sirius Pro");
    setMinimumSize(1200, 700);

    QFont font;
    font.setPixelSize(GlobalValue::com_fps);
    setFont(font);

    ui->action_Piston->setChecked(GlobalValue::zer_pis == 1);
    ui->action_Tilt->setChecked(GlobalValue::zer_tilt == 1);
    ui->action_Power->setChecked(GlobalValue::zer_pow == 1);
    ui->action_Ast->setChecked(GlobalValue::zer_ast == 1);
    ui->action_Coma->setChecked(GlobalValue::zer_coma == 1);
    ui->action_Spherical->setChecked(GlobalValue::zer_sph == 1);

    ui->toolBar->setStyleSheet("QToolButton { font-size: 14px;"
                                             "border: 2px;"
                                             "border-color: transparent;"
                                             "border-style:solid; }"
                               "QToolButton::hover { background-color: rgb(192, 220, 243);"
                                                    "font-size: 14px;"
                                                    "border: 2px;"
                                                    "border-color: transparent;"
                                                    "border-radius: 4px;"
                                                    "border-style: solid; }"
                               "QToolButton::checked { background-color: rgb(192, 220, 243);"
                                                      "font-size: 14px;"
                                                      "border: 2px;"
                                                      "border-color: transparent;"
                                                      "border-radius: 4px;"
                                                      "border-style: solid; }");

    // menu file
    connect(ui->action_load, &QAction::triggered, [&](){
        loadResource();
        m_calc_base = cv::Mat();
        m_calc_ope = cv::Mat();
    });
    connect(ui->action_save, &QAction::triggered, [&](){ saveResource(); });
    connect(ui->action_screenshot, &QAction::triggered, [&](){
        QString path = GlobalFun::getCurrentTime(1);
        if ( GlobalValue::file_save_path != "null" ) { path = GlobalValue::file_save_path + "/" + path; }

        QString file = QFileDialog::getSaveFileName(nullptr, GlobalString::action_screenshot, path, "Image files (*.png)");
        if ( file == "" ) {
            return;
        }
        QPixmap::grabWindow(this->winId()).save(file);
    });
    connect(ui->action_export_report, &QAction::triggered, [&](){
        if ( exportReport() ) {
            GlobalFun::showMessageBox(2, "Save PDF file successfully !");
        }
    });

    // menu language
    connect(ui->action_chinese, &QAction::triggered, [&](){ changeLanguage(1); });
    connect(ui->action_english, &QAction::triggered, [&](){ changeLanguage(2); });

    connect(ui->action_analysis, &QAction::triggered, [&](){
        if ( m_matList.size() == 0 ) {
            GlobalFun::showMessageBox(3, "No pictures !");
        } else if ( !GlobalFun::judVecAvailable(m_matList) ) {
            GlobalFun::showMessageBox(3, "Image is empty !");
        } else {
            calc_type = 1;
            changeStatus(false);
            startCalculation();
        }
    });

    connect(ui->action_sliceAnalysis, &QAction::triggered, [&](){
        if ( m_algorithm->errorType == ERROR_TYPE_NOT_ERROR && !m_algorithm->fittingSurface.empty() ) {
            createMenuDialog(11);
        } else {
            GlobalFun::showMessageBox(3, "The data is incorrect and cannot be analyzed !");
        }
    });

    connect(ui->action_setting, &QAction::triggered, [&](){ createMenuDialog(6); });

    connect(ui->action_Piston, &QAction::triggered, [&](bool checked){ GlobalValue::zer_pis = checked ? 1 : 0; });
    connect(ui->action_Tilt, &QAction::triggered, [&](bool checked){ GlobalValue::zer_tilt = checked ? 1 : 0; });
    connect(ui->action_Power, &QAction::triggered, [&](bool checked){ GlobalValue::zer_pow = checked ? 1 : 0; });
    connect(ui->action_Ast, &QAction::triggered, [&](bool checked){ GlobalValue::zer_ast = checked ? 1 : 0; });
    connect(ui->action_Coma, &QAction::triggered, [&](bool checked){ GlobalValue::zer_coma = checked ? 1 : 0; });
    connect(ui->action_Spherical, &QAction::triggered, [&](bool checked){ GlobalValue::zer_sph = checked ? 1 : 0; });

    switch (GlobalValue::com_unit) {
    case 0: ui->action_Company->setText(QStringLiteral("λ@") + QString::number(GlobalValue::com_value)); break;
    case 1: ui->action_Company->setText(QStringLiteral("fr@") + QString::number(GlobalValue::com_value/2)); break;
    case 2: ui->action_Company->setText(QStringLiteral("nm")); break;
    case 3: ui->action_Company->setText(QStringLiteral("μm")); break;
    case 4: ui->action_Company->setText(QStringLiteral("mm")); break;
    default: ui->action_Company->setText(QStringLiteral("nm")); break;
    }
    ui->action_Company->setFont(QFont(GlobalFun::getTTF(5)));
    connect(ui->action_Company, &QAction::triggered, [&](){ createMenuDialog(8); });

    //------------------------------------------------------------------------------

    createResultTable();
    createZernikeTable();
    createDataTable();
    createQualityTable();
    create2DView();
    create3DView();
    createLineChart();
    createPSFView();

    connect(ui->result_table, &QTableWidget::customContextMenuRequested, [&](){ createContextMenu(1); });
    connect(ui->zernike_table, &QTableWidget::customContextMenuRequested, [&](){ createContextMenu(2); });
    connect(ui->data_table, &QTableWidget::customContextMenuRequested, [&](){ createContextMenu(3); });
    connect(ui->quality_table, &QTableWidget::customContextMenuRequested, [&](){ createContextMenu(4); });
    connect(static_cast<QCustomPlot *>( ui->view_tab1->children().at(0) ),
            &QTableWidget::customContextMenuRequested, [&](){ createContextMenu(6); });

    //------------------------------------------------------------------------------

    // phase calculate
    connect(ui->phaseCalc_load, &QAbstractButton::clicked, [&](){
        calc_type = 1;
        loadOneAsc();
    });

    connect(ui->phaseCalc_inverse, &QAbstractButton::clicked, [&](){
        if( !m_calc_base.empty() ) {
            m_phase = BaseFunc::calculator_rev(m_calc_base);
            m_calc_base = m_phase.clone();

            isSirius = false;
            startCalculation();
        } else {
            GlobalFun::showMessageBox(3, "Please load file first !");
            return;
        }
    });

    connect(ui->phaseCalc_add, &QAbstractButton::clicked, [&](){
        if( !m_calc_base.empty() ) {
            calc_type = 2;
            loadOneAsc();
        } else {
            GlobalFun::showMessageBox(3, "Please load file first !");
            return;
        }
    });

    connect(ui->phaseCalc_minus, &QAbstractButton::clicked, [&](){
        if( !m_calc_base.empty() ) {
            calc_type = 3;
            loadOneAsc();
        } else {
            GlobalFun::showMessageBox(3, "Please load file first !");
            return;
        }
    });

    connect(ui->phaseCalc_multiply, &QAbstractButton::clicked, [&](){
        if( !m_calc_base.empty() ) {
            float scaleNum = ui->multiplicand->text().toFloat();
            m_phase = BaseFunc::calculator_mul(m_calc_base, scaleNum);
            m_calc_base = m_phase.clone();

            isSirius = false;
            startCalculation();
        } else {
            GlobalFun::showMessageBox(3, "Please load file first !");
            return;
        }
    });

    connect(ui->phaseCalc_rotate, &QAbstractButton::clicked, [&](){
        if( !m_calc_base.empty() ) {
            float angle = ui->rotate_degree->text().toFloat();

            if ( m_mask.empty() ) {
                GlobalFun::showMessageBox(3, "The mask is empty !");
                return;
            } else if ( m_mask.rows != m_calc_base.rows || m_mask.cols != m_calc_base.cols ) {
                GlobalFun::showMessageBox(3, "The mask size is inconsistent with the image size !");
                return;
            }

            m_phase = BaseFunc::calculator_rotate(m_calc_base, m_mask, angle);
            m_phase.setTo(nan(""), m_phase == 0);
            m_calc_base = m_phase.clone();

            isSirius = false;
            startCalculation();
        } else {
            GlobalFun::showMessageBox(3, "Please load file first !");
            return;
        }
    });
}

void MainWindow::initGraphicsScene()
{
    qDebug() << "initGraphicsScene !";
    m_scene = new BQGraphicsScene(this);
    ui->graphicsView->setScene(m_scene);
    m_scene->setPixmapSize(680, 510);

    m_scene->connectCirclePushButton(ui->circleBtn);
    m_scene->connectEllipsePushButton(ui->ellipseBtn);
    m_scene->connectConCirclePushButton(ui->conCircleBtn);
    m_scene->connectRectanglePushButton(ui->rectangleBtn);
    m_scene->connectSquarePushButton(ui->squareBtn);
    m_scene->connectPolygonPushButton(ui->polygonBtn);
    m_scene->connectPillPushButton(ui->pillBtn);
    m_scene->connectChamferPushButton(ui->chamferBtn);
    m_scene->connectClearPushButton(ui->clearBtn);
    m_scene->connectSavePushButton(ui->saveBtn);
    m_scene->connectLoadPushButton(ui->loadBtn);
    m_scene->connectChangePushButton(ui->changeBtn);
    m_scene->initStatus();

    connect(m_scene, &BQGraphicsScene::changeMode, [&](){
        ui->mode->setText( GlobalValue::com_tp == 1 ? GlobalString::graphics_manual : GlobalString::graphics_auto );
    });
}

//******************************************************************************************************************
// [6] 事件

bool MainWindow::event(QEvent *event)
{
    if ( event->type() == QEvent::WindowStateChange ) {
        if( this->windowState() == Qt::WindowMaximized ) {
            updateView(true);
        } else if ( this->windowState() == Qt::WindowNoState ) {
            updateView(false);
        }
    }

    if ( event->type() == QEvent::Resize ) {
        updateView(true);
    }

    return QMainWindow::event(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveConfig();
    hideConfig(GlobalFun::getCurrentPath() + "/config.ini");
    hideConfig(GlobalFun::getCurrentPath() + "/parameter.ini");

    m_scene->saveItemToConfig(GlobalFun::getCurrentPath() + "/item.ini");
    hideConfig(GlobalFun::getCurrentPath() + "/item.ini");

    m_log.write(GlobalFun::getCurrentTime(2) + " - application power off");
    m_log.write("\n****************************************************************************\n");
    m_log.closeFile();
    event->accept();
}

//******************************************************************************************************************
// [1] 基础操作（配置文件、动态界面、动态语言）

void MainWindow::createDefaultConfig()
{
    QString config = "config.ini";
    if ( !GlobalFun::isFileExist(config) ) {
        QSettings settings(config, QSettings::IniFormat);
        settings.setValue(GlobalFun::BCryptographicHash("common/type"), 1);                         // 掩膜方式 1-手动 2-自动
        settings.setValue(GlobalFun::BCryptographicHash("common/pic_scale"), 1);                    // 图片缩放比例
        settings.setValue(GlobalFun::BCryptographicHash("common/unit"), 0);                         // 单位 0-λ 1-fr 2-nm 3-μm 4-mm
        settings.setValue(GlobalFun::BCryptographicHash("common/value"), 632.8);                    // 1个λ对应多少nm
        settings.setValue(GlobalFun::BCryptographicHash("common/decimal"), 3);                      // 小数点位数
        settings.setValue(GlobalFun::BCryptographicHash("common/fontPixelSize"), 12);               // 字体像素大小
        settings.setValue(GlobalFun::BCryptographicHash("common/residuesNumThresh"), 350);          // 残差点个数阈值

        settings.setValue(GlobalFun::BCryptographicHash("analysis/row"), 10);                       // 切割分析 - 行数
        settings.setValue(GlobalFun::BCryptographicHash("analysis/col"), 10);                       // 切割分析 - 列数
        settings.setValue(GlobalFun::BCryptographicHash("analysis/percentage"), 5);                 // 切割分析 - 百分比

        settings.setValue(GlobalFun::BCryptographicHash("language/type"), 1);                       // 语言类型

        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/default_size"), 3);           // 图形默认大小，单位 mm
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/pixel"), 62.6);               // 像素标定值
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/circle_radius"), 187.8);      // 圆-半径
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/ellipse_width"), 250.4);      // 椭圆-宽度
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/ellipse_height"), 187.8);     // 椭圆-高度
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/conCircle_radius_1"), 187.8); // 同心圆-内圆半径
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/conCircle_radius_2"), 313);   // 同心圆-外圆半径
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/rectangle_width"), 250.4);    // 矩形-宽度
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/rectangle_height"), 187.8);   // 矩形-高度
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/square_length"), 187.8);      // 正方形-边长
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/pill_width"), 375.6);         // 圆端矩形-宽度
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/pill_height"), 93.9);         // 圆端矩形-高度
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/chamfer_width"), 250.4);      // 圆角矩形-宽度
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/chamfer_height"), 187.8);     // 圆角矩形-高度
        settings.setValue(GlobalFun::BCryptographicHash("graphicsItem/chamfer_radius"), 31.3);      // 圆角矩形-倒角半径

        settings.setValue(GlobalFun::BCryptographicHash("zernike/piston"), 1);                      // 偏移量
        settings.setValue(GlobalFun::BCryptographicHash("zernike/tilt"), 1);                        // 倾斜量
        settings.setValue(GlobalFun::BCryptographicHash("zernike/power"), 0);                       // 离焦
        settings.setValue(GlobalFun::BCryptographicHash("zernike/ast"), 0);                         // 像散
        settings.setValue(GlobalFun::BCryptographicHash("zernike/coma"), 0);                        // 慧差
        settings.setValue(GlobalFun::BCryptographicHash("zernike/spherical"), 0);                   // 球差

        settings.setValue(GlobalFun::BCryptographicHash("file/load_path"), "null");                 // 加载文件的路径
        settings.setValue(GlobalFun::BCryptographicHash("file/save_path"), "null");                 // 存放文件的路径
        settings.setValue(GlobalFun::BCryptographicHash("file/pdf_path"), "null");                  // 存放 PDF 的路径
        settings.setValue(GlobalFun::BCryptographicHash("file/product_name"), "null");              // 产品名称
        settings.setValue(GlobalFun::BCryptographicHash("file/product_id"), "null");                // 产品ID
        settings.setValue(GlobalFun::BCryptographicHash("file/quality_tester"), "null");            // 测试人员名称
        settings.setValue(GlobalFun::BCryptographicHash("file/device_id"), "");                     // 设备ID
        settings.setValue(GlobalFun::BCryptographicHash("file/device_name"), "");                   // 设备名称
        settings.setValue(GlobalFun::BCryptographicHash("file/company_name"), "null");              // 公司名称
    }

    QString parameter = "parameter.ini";
    if ( !GlobalFun::isFileExist(parameter) ) {
        QSettings settings(parameter, QSettings::IniFormat);

        settings.setValue(GlobalFun::BCryptographicHash("parameter/psi"), 5);                       // 几步移相 0-4A 1-5AH 2-5BCS 3-9ACS 4-9BCS 5-AIA
        settings.setValue(GlobalFun::BCryptographicHash("parameter/unwrap"), 0);                    // 解包裹 0-枝切法 1-质量图 2-跨区枝切法
        settings.setValue(GlobalFun::BCryptographicHash("parameter/zernikeTerm"), 37);              // zernike显示数量
        settings.setValue(GlobalFun::BCryptographicHash("parameter/intf_scale_factor"), 0.5);       // 比例因子，内标度系数
        settings.setValue(GlobalFun::BCryptographicHash("parameter/test_wavelength"), 632.8);       // 测试波长
        settings.setValue(GlobalFun::BCryptographicHash("parameter/iso_wavelength"), 546);          // ISO波长
        settings.setValue(GlobalFun::BCryptographicHash("parameter/highPass_filterCoef"), 0.005);   // 计算高通滤波半径的系数

        settings.setValue(GlobalFun::BCryptographicHash("parameter/filter"), 0);                    // 滤波 0-none 1-LOW_PASS 2-HIGH_PASS 3-BAND_PASS 4-BAND_REJECT
        settings.setValue(GlobalFun::BCryptographicHash("parameter/fws"), 5);                       // filter windows size
        settings.setValue(GlobalFun::BCryptographicHash("parameter/remove_spikes"), 1);             // 去毛刺
        settings.setValue(GlobalFun::BCryptographicHash("parameter/slopeRemoveSize"), 2);           // 去毛刺阈值像素数
        settings.setValue(GlobalFun::BCryptographicHash("parameter/slopeThresh"), 4);               // 去毛刺阈值
        settings.setValue(GlobalFun::BCryptographicHash("parameter/remove_residual"), 0);           // 去残差
        settings.setValue(GlobalFun::BCryptographicHash("parameter/refractive_index"), 1.5);        // 折射率，计算ttv时要用

        settings.beginWriteArray("algorithmList");
        settings.setArrayIndex(0);
        for ( int i = 0; i < m_algParameter.size(); ++i )
        {
            if ( i == 0 || i == 1 || i == 13 || i == 41 ) {
                settings.setValue(m_algParameter.at(i), 1);
            } else {
                settings.setValue(m_algParameter.at(i), 0);
            }
        }
        settings.endArray();

        settings.beginWriteArray("limitList");
        settings.setArrayIndex(0);
        for ( int i = 0; i < m_algParameterLimit.size(); ++i )
        {
            if ( i % 2 == 0 ) {
                settings.setValue(m_algParameterLimit.at(i), 0);
            } else {
                settings.setValue(m_algParameterLimit.at(i), 1);
            }
        }
        settings.endArray();
    }

    hideConfig(GlobalFun::getCurrentPath() + "/" + config);
    hideConfig(GlobalFun::getCurrentPath() + "/" + parameter);
}

void MainWindow::loadConfig()
{
    QSettings config("config.ini", QSettings::IniFormat);

    GlobalValue::com_tp = config.value(GlobalFun::BCryptographicHash("common/type")).toInt();
    GlobalValue::com_p_sle = config.value(GlobalFun::BCryptographicHash("common/pic_scale")).toDouble();
    GlobalValue::com_unit = config.value(GlobalFun::BCryptographicHash("common/unit")).toInt();
    GlobalValue::com_value = config.value(GlobalFun::BCryptographicHash("common/value")).toDouble();
    GlobalValue::com_dcl = config.value(GlobalFun::BCryptographicHash("common/decimal")).toInt();
    GlobalValue::com_fps = config.value(GlobalFun::BCryptographicHash("common/fontPixelSize")).toInt();
    GlobalValue::com_rnt = config.value(GlobalFun::BCryptographicHash("common/residuesNumThresh")).toInt();

    GlobalValue::ana_row = config.value(GlobalFun::BCryptographicHash("analysis/row")).toInt();
    GlobalValue::ana_col = config.value(GlobalFun::BCryptographicHash("analysis/col")).toInt();
    GlobalValue::ana_per = config.value(GlobalFun::BCryptographicHash("analysis/percentage")).toInt();

    GlobalValue::lgn_tp = config.value(GlobalFun::BCryptographicHash("language/type")).toInt();

    GlobalValue::gra_def_size = config.value(GlobalFun::BCryptographicHash("graphicsItem/default_size")).toInt();
    GlobalValue::gra_pixel = config.value(GlobalFun::BCryptographicHash("graphicsItem/pixel")).toInt();
    GlobalValue::gra_c_rad = config.value(GlobalFun::BCryptographicHash("graphicsItem/circle_radius")).toDouble();
    GlobalValue::gra_e_wid = config.value(GlobalFun::BCryptographicHash("graphicsItem/ellipse_width")).toDouble();
    GlobalValue::gra_e_hei = config.value(GlobalFun::BCryptographicHash("graphicsItem/ellipse_height")).toDouble();
    GlobalValue::gra_cc_rad_1 = config.value(GlobalFun::BCryptographicHash("graphicsItem/conCircle_radius_1")).toDouble();
    GlobalValue::gra_cc_rad_2 = config.value(GlobalFun::BCryptographicHash("graphicsItem/conCircle_radius_2")).toDouble();
    GlobalValue::gra_r_wid = config.value(GlobalFun::BCryptographicHash("graphicsItem/rectangle_width")).toDouble();
    GlobalValue::gra_r_hei = config.value(GlobalFun::BCryptographicHash("graphicsItem/rectangle_height")).toDouble();
    GlobalValue::gra_s_len = config.value(GlobalFun::BCryptographicHash("graphicsItem/square_length")).toDouble();
    GlobalValue::gra_p_wid = config.value(GlobalFun::BCryptographicHash("graphicsItem/pill_width")).toDouble();
    GlobalValue::gra_p_hei = config.value(GlobalFun::BCryptographicHash("graphicsItem/pill_height")).toDouble();
    GlobalValue::gra_ch_wid = config.value(GlobalFun::BCryptographicHash("graphicsItem/chamfer_width")).toDouble();
    GlobalValue::gra_ch_hei = config.value(GlobalFun::BCryptographicHash("graphicsItem/chamfer_height")).toDouble();
    GlobalValue::gra_ch_rad = config.value(GlobalFun::BCryptographicHash("graphicsItem/chamfer_radius")).toDouble();

    GlobalValue::zer_pis = config.value(GlobalFun::BCryptographicHash("zernike/piston")).toInt();
    GlobalValue::zer_tilt = config.value(GlobalFun::BCryptographicHash("zernike/tilt")).toInt();
    GlobalValue::zer_pow = config.value(GlobalFun::BCryptographicHash("zernike/power")).toInt();
    GlobalValue::zer_ast = config.value(GlobalFun::BCryptographicHash("zernike/ast")).toInt();
    GlobalValue::zer_coma = config.value(GlobalFun::BCryptographicHash("zernike/coma")).toInt();
    GlobalValue::zer_sph = config.value(GlobalFun::BCryptographicHash("zernike/spherical")).toInt();

    GlobalValue::file_load_path = config.value(GlobalFun::BCryptographicHash("file/load_path")).toString();
    GlobalValue::file_save_path = config.value(GlobalFun::BCryptographicHash("file/save_path")).toString();
    GlobalValue::file_pdf_path = config.value(GlobalFun::BCryptographicHash("file/pdf_path")).toString();
    GlobalValue::file_pdt_name = config.value(GlobalFun::BCryptographicHash("file/product_name")).toString();
    GlobalValue::file_pdt_id = config.value(GlobalFun::BCryptographicHash("file/product_id")).toString();
    GlobalValue::file_qua_tester = config.value(GlobalFun::BCryptographicHash("file/quality_tester")).toString();
    GlobalValue::file_device_id = config.value(GlobalFun::BCryptographicHash("file/device_id")).toString();
    GlobalValue::file_device_name = config.value(GlobalFun::BCryptographicHash("file/device_name")).toString();
    GlobalValue::file_company_name = config.value(GlobalFun::BCryptographicHash("file/company_name")).toString();

    //------------------------------------------------------------------------------

    QSettings parameter("parameter.ini", QSettings::IniFormat);

    GlobalValue::par_psi = parameter.value(GlobalFun::BCryptographicHash("parameter/psi")).toInt();
    GlobalValue::par_unw = parameter.value(GlobalFun::BCryptographicHash("parameter/unwrap")).toInt();
    GlobalValue::par_ztm = parameter.value(GlobalFun::BCryptographicHash("parameter/zernikeTerm")).toInt();
    GlobalValue::par_i_s_f = parameter.value(GlobalFun::BCryptographicHash("parameter/intf_scale_factor")).toDouble();
    GlobalValue::par_t_w = parameter.value(GlobalFun::BCryptographicHash("parameter/test_wavelength")).toDouble();
    GlobalValue::par_i_w = parameter.value(GlobalFun::BCryptographicHash("parameter/iso_wavelength")).toDouble();
    GlobalValue::par_hp_fc = parameter.value(GlobalFun::BCryptographicHash("parameter/highPass_filterCoef")).toDouble();

    GlobalValue::par_flt = parameter.value(GlobalFun::BCryptographicHash("parameter/filter")).toInt();
    GlobalValue::par_fws = parameter.value(GlobalFun::BCryptographicHash("parameter/fws")).toInt();
    GlobalValue::par_rm_spk = parameter.value(GlobalFun::BCryptographicHash("parameter/remove_spikes")).toInt();
    GlobalValue::par_srs = parameter.value(GlobalFun::BCryptographicHash("parameter/slopeRemoveSize")).toInt();
    GlobalValue::par_sth = parameter.value(GlobalFun::BCryptographicHash("parameter/slopeThresh")).toInt();
    GlobalValue::par_rm_rsl = parameter.value(GlobalFun::BCryptographicHash("parameter/remove_residual")).toInt();
    GlobalValue::par_ref_index = parameter.value(GlobalFun::BCryptographicHash("parameter/refractive_index")).toDouble();

    //------------------------------------------------------------------------------

    parameter.beginReadArray("algorithmList");
    parameter.setArrayIndex(0);
    for ( int i = 0; i < m_algParameter.size(); ++i )
    {
        m_stateList.insert( m_algParameter.at(i), parameter.value(m_algParameter.at(i)).toInt() );
    }
    parameter.endArray();

    parameter.beginReadArray("limitList");
    parameter.setArrayIndex(0);
    for ( int i = 0; i < m_algParameterLimit.size(); ++i )
    {
        m_limitList.insert( m_algParameterLimit.at(i), parameter.value(m_algParameterLimit.at(i)).toDouble() );
    }
    parameter.endArray();
}

void MainWindow::saveConfig()
{
    QSettings config("config.ini", QSettings::IniFormat);

    config.setValue(GlobalFun::BCryptographicHash("common/type"), GlobalValue::com_tp);
    config.setValue(GlobalFun::BCryptographicHash("common/pic_scale"), GlobalValue::com_p_sle);
    config.setValue(GlobalFun::BCryptographicHash("common/unit"), GlobalValue::com_unit);
    config.setValue(GlobalFun::BCryptographicHash("common/value"), GlobalValue::com_value);
    config.setValue(GlobalFun::BCryptographicHash("common/decimal"), GlobalValue::com_dcl);
    config.setValue(GlobalFun::BCryptographicHash("common/fontPixelSize"), GlobalValue::com_fps);
    config.setValue(GlobalFun::BCryptographicHash("common/residuesNumThresh"), GlobalValue::com_rnt);

    config.setValue(GlobalFun::BCryptographicHash("analysis/row"), GlobalValue::ana_row);
    config.setValue(GlobalFun::BCryptographicHash("analysis/col"), GlobalValue::ana_col);
    config.setValue(GlobalFun::BCryptographicHash("analysis/percentage"), GlobalValue::ana_per);

    config.setValue(GlobalFun::BCryptographicHash("language/type"), GlobalValue::lgn_tp);

    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/default_size"), GlobalValue::gra_def_size);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/pixel"), GlobalValue::gra_pixel);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/circle_radius"), GlobalValue::gra_c_rad);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/ellipse_width"), GlobalValue::gra_e_wid);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/ellipse_height"), GlobalValue::gra_e_hei);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/conCircle_radius_1"), GlobalValue::gra_cc_rad_1);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/conCircle_radius_2"), GlobalValue::gra_cc_rad_2);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/rectangle_width"), GlobalValue::gra_r_wid);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/rectangle_height"), GlobalValue::gra_r_hei);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/square_length"), GlobalValue::gra_s_len);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/pill_width"), GlobalValue::gra_p_wid);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/pill_height"), GlobalValue::gra_p_hei);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/chamfer_width"), GlobalValue::gra_ch_wid);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/chamfer_height"), GlobalValue::gra_ch_hei);
    config.setValue(GlobalFun::BCryptographicHash("graphicsItem/chamfer_radius"), GlobalValue::gra_ch_rad);

    config.setValue(GlobalFun::BCryptographicHash("zernike/piston"), GlobalValue::zer_pis);
    config.setValue(GlobalFun::BCryptographicHash("zernike/tilt"), GlobalValue::zer_tilt);
    config.setValue(GlobalFun::BCryptographicHash("zernike/power"), GlobalValue::zer_pow);
    config.setValue(GlobalFun::BCryptographicHash("zernike/ast"), GlobalValue::zer_ast);
    config.setValue(GlobalFun::BCryptographicHash("zernike/coma"), GlobalValue::zer_coma);
    config.setValue(GlobalFun::BCryptographicHash("zernike/spherical"), GlobalValue::zer_sph);

    config.setValue(GlobalFun::BCryptographicHash("file/load_path"), GlobalValue::file_load_path);
    config.setValue(GlobalFun::BCryptographicHash("file/save_path"), GlobalValue::file_save_path);
    config.setValue(GlobalFun::BCryptographicHash("file/pdf_path"), GlobalValue::file_pdf_path);
    config.setValue(GlobalFun::BCryptographicHash("file/product_name"), GlobalValue::file_pdt_name);
    config.setValue(GlobalFun::BCryptographicHash("file/product_id"), GlobalValue::file_pdt_id);
    config.setValue(GlobalFun::BCryptographicHash("file/quality_tester"), GlobalValue::file_qua_tester);
    config.setValue(GlobalFun::BCryptographicHash("file/device_id"), GlobalValue::file_device_id);
    config.setValue(GlobalFun::BCryptographicHash("file/device_name"), GlobalValue::file_device_name);
    config.setValue(GlobalFun::BCryptographicHash("file/company_name"), GlobalValue::file_company_name);

    //------------------------------------------------------------------------------

    QSettings parameter("parameter.ini", QSettings::IniFormat);

    parameter.setValue(GlobalFun::BCryptographicHash("parameter/psi"), GlobalValue::par_psi);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/unwrap"), GlobalValue::par_unw);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/zernikeTerm"), GlobalValue::par_ztm);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/intf_scale_factor"), GlobalValue::par_i_s_f);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/test_wavelength"), GlobalValue::par_t_w);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/iso_wavelength"), GlobalValue::par_i_w);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/highPass_filterCoef"), GlobalValue::par_hp_fc);

    parameter.setValue(GlobalFun::BCryptographicHash("parameter/filter"), GlobalValue::par_flt);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/fws"), GlobalValue::par_fws);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/remove_spikes"), GlobalValue::par_rm_spk);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/slopeRemoveSize"), GlobalValue::par_srs);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/slopeThresh"), GlobalValue::par_sth);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/remove_residual"), GlobalValue::par_rm_rsl);
    parameter.setValue(GlobalFun::BCryptographicHash("parameter/refractive_index"), GlobalValue::par_ref_index);

    //------------------------------------------------------------------------------

    parameter.beginWriteArray("algorithmList");
    parameter.setArrayIndex(0);
    for ( int i = 0; i < m_stateList.size(); ++i )
    {
        auto temp = m_stateList.begin() + i;
        parameter.setValue(temp.key(), temp.value());
    }
    parameter.endArray();

    parameter.beginWriteArray("limitList");
    parameter.setArrayIndex(0);
    for ( int i = 0; i < m_limitList.size(); ++i )
    {
        auto temp = m_limitList.begin() + i;
        parameter.setValue(temp.key(), temp.value());
    }
    parameter.endArray();
}

void MainWindow::updateView(bool state)
{
    if (state) {
        ui->graphicsView->setGeometry(120, 10, 700*(this->width()-400)/1200, 530*(this->height()-90)/810);
        ui->scrollArea->setGeometry(10, 10, 100, ui->graphicsView->height());
        ui->view->setGeometry(130 + ui->graphicsView->width(), 10,
                              this->width() - 400 - ui->graphicsView->width(), ui->graphicsView->height());
        ui->result->setGeometry(140 + ui->graphicsView->width() + ui->view->width(), 10,
                                250, ui->graphicsView->height());
        ui->data->setGeometry(10, ui->graphicsView->height() + 20, ui->graphicsView->width() + 110, 280*(this->height()-90)/810);
        ui->chart->setGeometry(ui->view->x(), ui->data->y(), ui->view->width(), ui->data->height());
        ui->automation->setGeometry(ui->result->x(), ui->data->y(), ui->result->width(), ui->data->height());
    } else {
        this->resize(1600, 900);
        ui->scrollArea->setGeometry(10, 10, 100, 530);
        ui->graphicsView->setGeometry(120, 10, 700, 530);
        ui->view->setGeometry(830, 10, 500, 530);
        ui->result->setGeometry(1340, 10, 250, 530);
        ui->data->setGeometry(10, 550, 810, 280);
        ui->chart->setGeometry(830, 550, 500, 280);
        ui->automation->setGeometry(1340, 550, 250, 280);
    }

    modifyScale();

    ui->result_table->resize(ui->result->width() - 6, ui->result->height() - 24);
    ui->zernike_table->resize(ui->result->width() - 6, ui->result->height() - 24);
    ui->data_table->resize(ui->data->width() - 6, ui->data->height() - 24);

    QCustomPlot *customPlot = static_cast<QCustomPlot *>( ui->view_tab1->children().at(0) );
    if ( ui->view->width() >= ui->view->height() ) {
        customPlot->setFixedSize(ui->view->height(), ui->view->height() - 40);
    } else {
        customPlot->setFixedSize(ui->view->width() - 10, ui->view->width() - 50);
    }

    ui->view_3d->resize(ui->view->width() - 6, ui->view->height() - 24);
    container->setGeometry(0, 0, ui->view_3d->width(), ui->view_3d->height());

    ui->line_chart->resize(ui->chart->width(), ui->chart->height() - 20);
    ui->psdXLineChart->resize(ui->chart->width(), ui->chart->height() - 20);

    ui->psf_2d->setGeometry(0, 0, ui->chart->width()/2, ui->chart->height() - 24);
    ui->psf_image->setGeometry(10, 10, ui->psf_2d->width() - 20, ui->psf_2d->height() - 20);
    ui->psf_3d->setGeometry(ui->chart->width()/2, 0, ui->chart->width()/2, ui->chart->height() - 24);
    psf_container->setGeometry(0, 0, ui->psf_3d->width(), ui->psf_3d->height());

    // 质检分类
    ui->quality_table->resize(ui->automation->width() - 6, ui->automation->height() - 24);
    ui->quality_table->setColumnWidth(0, ui->quality_table->width());
    ui->quality_table->setRowHeight(0, ui->quality_table->height());
    ui->quality_table->item(0, 0)->setTextAlignment(Qt::AlignCenter);

    // 相位计算
    qreal phase_height = (ui->quality_table->height() - 120) / 5;
    ui->phaseCalc_load->setGeometry(20, phase_height, 90, 30);
    ui->phaseCalc_inverse->setGeometry(140, phase_height, 90, 30);
    ui->phaseCalc_add->setGeometry(20, 30 + phase_height*2, 90, 30);
    ui->phaseCalc_minus->setGeometry(140, 30 + phase_height*2, 90, 30);
    ui->multiplicand->setGeometry(20, 60 + phase_height*3, 90, 30);
    ui->phaseCalc_multiply->setGeometry(140, 60 + phase_height*3, 90, 30);
    ui->rotate_degree->setGeometry(20, 90 + phase_height*4, 90, 30);
    ui->phaseCalc_rotate->setGeometry(140, 90 + phase_height*4, 90, 30);

    // psd label
    label_x->setGeometry(psd_chart->x() + 10, ui->psdXLineChart->y() + ui->psdXLineChart->height() - 50, 80, 20);
    label_y->setGeometry(psd_chart->x() + 10, ui->psdXLineChart->y() + ui->psdXLineChart->height() - 30, 80, 20);
}

void MainWindow::modifyScale()
{
    int picWidth = m_scene->getUnlivePixmapItem().pixmap().width();
    int picHeight = m_scene->getUnlivePixmapItem().pixmap().height();
    int sceneWidth = ui->graphicsView->width() - 20;
    int sceneHeight = ui->graphicsView->height() - 20;

    double w_sceneScale = floor(picWidth/(sceneWidth*1.0f) * 100.000f) / 100.000f;
    double h_sceneScale = floor(picHeight/(sceneHeight*1.0f) * 100.000f) / 100.000f;
    double sceneScale = w_sceneScale >= h_sceneScale ? w_sceneScale : h_sceneScale;

    double w_viewScale = floor(sceneWidth/(picWidth*1.0f) * 100.000f) / 100.000f;
    double h_viewScale = floor(sceneHeight/(picHeight*1.0f) * 100.000f) / 100.000f;
    double viewScale = w_viewScale <= h_viewScale ? w_viewScale : h_viewScale;

    if ( picWidth == 680 && picHeight == 510 ) {
        sceneScale = viewScale = 1;
    }

//    qDebug() << "---------------------------------------";
//    qDebug() << "picWidth: " << picWidth;
//    qDebug() << "picHeight: " << picHeight;
//    qDebug() << "sceneWidth: " << sceneWidth;
//    qDebug() << "sceneHeight: " << sceneHeight;
//    qDebug() << "sceneScale: " << sceneScale;
//    qDebug() << "viewScale: " << viewScale;
//    qDebug() << "---------------------------------------" << endl;

    GlobalValue::com_p_sle = viewScale;

    ui->graphicsView->setSceneRect(-sceneWidth/2 * sceneScale, -sceneHeight/2 * sceneScale,
                                   sceneWidth * sceneScale, sceneHeight * sceneScale);

    ui->graphicsView->resetTransform();
    ui->graphicsView->scale(viewScale, viewScale);
}

void MainWindow::changeLanguage(int type)
{
    QFont font;
    font.setPixelSize(GlobalValue::com_fps);

    ui->action_load->setFont(font);
    ui->action_save->setFont(font);
    ui->action_screenshot->setFont(font);
    ui->action_export_report->setFont(font);
    ui->action_chinese->setFont(font);
    ui->action_english->setFont(font);

    //------------------------------------------------------------------------------

    QString source = "";

    switch (type)
    {
    case 1: {
        source = ":/source/strings-C.json";

        ui->view->setTabText(0, QStringLiteral("二维图形"));
        ui->view->setTabText(1, QStringLiteral("三维图形"));
        ui->result->setTabText(0, QStringLiteral("检测结果"));
        ui->result->setTabText(1, QStringLiteral("Zernike"));
        ui->data->setTabText(0, QStringLiteral("数据保存"));
        ui->chart->setTabText(0, QStringLiteral("XY轴"));
        ui->chart->setTabText(1, QStringLiteral("PSF"));
        ui->automation->setTabText(0, QStringLiteral("质检分类"));
        ui->automation->setTabText(1, QStringLiteral("相位计算"));

        ui->action_analysis->setToolTip(QStringLiteral("分析"));
        ui->action_sliceAnalysis->setToolTip(QStringLiteral("切割分析"));
        ui->action_setting->setToolTip(QStringLiteral("设置"));
        ui->action_Company->setToolTip(QStringLiteral("单位"));
    } break;
    case 2: {
        source = ":/source/strings-E.json";

        ui->view->setTabText(0, QStringLiteral("2D"));
        ui->view->setTabText(1, QStringLiteral("3D"));
        ui->result->setTabText(0, QStringLiteral("Result"));
        ui->result->setTabText(1, QStringLiteral("Zernike"));
        ui->data->setTabText(0, QStringLiteral("Data"));
        ui->chart->setTabText(0, QStringLiteral("XY-Axis"));
        ui->chart->setTabText(1, QStringLiteral("PSF"));
        ui->automation->setTabText(0, QStringLiteral("Quality"));
        ui->automation->setTabText(1, QStringLiteral("Phase"));

        ui->action_analysis->setToolTip(QStringLiteral("Analysis"));
        ui->action_sliceAnalysis->setToolTip(QStringLiteral("Slice Analysis"));
        ui->action_setting->setToolTip(QStringLiteral("Setting"));
        ui->action_Company->setToolTip(QStringLiteral("Unit"));
    } break;
    default: break;
    }

    QJsonObject obj = GlobalFun::getJsonObj(source);
    GlobalString::menu_file = obj.value("menu_file").toString();
    GlobalString::menu_language = obj.value("menu_language").toString();

    GlobalString::action_load = obj.value("action_load").toString();
    GlobalString::action_save = obj.value("action_save").toString();
    GlobalString::action_screenshot = obj.value("action_screenshot").toString();
    GlobalString::action_export_report = obj.value("action_export_report").toString();
    GlobalString::action_setting = obj.value("action_setting").toString();
    GlobalString::action_chinese = obj.value("action_chinese").toString();
    GlobalString::action_english = obj.value("action_english").toString();

    GlobalString::graphics_circle = obj.value("graphics_circle").toString();
    GlobalString::graphics_ellipse = obj.value("graphics_ellipse").toString();
    GlobalString::graphics_concentric_circle = obj.value("graphics_concentric_circle").toString();
    GlobalString::graphics_rectangle = obj.value("graphics_rectangle").toString();
    GlobalString::graphics_square = obj.value("graphics_square").toString();
    GlobalString::graphics_polygon = obj.value("graphics_polygon").toString();
    GlobalString::graphics_pill = obj.value("graphics_pill").toString();
    GlobalString::graphics_chamfer = obj.value("graphics_chamfer").toString();
    GlobalString::graphics_auto_recognise = obj.value("graphics_auto_recognise").toString();

    GlobalString::graphics_auto_circle = obj.value("graphics_auto_circle").toString();
    GlobalString::graphics_auto_ellipse = obj.value("graphics_auto_ellipse").toString();
    GlobalString::graphics_auto_pill = obj.value("graphics_auto_pill").toString();
    GlobalString::graphics_auto_chamfer = obj.value("graphics_auto_chamfer").toString();
    GlobalString::graphics_auto_roundEdgeRec = obj.value("graphics_auto_roundEdgeRec").toString();
    GlobalString::graphics_auto_rotateRec = obj.value("graphics_auto_rotateRec").toString();

    GlobalString::graphics_clear = obj.value("graphics_clear").toString();
    GlobalString::graphics_change = obj.value("graphics_change").toString();
    GlobalString::graphics_manual = obj.value("graphics_manual").toString();
    GlobalString::graphics_auto = obj.value("graphics_auto").toString();

    GlobalString::contextMenu_clear = obj.value("contextMenu_clear").toString();
    GlobalString::contextMenu_delete = obj.value("contextMenu_delete").toString();
    GlobalString::contextMenu_export = obj.value("contextMenu_export").toString();
    GlobalString::contextMenu_show = obj.value("contextMenu_show").toString();
    GlobalString::contextMenu_sure = obj.value("contextMenu_sure").toString();
    GlobalString::contextMenu_unit = obj.value("contextMenu_unit").toString();
    GlobalString::contextMenu_fitting = obj.value("contextMenu_fitting").toString();
    GlobalString::contextMenu_residual = obj.value("contextMenu_residual").toString();
    GlobalString::contextMenu_fillSpikes = obj.value("contextMenu_fillSpikes").toString();

    GlobalString::phase_add = obj.value("phase_add").toString();
    GlobalString::phase_minus = obj.value("phase_minus").toString();
    GlobalString::phase_rotate = obj.value("phase_rotate").toString();
    GlobalString::phase_inverse = obj.value("phase_inverse").toString();
    GlobalString::phase_multiply = obj.value("phase_multiply").toString();

    ui->menu_file->setTitle(GlobalString::menu_file);
    ui->menu_language->setTitle(GlobalString::menu_language);

    ui->action_load->setText(GlobalString::action_load);
    ui->action_save->setText(GlobalString::action_save);
    ui->action_screenshot->setText(GlobalString::action_screenshot);
    ui->action_export_report->setText(GlobalString::action_export_report);
    ui->action_chinese->setText(GlobalString::action_chinese);
    ui->action_english->setText(GlobalString::action_english);

    ui->circleBtn->setText(GlobalString::graphics_circle);
    ui->ellipseBtn->setText(GlobalString::graphics_ellipse);
    ui->conCircleBtn->setText(GlobalString::graphics_concentric_circle);
    ui->rectangleBtn->setText(GlobalString::graphics_rectangle);
    ui->squareBtn->setText(GlobalString::graphics_square);
    ui->polygonBtn->setText(GlobalString::graphics_polygon);
    ui->pillBtn->setText(GlobalString::graphics_pill);
    ui->chamferBtn->setText(GlobalString::graphics_chamfer);
    ui->clearBtn->setText(GlobalString::graphics_clear);
    ui->saveBtn->setText(GlobalString::action_save);
    ui->loadBtn->setText(GlobalString::action_load);
    ui->changeBtn->setText(GlobalString::graphics_change);
    ui->mode->setText( GlobalValue::com_tp == 1 ? GlobalString::graphics_manual : GlobalString::graphics_auto );

    ui->phaseCalc_load->setText(GlobalString::action_load);
    ui->phaseCalc_add->setText(GlobalString::phase_add);
    ui->phaseCalc_minus->setText(GlobalString::phase_minus);
    ui->phaseCalc_rotate->setText(GlobalString::phase_rotate);
    ui->phaseCalc_multiply->setText(GlobalString::phase_multiply);
    ui->phaseCalc_inverse->setText(GlobalString::phase_inverse);

    GlobalValue::lgn_tp = type;
}

void MainWindow::changeStatus(bool status)
{
    ui->menu_file->setEnabled(status);
    ui->menu_language->setEnabled(status);
    ui->action_analysis->setEnabled(status);
    ui->action_sliceAnalysis->setEnabled(status);

    ui->graphicsView->setEnabled(status);
    ui->result_table->setEnabled(status);
    ui->zernike_table->setEnabled(status);
    ui->data_table->setEnabled(status);
    ui->quality_table->setEnabled(status);
    ui->view_tab1->setEnabled(status);
}

void MainWindow::hideConfig(QString path)
{
    SetFileAttributes((LPCWSTR)path.unicode(), FILE_ATTRIBUTE_HIDDEN);
}

void MainWindow::updateGraphBtn(bool type)
{
    ui->circleBtn->setEnabled(type);
    ui->clearBtn->setEnabled(type);
    ui->conCircleBtn->setEnabled(type);
    ui->ellipseBtn->setEnabled(type);
    ui->squareBtn->setEnabled(type);
    ui->saveBtn->setEnabled(type);
    ui->chamferBtn->setEnabled(type);
    ui->changeBtn->setEnabled(type);
    ui->loadBtn->setEnabled(type);
    ui->mode->setEnabled(type);
    ui->pillBtn->setEnabled(type);
    ui->polygonBtn->setEnabled(type);
    ui->rectangleBtn->setEnabled(type);

    ui->action_load->setEnabled(type);
    ui->action_save->setEnabled(type);
    ui->action_export_report->setEnabled(type);
    ui->action_analysis->setEnabled(type);
}

void MainWindow::updateGraphItemSize(qreal m_pix)
{
    if ( m_pix > 0 ) {
        GlobalValue::gra_pixel = 1/(m_pix * 1000);
        qDebug() << "GlobalValue::gra_pixel: " << GlobalValue::gra_pixel;
    }

    if ( GlobalValue::gra_pixel == 0 ) {
        return;
    }

    GlobalValue::gra_c_rad = GlobalValue::gra_pixel * GlobalValue::gra_def_size;
    GlobalValue::gra_e_wid = GlobalValue::gra_pixel * GlobalValue::gra_def_size /3*4;
    GlobalValue::gra_e_hei = GlobalValue::gra_pixel * GlobalValue::gra_def_size;
    GlobalValue::gra_cc_rad_1 = GlobalValue::gra_pixel * GlobalValue::gra_def_size;
    GlobalValue::gra_cc_rad_2 = GlobalValue::gra_pixel * GlobalValue::gra_def_size /3*5;
    GlobalValue::gra_r_wid = GlobalValue::gra_pixel * GlobalValue::gra_def_size /3*4;
    GlobalValue::gra_r_hei = GlobalValue::gra_pixel * GlobalValue::gra_def_size;
    GlobalValue::gra_s_len = GlobalValue::gra_pixel * GlobalValue::gra_def_size;
    GlobalValue::gra_p_wid = GlobalValue::gra_pixel * GlobalValue::gra_def_size *2;
    GlobalValue::gra_p_hei = GlobalValue::gra_pixel * GlobalValue::gra_def_size /2;
    GlobalValue::gra_ch_wid = GlobalValue::gra_pixel * GlobalValue::gra_def_size /3*4;
    GlobalValue::gra_ch_hei = GlobalValue::gra_pixel * GlobalValue::gra_def_size;
    GlobalValue::gra_ch_rad = GlobalValue::gra_pixel * GlobalValue::gra_def_size /6;
}

//******************************************************************************************************************
// [2] 文件加载、保存 + 导出报表

void MainWindow::loadResource()
{
    // 判断加载路径是否为空
    QString path = GlobalFun::getCurrentPath();
    if ( GlobalValue::file_load_path != "null" ) { path = GlobalValue::file_load_path; }

    QStringList list = QFileDialog::getOpenFileNames(nullptr, GlobalString::action_load, path,
                                                     "All Files (*.asc *.xyz *.sirius);;"
                                                     "ASC Files (*.asc);; XYZ Files (*.xyz);; Sirius File (*.sirius)");

    if ( list.size() == 0 ) {
        return;
    }

    if ( list.size() > 1 ) {
        GlobalFun::showMessageBox(3, "Only one file can be loaded !");
        return;
    }

    // 保存加载路径
    QStringList list2 = list.at(0).split("/");
    QString str = "";
    for ( int i = 0; i < list2.size() - 1; ++i )
    {
        str += list2.at(i) + "/";
    }
    GlobalValue::file_load_path = str;
    QStringList temp = list.at(0).split(".");
    QString type = temp.at(temp.size() - 1);

    if ( type == "xyz" ) {
        loadXYZ(list.at(0));
    } else if ( type == "asc" ) {
        loadASC(list.at(0));
    } else if ( type == "sirius" ) {
        loadSirius(list.at(0));
    }

    if ( loadFileState ) {
        GlobalFun::showMessageBox(2, QString("Load successfully !\nFormat: %1").arg(type));
    } else {
        GlobalFun::showMessageBox(3, "Load " + type + " file error !");
    }
}

void MainWindow::saveResource()
{
    if ( m_matList.size() == 0 ) {
        GlobalFun::showMessageBox(3, "No picture to save !");
        return;
    } else if ( !GlobalFun::judVecAvailable(m_matList) ) {
        GlobalFun::showMessageBox(3, "The picture is empty !");
        return;
    } else if ( m_algorithm->errorType != ERROR_TYPE_NOT_ERROR ) {
        GlobalFun::showMessageBox(3, "The result is wrong, unable to save ASC or XYZ file !");
        return;
    }

    QString path;
    QString subPath;
    QString file;

    // 判断存放路径是否为空
    path = GlobalFun::getCurrentTime(1);

    if ( GlobalValue::file_save_path != "null" ) { path = GlobalValue::file_save_path + "/" + path; }

    file = QFileDialog::getSaveFileName(nullptr, GlobalString::action_save, path,
                                                "ASC Files (*.asc);; XYZ Files (*.xyz)");

    if ( file == "" ) {
        return;
    }

    // 保存存放路径
    QStringList list = file.split("/");
    QString str = "";
    for ( int i = 0; i < list.size() - 1; ++i )
    {
        str += list.at(i) + "/";
    }
    GlobalValue::file_save_path = str;

    QStringList temp = file.split(".");
    QString type = temp.at(temp.size() - 1);

    if ( type == "xyz" ) {
        saveXYZ(file);
    } else if ( type == "asc" ) {
        saveACS(file);
    }
}

bool MainWindow::exportReport()
{
    if ( m_matList.size() == 0 ) {
        GlobalFun::showMessageBox(3, "No picture to save !");
        return false;
    } else if ( !GlobalFun::judVecAvailable(m_matList) ) {
        GlobalFun::showMessageBox(3, "The picture is empty !");
        return false;
    }

    if ( m_algorithm->errorType != ERROR_TYPE_NOT_ERROR ) {
        GlobalFun::showMessageBox(3, "The result is wrong, unable to export report !");
        return false;
    }

    // 判断 PDF 存放路径是否为空
    QString path = GlobalFun::getCurrentTime(1);
    if ( GlobalValue::file_pdf_path != "null" ) { path = GlobalValue::file_pdf_path + "/" + path; }

    QString file = QFileDialog::getSaveFileName(nullptr, GlobalString::action_save, path, "PDF Files (*.pdf)");

    if ( file == "" ) {
        return false;
    }

    // 保存 PDF 存放路径
    QStringList list = file.split("/");
    QString str = "";
    for ( int i = 0; i < list.size() - 1; ++i )
    {
        str += list.at(i) + "/";
    }
    GlobalValue::file_pdf_path = str;

    QString product_name = "";
    QString product_id = "";
    QString quality_tester = "";
    QString comments = "";
    QString company_name = "";
    QString serial_id = "";
    int plot_type = 0;

    ExportReport_Dialog *dialog = new ExportReport_Dialog();
    if ( dialog->exec() == QDialog::Accepted ) {
        QStringList list = dialog->getData();
        product_name = list.at(0);
        product_id = list.at(1);
        quality_tester = list.at(2);
        comments = list.at(3) == "nan" ? "/" : list.at(3);
        company_name = list.at(4) == "nan" ? "" : list.at(4);
        plot_type = list.at(5).toInt();
    }
    delete dialog;

    if ( product_name == "" ) {
        GlobalFun::showMessageBox(3, "Please fill in the product information first !");
        return false;
    }

    // 标题、Logo、公司名称、波长、单位
    BPdfCreater pdf(file);
    QString title = GlobalValue::lgn_tp == 1 ? QStringLiteral("检测报告") : "Report";
    QString deviceName = GlobalValue::file_device_name == "" ? "":GlobalValue::file_device_name;
    pdf.drawTitle( deviceName + " " + title);
    pdf.drawLogo(QImage(":/images/Company_Logo.bmp"));
    pdf.drawCompanyOrUrl("http://www.shineoptics.com");
    if ( company_name == "" ) {
        pdf.drawCompanyName(QStringLiteral("上海乾曜光学科技有限公司"));
    } else {
        pdf.drawCompanyName(company_name);
    }
    pdf.drawWaveAndUnit();

    // 强度图 + 掩膜（加载 xyz 文件时是相位图）
    cv::Mat mat = m_matList.at(0).clone();

    if ( isSirius ) {
        QImage image = m_scene->getMask();
        cv::Mat mask = GlobalFun::convertQImageToMat(image);
        uchar *data = mask.data;
        for ( int i = 0; i < mask.rows * mask.cols; ++i )
        {
            if ( *data == 0 && *(data + 1) == 35 && *(data + 2) == 0 ) {
                *data = 54;
                *(data + 1) = 54;
                *(data + 2) = 54;
            }
            data += mask.type() == CV_8UC4 ? 4 : (mask.type() == CV_8UC3 ? 3 : 1);
        }

        // 映美精相机获取的照片格式为 CV_8UC4，巴斯勒和海康威视获取的照片格式为 CV_8UC3
        cv::Mat dst;
        if ( mat.type() == CV_8UC3 ) {
            cv::Mat alpha = GlobalFun::createAlpha(mat);
            GlobalFun::addAlpha(mat, dst, alpha);
        } else {
            dst = mat.clone();
        }

        pdf.drawMat(GlobalFun::convertMatToQImage(dst + mask));
    } else {
        pdf.drawMat(GlobalFun::convertMatToQImage(mat));
    }

    // 2D、3D 图
    if ( plot_type == 2 ) {
        QCustomPlot *customPlot = static_cast<QCustomPlot *>( ui->view_tab1->children().at(0) );
        QPixmap pixmap = customPlot->toPixmap();
        pdf.drawPlot(pixmap.toImage());
    } else {
        int index = ui->view->currentIndex();
        if ( index == 0 ) {
            ui->view->setCurrentIndex(1);
            GlobalFun::bsleep(500);
        }
        QPixmap pixmap = m_graph->screen()->grabWindow( m_graph->winId(), 0, 0, m_graph->width(), m_graph->height() );
        QImage image = pixmap.toImage();
        pdf.drawPlot(pixmap.toImage());
    }

    // 产品信息
    QString removeStr = "";
    if ( GlobalValue::zer_pis == 1 ) { removeStr += "Piston; "; }
    if ( GlobalValue::zer_tilt == 1 ) { removeStr += "Tilt; "; }
    if ( GlobalValue::zer_pow == 1 ) { removeStr += "Power; "; }
    if ( GlobalValue::zer_ast == 1 ) { removeStr += "Ast; "; }
    if ( GlobalValue::zer_coma == 1 ) { removeStr += "Coma; "; }
    if ( GlobalValue::zer_sph == 1 ) { removeStr += "Spherical; "; }
    if ( removeStr == "" ) { removeStr = "/"; }
    serial_id = GlobalValue::file_device_id == "" ? "/":GlobalValue::file_device_id;

    list.clear();
    list << product_name << product_id << quality_tester << GlobalFun::getCurrentTime(5) << serial_id << comments << removeStr;
    pdf.drawProductInfo(GlobalValue::lgn_tp == 1, list);

    // Seidel
    QString unit = "";
    switch (GlobalValue::com_unit) {
    case 0: unit = QStringLiteral(" λ"); break;
    case 1: unit = QStringLiteral(" fr"); break;
    case 2: unit = QStringLiteral(" nm"); break;
    case 3: unit = QStringLiteral(" μm"); break;
    case 4: unit = QStringLiteral(" mm"); break;
    default: unit = QStringLiteral(" nm"); break;
    }

    float decimal = 5 / pow(10, GlobalValue::com_dcl+1);
    int divisor = pow(10, GlobalValue::com_dcl);

    list.clear();
    list << QString::number(floor((m_algorithm->totalResult.zerSeiResult.seidelTilt + decimal) * divisor) / divisor) + unit
         << QString::number(floor((m_algorithm->totalResult.zerSeiResult.seidelFocus + decimal) * divisor) / divisor) + unit
         << QString::number(floor((m_algorithm->totalResult.zerSeiResult.seidelAst + decimal) * divisor) / divisor) + unit
         << QString::number(floor((m_algorithm->totalResult.zerSeiResult.seidelComa + decimal) * divisor) / divisor) + unit
         << QString::number(floor((m_algorithm->totalResult.zerSeiResult.seidelSpherical + decimal) * divisor) / divisor) + unit
         << QString::number(floor((m_algorithm->totalResult.zerSeiResult.seidelTiltClock + decimal) * divisor) / divisor) << "/"
         << QString::number(floor((m_algorithm->totalResult.zerSeiResult.seidelAstClock + decimal) * divisor) / divisor)
         << QString::number(floor((m_algorithm->totalResult.zerSeiResult.seidelComaClock + decimal) * divisor) / divisor) << "/";

    pdf.drawSeidelInfo(list);

    // Result
    QVector<QStringList> vec;
    list.clear();
    QStringList vulist;

    int count = 12;
    for ( int i = 0; i < ui->result_table->rowCount(); ++i )
    {
        list << ui->result_table->item(i, 0)->text();
        vulist << ui->result_table->item(i, 1)->text() + " " + ui->result_table->item(i, 2)->text();
        count--;
        if ( count == 0 ) { break; }
    }

    vec.push_back(list);
    vec.push_back(vulist);
    pdf.drawResultInfo(vec);

    return true;
}

void MainWindow::writeHead(QTextStream &out, int type)
{
    switch (type) {
    case 1: {
        // line 1: Zygo XYZ Data File - Format 1
        out << "Zygo XYZ Data File - Format 1"
            << "\n";
    } break;
    case 2: {
        // line 1: Zygo ASCII Data File - Format 2
        out << "Zygo ASCII Data File - Format 2"
            << "\n";
    } break;
    case 3: {
        // line 1: ShineOptics Sirius Data File - Format 3
        out << "ShineOptics Sirius Data File - Format 3"
            << "\n";

        // 移相算法 0-4A 1-5AH 2-5BCS 3-9ACS 4-9BCS 5-AIA
        int psi = GlobalValue::par_psi;

        // 横向分辨率（单位：m/pixel）
        double camerares = 1 / GlobalValue::gra_pixel * 1.0f / 1000;

        // 强度数据矩阵的宽度（列）
        int intenswidth = m_matList.at(0).cols;

        // 强度数据矩阵的高度（行）
        int intensheight = m_matList.at(0).rows;

        // line 2: PSI CameraRes IntensWidth IntensHeight
        out << psi << " "
            << camerares << " "
            << intenswidth << " "
            << intensheight << " "
            << "\n";

        // 创建文件的时间和日期
        QDateTime currentdate = QDateTime::currentDateTime();
        QString format = "ddd MMM dd hh:mm:ss yyyy";
        QLocale locale = QLocale::English;
        QString softwaredate = locale.toString(currentdate, format);

        // line 3: SoftwareDate
        out << "\"" << softwaredate << "\""
            << "\n";

        return;
    };
    default: break;
    }

    // 指示哪个程序创建了数据文件：未知（0），MetroPro（1），MetroBASIC（2），d2bug（3）
    int softwaretype = 0;

    // 创建文件的程序的版本号
    int majorvers = 3;
    int minorvers = 0;
    int bugvers = 0;

    // 创建文件的时间和日期
    QDateTime currentdate = QDateTime::currentDateTime();
    QString format = "ddd MMM dd hh:mm:ss yyyy";
    QLocale locale = QLocale::English;
    QString softwaredate = locale.toString(currentdate, format);

    // line 2: SoftwareType MajorVers MinorVers BugVers SoftwareDate
    out << softwaretype << " "
        << majorvers << " "
        << minorvers << " "
        << bugvers << " "
        << "\"" << softwaredate << "\""
        << "\n";

    // 强度数据矩阵的原点的坐标，它们引用相机坐标系中的位置，相机坐标系的原点（0，0）位于视频监视器的左上角
    int intensoriginx = 0;
    int intensoriginy = 0;

    // 强度数据矩阵的宽度（列）和高度（行），如果没有强度数据，则该值为零
    int intenswidth = m_matList.at(0).cols;
    int intensheight = m_matList.at(0).rows;

    // 强度数据矩阵的数量，如果不存在强度数据矩阵，则该值为零
    int nbuckets = 1;

    // 强度数据点的最大可能值
    int intensrange = 255;

    // line 3: IntensOriginX IntensOriginY IntensWidth IntensHeight NBuckets IntensRange
    out << intensoriginx << " "
        << intensoriginy << " "
        << intenswidth << " "
        << intensheight << " "
        << nbuckets << " "
        << intensrange
        << "\n";

    // 相位数据矩阵的原点的坐标，它们引用相机坐标系中的位置，相机坐标系的原点（0，0）位于视频监视器的左上角
    int phaseoriginx = 0;
    int phaseoriginy = 0;

    // 相位数据矩阵的宽度（列）和高度（行），如果没有相位数据矩阵，则该值为零
    int phasewidth = m_matList.at(0).cols;
    int phaseheight = m_matList.at(0).rows;

    // line 4: PhaseOriginX PhaseOriginY PhaseWidth PhaseHeight
    out << phaseoriginx << " "
        << phaseoriginy << " "
        << phasewidth << " "
        << phaseheight
        << "\n";

    // 用户输入的注释行，81 个字符
    QString comment(81, ' ');

    // line 5: Comment
    out << "\"" << comment << "\""
        << "\n";

    // 用户输入的所测零件的序列号，39 个字符
    QString partsernum(39, ' ');

    // line 6: PartSerNum
    out << "\"" << partsernum << "\""
        << "\n";

    // 用户输入的所测零件的标识符，39 个字符
    QString partnum(39, ' ');

    // line 7: PartNum
    out << "\"" << partnum << "\""
        << "\n";

    // 数据源，0 - 数据来自仪器；1 - 已生成数据
    int source = 0;

    // 干涉比例因子，是用户指定的每个条纹的波数
    double intfscalefactor = GlobalValue::par_i_s_f;

    // 测量干涉图的波长（以米为单位）
    double wavelengthin = GlobalValue::par_t_w / 1000000000.0;

    // 1 /（2 * f值）
    double munericaperture = 0.5;

    // 在显微镜上使用 Mirau 物镜时所需的相位校正因子，值为 1.0 表示不需要校正因子
    double obliquityfactor = 1.0;

    // 保留供将来使用
    double magnification = 0;

    // 相机像素的横向分辨率，以米/像素为单位，值为 0 表示该值未知
    double camerares = 1 / GlobalValue::gra_pixel * 1.0f / 1000;

    // 测量或生成数据的日期和时间的系统表示，它是自1970年1月1日0:00:00以来的秒数
    QDateTime datebegin(QDate(1970, 1, 1), QTime(0, 0));
    int timestamp = datebegin.secsTo(currentdate);

    // line 8: Source IntfScaleFactor WavelengthIn NumericAperture ObliquityFactor Magnification CameraRes TimeStamp
    out << source << " "
        << intfscalefactor << " "
        << wavelengthin << " "
        << munericaperture << " "
        << obliquityfactor << " "
        << magnification << " "
        << camerares << " "
        << timestamp << " "
        << "\n";

    // 相机字段的宽度（列）和高度（行），以像素为单位
    int camerawidth = m_matList.at(0).cols;
    int cameraheight = m_matList.at(0).rows;

    // 进行测量的系统的类型。该系统可能是：
    // 无（0），Mark IVxp（1），Maxim•3D（2），Maxim•NT（3），GPI-XP（4），
    // NewView（5），Maxim•GP（6），NewView / GP（7），标记到GPI转换（8）
    int systemtype = 0;

    // 进行数据测量时使用的是哪个系统板，有效值的范围是 0 ~ 7
    int systemboard = 0;

    // 仪器的序列号
    int systemserial = 666666;

    // 仪器单位编号，有效值的范围是 0 ~ 7
    int instrumentid = 0;

    // 这是一个11个字符的字符串。对于显微镜，该字段指示进行测量时使用的物镜。对于GPI，此字段指示进行测量时使用的光圈。如果生成了数据，则此字段为空白
    QString objectname(11, ' ');

    // line 9: CameraWidth CameraHeight SystemType SystemBoard SystemSerial InstrumentId ObjectiveName
    out << camerawidth << " "
        << cameraheight << " "
        << systemtype << " "
        << systemboard << " "
        << systemserial << " "
        << instrumentid << " "
        <<  "\"" << objectname << "\""
        << "\n";

    // 采集模式：相位（0），条纹（1），扫描（2）
    int acquiremode = 0;

    // 所执行的强度平均值的数量，0 或 1 表示无平均
    int intensavgs = 0;

    // 在采集过程中是否自动调整了调制幅度，1 - 调整，0 - 不调整
    int pztcal = 1;

    // 数据采集期间使用的调制幅度值
    int pztgain = 33;

    // 如果调整了PZT校准，则此整数指定PZT误差范围
    int pztgaintolerace = 0;

    // 在数据采集期间是否执行了自动增益控制：1 - 使用AGC，0 - 未使用AGC
    int agc = 0;

    // 在AGC期间使用的光强度的可接受公差极限
    double targetrange = 0.1;

    // 数据采集期间使用的照明水平设置
    double lightlevel = 60.4318;

    // 计算相位值所需的最小调制值
    // MinMod 等于 10.23 * MinMod（％）。MinMod（％）是一个用户设置，指示每个相机像素必须具有的全调制百分比才能被接受为有效数据点
    int minmod = 2;

    // 在AGC期间通过MinMod标准所需的最小数据点数
    int minmodpts = 50;

    // line 10: AcquireMode IntensAvgs PZTCal PZTGain PZTGainTolerance AGC TargetRange LightLevel MinMod MinModPts
    out << acquiremode << " "
        << intensavgs << " "
        << pztcal << " "
        << pztgain << " "
        << pztgaintolerace << " "
        << agc << " "
        << targetrange << " "
        << lightlevel << " "
        << minmod << " "
        << minmodpts << " "
        << "\n";

    // 相位数据点的分辨率，0 - 正常分辨率，每个条纹由4096个计数表示，1 - 高分辨率，每个条纹由32768个计数表示
    int phaseres = 0;

    // 平均相位数
    int phaseavgs = 1;

    // 有效数据区域所需的最小连续数据点数
    int minimumareasize = 100;

    // 系统在相位数据中遇到不连续时采取的措施，不连续动作是：删除区域（0），过滤区域（1），忽略（2）
    int disconaction = 0;

    // 在过滤DisconAction时除去不连续性的程度，有效值的范围是 0（无）~ 100（所有）
    double disconfilter = 0;

    // 处理相位数据的各个区域的顺序，顺序可以是按位置（0）或按大小（1）
    int connectionorder = 0;

    // 是否从相位数据中删除了倾斜偏差，1 - 已将其删除；0 - 未将其删除
    int removetiltbias = 0;

    // 数据的符号，数据符号可以是正常（0）或倒置（1）
    int datasign = 0;

    // 相位数据表示波阵面（0）还是表面（1），此信息由CODE V程序使用
    int codevtype = 0;

    // line 11: PhaseRes PhaseAvgs MinimumAreaSize DisconAction DisconFilter ConnectionOrder RemoveTiltBias DataSign CodeVType
    out << phaseres << " "
        << phaseavgs << " "
        << minimumareasize << " "
        << disconaction << " "
        << disconfilter << " "
        << connectionorder << " "
        << removetiltbias << " "
        << datasign << " "
        << codevtype << " "
        << "\n";

    // 是否从相位数据中减去系统误差，1 - 已减去；0 - 未减去
    int subtractsyserr = 0;

    // 系统错误数据的文件的用户输入名称，27个字符
    QString syserrfile(27, ' ');

    // line 12: SubtractSysErr SysErrFile
    out << subtractsyserr << " "
        << "\"" << syserrfile << "\""
        << "\n";

    // 折射率，当前此值仅用于角corner角二面角的计算
    double refractiveindex = GlobalValue::par_ref_index;

    // 所测量零件的厚度（以米为单位），当前该值仅与均一性的计算有关
    double partthickness = 0;

    // line 13: RefractiveIndex PartThickness
    out << refractiveindex << " "
        << partthickness << " "
        << "\n";

    if ( type == 2 ) {
        // 数据获取期间使用的图像缩放的值，7个字符
        QString zoomdesc(7, ' ');

        // line 14: ZoomDesc
        out << "\"" << zoomdesc << "\""
            << "\n";
    }
}

void MainWindow::saveXYZ(QString path)
{
    // XYZ 文件格式：标头 + 0张强度图 + 处理后的相位图（即拟合图）

    QFile file(path);
    if ( !file.open(QIODevice :: WriteOnly | QIODevice :: Truncate) ) {
        return;
    }

    QTextStream out(&file);

    writeHead(out, 1);

    // phase data matrix
    // 原始ASCII文件的组成
    // 第一行：组成数据点的总数
    // 后续行：列号（x），行号（y）和数据值（z）。这些数字用空格分隔。行数等于数据点数。数据以行优先顺序显示。数据值（z）是二进制文件中连接的相位数据值
    // 倒数第二行：所有数据点的最小值
    // 最后一行：最大数据值减去最小数据值
    out << "#"
        << "\n";

    cv::Mat mat = m_algorithm->fittingSurface_fillSpikes.clone();

    cv::Mat image = cv::Mat(m_matList.at(0).rows, m_matList.at(0).cols, CV_32FC1, cv::Scalar(nan("")));

    if ( !isSirius ) {
        // 允许用户加载xyz文件之后，再保存asc或者xyz。此时允许不画掩膜，以算法中的位置填充数据
        int startX = m_algorithm->mask_rect.x;
        int endX = m_algorithm->mask_rect.x + m_algorithm->mask_rect.width;
        int startY = m_algorithm->mask_rect.y;
        int endY = m_algorithm->mask_rect.y + m_algorithm->mask_rect.height;
        mat.copyTo(image(cv::Range(startY, endY), cv::Range(startX, endX)));
    } else {
        // fittingSurface 的尺寸比 unwrappedPhase 小，需要内缩dx/2
        qreal dx = m_rect.width - mat.cols;
        qreal dy = m_rect.height - mat.rows;
        qreal startX = m_rect.x + dx/2;
        qreal endX = startX + mat.cols;
        qreal startY = m_rect.y + dy/2;
        qreal endY = startY + mat.rows;
        mat.copyTo(image(cv::Range(startY, endY), cv::Range(startX, endX)));
    }

    image = image * GlobalValue::par_t_w / 1000;

    for ( int r = 0; r < image.rows; ++r )
    {
        const float *pdata = image.ptr<float>(r);

        for ( int c = 0; c < image.cols; ++c )
        {
            if ( isnan(*pdata) ) {
                out << c << " "
                    << r << " "
                    << "No Data" << "\n";
            } else {
                out << c << " "
                    << r << " "
                    << *pdata << "\n";
            }
            pdata++;
        }
    }

    // end
    out << "#"
        << "\n";

    file.close();

    GlobalFun::showMessageBox(2, "Save XYZ file successfully !");
}

void MainWindow::loadXYZ(QString path)
{
    // XYZ 文件数据用途：无强度图，相位图传给算法进行分析

    QFile file(path);
    if ( !file.open(QIODevice :: ReadOnly) ) {
        loadFileState = false;
        return;
    }

    QTextStream in(&file);

    QString line;
    QStringList list;

    line = in.readLine();
    line = in.readLine();
    line = in.readLine();
    line = in.readLine();   // line4
    list = line.split(" ", QString::SkipEmptyParts);

    int phasewidth = list.at(2).toInt();
    int phaseheight = list.at(3).toInt();

    line = in.readLine();
    line = in.readLine();
    line = in.readLine();
    line = in.readLine();   // line8
    list = line.split(" ", QString::SkipEmptyParts);

    double camerares = list.at(6).toDouble();
    updateGraphItemSize(camerares);

    line = in.readLine();
    line = in.readLine();
    line = in.readLine();   // line11
    list = line.split(" ", QString::SkipEmptyParts);

    int phaseres =  list.at(0).toInt();

    line = in.readLine();
    line = in.readLine();   // line 13
    line = in.readLine();   // #

    int phaselines = phasewidth * phaseheight;
    float *phasedata = new float[phasewidth * phaseheight];
    float *phasedata_t = phasedata;

    int R = 32768;
    if ( phaseres == 0 ) {
        R = 4096;
    } else if ( phaseres == 1 ) {
        R = 32768;
    }

    for ( int i = 0; i < phaselines; ++i )
    {
        line = in.readLine();
        list = line.split(" ", QString::SkipEmptyParts);
        QString str = list.at(2);
        float value = str.toDouble();

        *phasedata = str == "No" ? nan("") : value * 1000 / GlobalValue::par_t_w;
        phasedata++;
    }

    m_phase = cv::Mat(phaseheight, phasewidth, CV_32FC1, phasedata_t).clone();
    cv::Mat ret = GlobalFun::dataProcessing(m_phase, false);
    m_matList.clear();
    m_matList.push_back(ret);

    file.close();

    isSirius = false;
    loadFileState = true;
    startCalculation();
}

void MainWindow::saveACS(QString path)
{
    // ASC 文件格式：标头 + 1张强度图 + 原始位图（即解包裹后的相位图）

    QFile file(path);
    if ( !file.open(QIODevice :: WriteOnly | QIODevice :: Truncate) ) {
        return;
    }

    QTextStream out(&file);

    writeHead(out, 2);

    // intensity data matrix
    out << "#"
        << "\n";

    cv::Mat mat = m_matList.at(0).clone();
    unsigned char *data = mat.data;
    int count = 0;

    for ( int c = 0; c < mat.cols; ++c )
    {
        for ( int r = 0; r < mat.rows; ++r )
        {
            out << *data << " ";
            data += mat.type() == CV_8UC4 ? 4 : (mat.type() == CV_8UC3 ? 3 : 1);

            count++;
            if ( count == 10 ) {
                out << "\n";
                count = 0;
            }
        }
    }

    if ( count != 0 ) {
        out << "\n";
    }

    // phase data matrix
    out << "#"
        << "\n";

    const int phase_invalid_value = 2147483640;
    count = 0;

    mat = m_algorithm->unwrappedPhase_original.clone();

    cv::Mat image = cv::Mat(m_matList.at(0).rows, m_matList.at(0).cols, CV_32FC1, cv::Scalar(nan("")));

    if ( !isSirius ) {
        // 允许用户加载asc文件之后，再保存asc或者xyz。此时允许不画掩膜，以算法中的位置填充数据
        int startX = m_algorithm->mask_rect.x;
        int endX = m_algorithm->mask_rect.x + m_algorithm->mask_rect.width;
        int startY = m_algorithm->mask_rect.y;
        int endY = m_algorithm->mask_rect.y + m_algorithm->mask_rect.height;
        mat.copyTo(image(cv::Range(startY, endY), cv::Range(startX, endX)));
    } else {
        qreal dx = m_rect.width - mat.cols;
        qreal dy = m_rect.height - mat.rows;
        qreal startX = m_rect.x + dx/2;
        qreal endX = startX + mat.cols;
        qreal startY = m_rect.y + dy/2;
        qreal endY = startY + mat.rows;
        mat.copyTo(image(cv::Range(startY, endY), cv::Range(startX, endX)));
    }

    image = image * 4096 / (GlobalValue::par_i_s_f * 1.0);

    for ( int r = 0; r < image.rows; ++r )
    {
        const float *pdata = image.ptr<float>(r);

        for ( int c = 0; c < image.cols; ++c )
        {
            if ( isnan(*pdata) ) {
                out << phase_invalid_value << " ";
            } else {
                out << int(*pdata) << " ";
            }
            pdata++;
            count++;
            if ( count == 10 ) {
                out << "\n";
                count = 0;
            }
        }
    }

    if ( count != 0 ) {
        out << "\n";
    }

    // end
    out << "#"
        << "\n";

    file.close();

    GlobalFun::showMessageBox(2, "Save ASC file successfully !");
}

void MainWindow::loadASC(QString path)
{
    // ASC 文件数据用途：强度图在场景中显示，相位图传给算法进行分析

    QFile file(path);
    if ( !file.open(QIODevice :: ReadOnly) ) {
        loadFileState = false;
        return;
    }

    QTextStream in(&file);

    QString line;
    QStringList list;

    line = in.readLine();
    line = in.readLine();
    line = in.readLine();   // line3
    list = line.split(" ", QString::SkipEmptyParts);

    int intenswidth = list.at(2).toInt();
    int intensheight = list.at(3).toInt();
    int nbuckets = list.at(4).toInt();
    int intensrange = list.at(5).toInt();

    // 相位计算 加、减（需判断大小是否一致）
    if ( calc_type == 2 || calc_type == 3 ) {
        if ( m_calc_base.cols != intenswidth || m_calc_base.rows != intensheight ) {
            GlobalFun::showMessageBox(3, "Different mask sizes !");
            calc_type = 7;
            file.close();
            return ;
        }
    }

    line = in.readLine();   // line4
    list = line.split(" ", QString::SkipEmptyParts);

    int phasewidth = list.at(2).toInt();
    int phaseheight = list.at(3).toInt();

    line = in.readLine();
    line = in.readLine();
    line = in.readLine();
    line = in.readLine();   // line8
    list = line.split(" ", QString::SkipEmptyParts);

    double intfscalefactor = list.at(1).toDouble();
    int obliquityfactor = list.at(4).toInt();
    double camerares = list.at(6).toDouble();
    updateGraphItemSize(camerares);

    line = in.readLine();
    line = in.readLine();
    line = in.readLine();   // line11
    list = line.split(" ", QString::SkipEmptyParts);

    int phaseres =  list.at(0).toInt();

    line = in.readLine();
    line = in.readLine();
    line = in.readLine();   // line 14
    line = in.readLine();   // #

    int intenslines = int(ceil(double(intenswidth * intensheight * nbuckets) / 10.0));
    int *intensdata = new int[intenswidth * intensheight * nbuckets];
    int *intensdata_t = intensdata;

    for ( int i = 0; i < intenslines; ++i )
    {
        line = in.readLine();
        list = line.split(" ", QString::SkipEmptyParts);

        for ( int j = 0; j < list.size(); ++j )
        {
            int value = list[j].toInt();
            *intensdata = value * 255 / intensrange;
            intensdata++;
        }
    }

    m_matList.clear();
    for ( int i = 0; i < nbuckets; ++i )
    {
        cv::Mat mat(intensheight, intenswidth, CV_8UC4, intensdata_t + (intensheight * intenswidth) * i);
        unsigned char *data = mat.data;

        for ( int i = 0; i < mat.cols; ++i )
        {
            for ( int j = 0; j < mat.rows; ++j )
            {
                *(data + 1) = *data;
                *(data + 2) = *data;
                *(data + 3) = 255;
                data += 4;
            }
        }

        m_matList.push_back(mat.clone());
    }
    delete []intensdata_t;

    line = in.readLine();   // #

    int phaselines = int(ceil(double(phasewidth * phaseheight) / 10.0));
    float *phasedata = new float[phasewidth * phaseheight];
    float *phasedata_t = phasedata;
    const int phase_invalid_value = 2147483640;

    int R = 32768;
    if ( phaseres == 0 ) {
        R = 4096;
    } else if ( phaseres == 1 ) {
        R = 32768;
    }

    for ( int i = 0; i < phaselines; ++i )
    {
        line = in.readLine();
        list = line.split(" ", QString::SkipEmptyParts);

        for ( int j = 0; j < list.size(); ++j )
        {
            int value = list[j].toInt();
            if ( value >= phase_invalid_value ) {
                *phasedata = nan("");
            } else {
                *phasedata = float(value) * (intfscalefactor * obliquityfactor) / R;
            }
            phasedata++;
        }
    }

    m_phase = cv::Mat(phaseheight, phasewidth, CV_32FC1, phasedata_t).clone();

    switch ( calc_type )
    {
    case 1: {
        m_calc_base = m_phase.clone();
        asc_res = camerares;
    } break;
    case 2: {
        m_calc_ope = m_phase.clone();
        m_phase = BaseFunc::calculator_add(m_calc_base,m_calc_ope);
        m_calc_base = m_phase.clone();
        asc_res = camerares;
        calc_type = 7;
    } break;
    case 3: {
        m_calc_ope = m_phase.clone();
        m_phase = BaseFunc::calculator_sub(m_calc_base,m_calc_ope);
        m_calc_base = m_phase.clone();
        asc_res = camerares;
        calc_type = 7;
    } break;
    default: break;
    }

    delete []phasedata_t;
    file.close();

    isSirius = false;
    loadFileState = true;
    startCalculation();
}

void MainWindow::saveSirius(QString path)
{
    // Sirius 文件格式：标头 + 原始图片（数量根据算法） + 无相位图

    QFile file(path);
    if ( !file.open(QIODevice :: WriteOnly | QIODevice :: Truncate) ) {
        return;
    }

    QTextStream out(&file);

    writeHead(out, 3);

    // intensity data matrix
    out << "#"
        << "\n";

    int count = 0;
    for ( size_t i = 0; i < m_matList.size(); ++i )
    {
        cv::Mat mat = m_matList.at(i).clone();
        unsigned char *data = mat.data;

        for ( int c = 0; c < mat.cols; ++c )
        {
            for ( int r = 0; r < mat.rows; ++r )
            {
                out << *data << " ";
                data += mat.type() == CV_8UC4 ? 4 : (mat.type() == CV_8UC3 ? 3 : 1);

                count++;
                if ( count == 10 ) {
                    out << "\n";
                    count = 0;
                }
            }
        }
    }

    if ( count != 0 ) {
        out << "\n";
    }

    // end
    out << "#"
        << "\n";

    file.close();

    GlobalFun::showMessageBox(2, "Save Sirius file successfully !");
}

void MainWindow::loadSirius(QString path)
{
    // Sirius 文件数据用途：强度图在场景中显示，无相位图

    QFile file(path);
    if ( !file.open(QIODevice :: ReadOnly) ) {
        loadFileState = false;
        return;
    }

    QTextStream in(&file);

    QString line;
    QStringList list;

    line = in.readLine();   // Format 3
    line = in.readLine();
    list = line.split(" ", QString::SkipEmptyParts);

    int psi = list.at(0).toInt();

    if ( psi != GlobalValue::par_psi ) {
        auto str = [](int psi)->QString {
            switch ( psi )
            {
            case 1: return "PSI_METHOD_BUCKETB5A_H_P";
            case 3: return "PSI_METHOD_BUCKET9A_CS_P";
            case 5: return "PSI_METHOD_OPT_SEQUENCE";
            default: return "";
            }
        };
        GlobalFun::showMessageBox(3, QString("The PSI algorithm read from the file is different from the setting. "
                                             "%1 is in the file and "
                                             "%2 is in the setting.").arg(str(psi)).arg(str(GlobalValue::par_psi)));
        loadFileState = false;
        return;
    }

    double camerares = list.at(1).toDouble();
    int intenswidth = list.at(2).toInt();
    int intensheight = list.at(3).toInt();
    updateGraphItemSize(camerares);

    line = in.readLine();   // date
    line = in.readLine();   // #

    m_matList.clear();
    int nbuckets = 0;
    if ( psi == 1 ) {
        nbuckets = 5;
    } else if ( psi == 3 || psi == 5 ) {
        nbuckets = 9;
    }

    int intenslines = int(ceil(double(intenswidth * intensheight * nbuckets) / 10.0));
    int *intensdata = new int[intenswidth * intensheight * nbuckets];
    int *intensdata_t = intensdata;

    for ( int i = 0; i < intenslines; ++i )
    {
        line = in.readLine();
        list = line.split(" ", QString::SkipEmptyParts);

        for ( int j = 0; j < list.size(); ++j )
        {
            int value = list.at(j).toInt();
            *intensdata = value;
            intensdata++;
        }
    }

    for ( int i = 0; i < nbuckets; ++i )
    {
        cv::Mat mat(intensheight, intenswidth, CV_8UC4, intensdata_t + (intensheight * intenswidth) * i);

        unsigned char *data = mat.data;
        for ( int i = 0; i < mat.cols; ++i )
        {
            for ( int j = 0; j < mat.rows; ++j )
            {
                *(data + 1) = *data;
                *(data + 2) = *data;
                *(data + 3) = 255;
                data += 4;
            }
        }

        m_matList.push_back(mat.clone());
    }

    delete []intensdata_t;
    file.close();

    isSirius = true;
    loadFileState = true;
    startCalculation();
}

//******************************************************************************************************************
// [3] 创建表格 + 弹窗

void MainWindow::createResultTable()
{
    // clear
    ui->result_table->clear();

    // column count
    ui->result_table->setColumnCount(3);
    ui->result_table->setHorizontalHeaderLabels(QStringList() << "   Item   " << "   Value   " << "Unit");

    QString unit = "";
    switch (GlobalValue::com_unit) {
    case 0: unit = QStringLiteral("λ"); break;
    case 1: unit = QStringLiteral("fr"); break;
    case 2: unit = QStringLiteral("nm"); break;
    case 3: unit = QStringLiteral("μm"); break;
    case 4: unit = QStringLiteral("mm"); break;
    default: unit = QStringLiteral("nm"); break;
    }

    // row count
    int rowCount = 0;
    for ( int i = 0; i < m_algParameter.size(); ++i )
    {
        if ( m_stateList.value( m_algParameter.at(i) ) == 1 ) {
            ui->result_table->setRowCount(++rowCount);
            ui->result_table->setItem( rowCount - 1, 0, new QTableWidgetItem(m_algParameter.at(i)) );
            ui->result_table->setItem( rowCount - 1, 1, new QTableWidgetItem("") );
            if ( i == 24 || i == 27 || i == 29 ) {  // Clock
                ui->result_table->setItem( rowCount - 1, 2, new QTableWidgetItem("deg") );
            } else if ( i >= 31 && i <= 36 ) {  // ISO10110
                ui->result_table->setItem( rowCount - 1, 2, new QTableWidgetItem(QString("fr@%1").arg(GlobalValue::par_i_w)) );
            } else if ( i == 38 ) { // Fringes
                ui->result_table->setItem( rowCount - 1, 2, new QTableWidgetItem("fr") );
            } else if ( i == 40 ) { // Parallel_Theta
                ui->result_table->setItem( rowCount - 1, 2, new QTableWidgetItem("sec") );
            } else if ( i == 41 || i == 42 || i == 43 || i == 45 ) {   // Aperture WE-100
                ui->result_table->setItem( rowCount - 1, 2, new QTableWidgetItem("mm") );
            } else if ( i == 11 ) {  // GRMS
                ui->result_table->setItem( rowCount - 1, 2, new QTableWidgetItem(unit + "/mm") );
            }  else if ( i == 39 || i == 44 ) { // Strehl_Ratio and Concavity
                ui->result_table->setItem( rowCount - 1, 2, new QTableWidgetItem("/") );
            } else {    // unit
                ui->result_table->setItem( rowCount - 1, 2, new QTableWidgetItem(unit) );
            }

            ui->result_table->item(rowCount - 1, 0)->setTextAlignment(Qt::AlignCenter);
            ui->result_table->item(rowCount - 1, 1)->setTextAlignment(Qt::AlignCenter);
            ui->result_table->item(rowCount - 1, 2)->setTextAlignment(Qt::AlignCenter);
        }
    }

    // setting
    ui->result_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->result_table->horizontalHeader()->setFrameShape(QFrame::NoFrame);
    ui->result_table->verticalHeader()->setVisible(false);
    ui->result_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->result_table->setAlternatingRowColors(true);
    ui->result_table->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->result_table->setFocusPolicy(Qt::StrongFocus);
}

void MainWindow::createZernikeTable()
{
    // clear
    ui->zernike_table->clear();

    // column count
    ui->zernike_table->setColumnCount(3);
    ui->zernike_table->setHorizontalHeaderLabels(QStringList() << "Meaning" << "   Value   " << "Unit");

    // row count
    ui->zernike_table->setRowCount(GlobalValue::par_ztm);
    QStringList list;
    list << "Piston Or Bias" << "Tilt X" << "Tilt Y" << "Power" << "Astigmatism X" <<
            "Astigmatism Y" << "Coma X" << "Coma Y" << "Primary Spherical" << "Trefoil X" <<
            "Trefoil Y" << "Secondary Astigmatism X" << "Secondary Astigmaitsm Y" <<
            "Secondary Coma X" << "Secondary Coma Y" << "Secondary Spherical" <<
            "Tetrafoil X" << "Tetrafoil Y" << "Secondary Trefoil X" << "Secondary Trefoil Y" <<
            "Tertiary Astigmatism X" << "Tertiary Astigmatism Y" << "Tertiary Coma X" <<
            "Tertiary Coma Y" << "Tertiary Spherical" << "Pentafoil X" << "Pentafoil Y" <<
            "Secondary Tetrafoil X" << "Secondary Tetrafoil Y" << "Tertiary Trefoil X" <<
            "Tertiary Trefoil Y" << "Quatenary Astigmatism X" << "Quatenary Astigmatism Y" <<
            "Quatenary Coma X" << "Quatenary Coma Y" << "Quatenary Spherical" << "Quinary Spherical";

    QString unit = "";
    switch (GlobalValue::com_unit) {
    case 0: unit = QStringLiteral("λ"); break;
    case 1: unit = QStringLiteral("fr"); break;
    case 2: unit = QStringLiteral("nm"); break;
    case 3: unit = QStringLiteral("μm"); break;
    case 4: unit = QStringLiteral("mm"); break;
    default: unit = QStringLiteral("nm"); break;
    }

    for ( int i = 0; i < ui->zernike_table->rowCount(); ++i )
    {
        ui->zernike_table->setItem(i, 0, new QTableWidgetItem(list.at(i)));
        ui->zernike_table->setItem(i, 1, new QTableWidgetItem(""));
        ui->zernike_table->setItem(i, 2, new QTableWidgetItem(unit) );

        ui->zernike_table->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        ui->zernike_table->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        ui->zernike_table->item(i, 2)->setTextAlignment(Qt::AlignCenter);
    }

    // setting
    ui->zernike_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->zernike_table->horizontalHeader()->setFrameShape(QFrame::NoFrame);
    ui->zernike_table->verticalHeader()->setVisible(false);
    ui->zernike_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->zernike_table->setAlternatingRowColors(true);
    ui->zernike_table->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->zernike_table->setFocusPolicy(Qt::StrongFocus);
}

void MainWindow::createDataTable()
{
    // clear
    ui->data_table->clear();

    // column count
    int columnCount = 1;
    QStringList columnHead;
    columnHead << "ProductID";

    for ( int i = 0; i < m_algParameter.size(); ++i )
    {
        if ( m_stateList.value( m_algParameter.at(i) ) == 1 ) {
            columnCount ++;
            columnHead << m_algParameter.at(i);
        }
    }

    columnHead << "QC" << "Date";
    columnCount += 2;

    ui->data_table->setColumnCount(columnCount);
    ui->data_table->setHorizontalHeaderLabels(columnHead);

    // setting
    ui->data_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->data_table->horizontalHeader()->setStyleSheet(GlobalString::table_style);
    ui->data_table->horizontalHeader()->setSectionsClickable(false);
    ui->data_table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->data_table->verticalHeader()->setFrameShape(QFrame::NoFrame);
    ui->data_table->setAlternatingRowColors(true);
    ui->data_table->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->data_table->setFocusPolicy(Qt::StrongFocus);

    ui->data_table->setRowCount(0);

    // 修改宽度
    for ( int i = 0; i < columnCount; ++i )
    {
        ui->data_table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
        if ( ui->data_table->horizontalHeaderItem(i)->text().size() > 15 ) {
            ui->data_table->setColumnWidth(i, 120);
        } else if ( ui->data_table->horizontalHeaderItem(i)->text().size() > 10 ) {
            ui->data_table->setColumnWidth(i, 100);
        } else {
            ui->data_table->setColumnWidth(i, 80);
        }
    }
}

void MainWindow::create2DView()
{
    QCustomPlot *customPlot = new QCustomPlot(ui->view_tab1);
    if ( ui->view->width() >= ui->view->height() ) {
        customPlot->setFixedSize(ui->view->height(), ui->view->height() - 40);
    } else {
        customPlot->setFixedSize(ui->view->width() - 10, ui->view->width() - 50);
    }

    QCPColorGradient cpg;                               // 渐变色色条
    cpg.setColorStopAt(1, Qt::red);
    cpg.setColorStopAt(0.55, Qt::yellow);
    cpg.setColorStopAt(0.44, Qt::green);
    cpg.setColorStopAt(0, Qt::blue);

    QCPColorScale *colorScale = new QCPColorScale(customPlot);
    colorScale->axis()->setTicks(true);                 // 设置色条显示刻度值
    colorScale->axis()->setRange(0, 1);                 // 设置色条刻度范围
    colorScale->setType(QCPAxis::atRight);              // 设置色条在右边显示
    colorScale->setRangeDrag(false);                    // 禁用色条滚动事件
    colorScale->setRangeZoom(false);                    // 禁用色条缩放事件
    colorScale->setBarWidth(8);                         // 设置色条宽度
    colorScale->setGradient(cpg);                       // 设置色条渐变

    customPlot->plotLayout()->addElement(0, 1, colorScale);
    customPlot->rescaleAxes();
    customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::create3DView()
{
    m_graph = new BQ3DSurface();
    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);

    m_graph->axisX()->setLabelFormat("%.2f");
    m_graph->axisY()->setLabelFormat("%.2f");
    m_graph->axisZ()->setLabelFormat("%.2f");
    m_graph->axisX()->setAutoAdjustRange(true);
    m_graph->axisY()->setAutoAdjustRange(true);
    m_graph->axisZ()->setAutoAdjustRange(true);
    m_graph->axisX()->setLabelAutoRotation(30);
    m_graph->axisY()->setLabelAutoRotation(30);
    m_graph->axisZ()->setLabelAutoRotation(30);

    m_graph->activeTheme()->setType(Q3DTheme::Theme::ThemeRetro);
    m_graph->setSelectionMode(QAbstract3DGraph::SelectionNone);

    container = QWidget::createWindowContainer(m_graph, ui->view_3d);
    container->setGeometry(0, 0, ui->view_3d->width(), ui->view_3d->height());

    QLinearGradient gr;
    gr.setColorAt(1.0, Qt::red);
    gr.setColorAt(0.55, Qt::yellow);
    gr.setColorAt(0.44, Qt::green);
    gr.setColorAt(0, Qt::blue);

    m_sProxy = new QSurfaceDataProxy();
    QSurface3DSeries *m_sSeries = new QSurface3DSeries(m_sProxy);
    m_sSeries->setDrawMode(QSurface3DSeries::DrawSurface);
    m_sSeries->setFlatShadingEnabled(true);
    m_sSeries->setBaseGradient(gr);
    m_sSeries->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    m_graph->addSeries(m_sSeries);

    connect(m_graph, &BQ3DSurface::mousePressed, [&](){
        QMenu *menu = new QMenu(this);

        QAction *fitting_plot = new QAction(QIcon(m_3DPlotType == 1 ? ":/images/Dot.png" : ""), GlobalString::contextMenu_fitting, menu);
        QAction *residual_plot = new QAction(QIcon(m_3DPlotType == 2 ? ":/images/Dot.png" : ""), GlobalString::contextMenu_residual, menu);
        QAction *fillSpikes_plot = new QAction(QIcon(m_3DPlotType == 3 ? ":/images/Dot.png" : ""), GlobalString::contextMenu_fillSpikes, menu);
        connect(fitting_plot, &QAction::triggered, [&](){ change3DPlot(1); });
        connect(residual_plot, &QAction::triggered, [&](){ change3DPlot(2); });
        connect(fillSpikes_plot, &QAction::triggered, [&](){ change3DPlot(3); });
        menu->addAction(fitting_plot);
        menu->addAction(residual_plot);
        menu->addAction(fillSpikes_plot);

        menu->exec(QCursor::pos());
        delete menu;
    });
}

void MainWindow::createLineChart()
{
    xSeries = new QLineSeries(this);
    xSeries->setName("x");
    xSeries->setColor(Qt::red);

    ySeries = new QLineSeries(this);
    ySeries->setName("y");
    ySeries->setColor(Qt::green);

    m_chart = new QChart();
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    m_chart->addSeries(xSeries);
    m_chart->addSeries(ySeries);
    m_chart->createDefaultAxes();

    ui->line_chart->setChart(m_chart);

    // psd
    psd_xSeries = new QLineSeries(this);
    psd_xSeries->setName("x");
    psd_xSeries->setColor(Qt::red);

    psd_ySeries = new QLineSeries(this);
    psd_ySeries->setName("y");
    psd_ySeries->setColor(Qt::green);

    psd_chart = new QChart();

    axisXX = new QCategoryAxis();
    axisYY = new QCategoryAxis();

    psdShowEmpty();

    axisXX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisYY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

    psd_chart->addAxis(axisXX,Qt::AlignBottom);
    psd_chart->addAxis(axisYY,Qt::AlignLeft);
    psd_chart->addSeries(psd_xSeries);
    psd_chart->addSeries(psd_ySeries);
    psd_chart->legend()->setAlignment(Qt::AlignBottom);

    psd_xSeries->attachAxis(axisXX);
    psd_xSeries->attachAxis(axisYY);
    psd_ySeries->attachAxis(axisXX);
    psd_ySeries->attachAxis(axisYY);

    ui->psdXLineChart->setChart(psd_chart);

    label_x = new QLabel("X: Log10", ui->psdXLineChart);
    label_y = new QLabel("Y: Log10", ui->psdXLineChart);
}

void MainWindow::createPSFView()
{
    psf_graph = new BQ3DSurface();
    psf_graph->activeTheme()->setType(Q3DTheme::Theme::ThemeDigia);
    psf_graph->setSelectionMode(QAbstract3DGraph::SelectionNone);

    psf_container = QWidget::createWindowContainer(psf_graph, ui->psf_3d);
    psf_container->setGeometry(0, 0, ui->psf_3d->width(), ui->psf_3d->height());

    QLinearGradient gr;
    gr.setColorAt(1.0, Qt::red);
    gr.setColorAt(0.55, Qt::yellow);
    gr.setColorAt(0.44, Qt::green);
    gr.setColorAt(0, Qt::blue);

    psf_proxy = new QSurfaceDataProxy();
    QSurface3DSeries *m_psfSeries = new QSurface3DSeries(psf_proxy);
    m_psfSeries->setDrawMode(QSurface3DSeries::DrawSurface);
    m_psfSeries->setFlatShadingEnabled(false);
    m_psfSeries->setBaseGradient(gr);
    m_psfSeries->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    psf_graph->addSeries(m_psfSeries);
}

void MainWindow::createQualityTable()
{
    // clear
    ui->quality_table->clear();

    ui->quality_table->setRowCount(1);
    ui->quality_table->setRowHeight(0, ui->automation->height());
    ui->quality_table->setColumnCount(1);
    ui->quality_table->setColumnWidth(0 , ui->automation->width());
    ui->quality_table->verticalHeader()->setVisible(false);
    ui->quality_table->horizontalHeader()->setVisible(false);
    ui->quality_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->quality_table->setSelectionMode(QAbstractItemView::NoSelection);
    ui->quality_table->setWordWrap(true);

    QTableWidgetItem *item = new QTableWidgetItem("QC");
    item->setBackgroundColor(Qt::gray);
    item->setTextColor(QColor(54, 54, 54));

    QFont font(GlobalFun::getTTF(1), 36);
    font.setBold(true);

    item->setFont(font);
    ui->quality_table->setItem(0, 0, item);

    // setting
    ui->quality_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->quality_table->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::createContextMenu(int type)
{
    QMenu *menu = new QMenu(this);

    switch (type)
    {
    case 1: {
        QAction *setting = new QAction(QIcon(":/images/Gear.png"), GlobalString::action_setting, menu);
        connect(setting, &QAction::triggered, [&](){ createMenuDialog(9); });
        menu->addAction(setting);
    } break;
    case 2: {
        QVBoxLayout *layout = new QVBoxLayout();

        QButtonGroup *m_group = new QButtonGroup(this);
        m_group->setExclusive(true);

        QRadioButton *btn4 = new QRadioButton("zernikeCoef_4", menu);
        QRadioButton *btn9 = new QRadioButton("zernikeCoef_9", menu);
        QRadioButton *btn16 = new QRadioButton("zernikeCoef_16", menu);
        QRadioButton *btn25 = new QRadioButton("zernikeCoef_25", menu);
        QRadioButton *btn36 = new QRadioButton("zernikeCoef_36", menu);
        QRadioButton *btn37 = new QRadioButton("zernikeCoef_37", menu);
        btn4->setChecked(GlobalValue::par_ztm == 4);
        btn9->setChecked(GlobalValue::par_ztm == 9);
        btn16->setChecked(GlobalValue::par_ztm == 16);
        btn25->setChecked(GlobalValue::par_ztm == 25);
        btn36->setChecked(GlobalValue::par_ztm == 36);
        btn37->setChecked(GlobalValue::par_ztm == 37);
        m_group->addButton(btn4);
        m_group->addButton(btn9);
        m_group->addButton(btn16);
        m_group->addButton(btn25);
        m_group->addButton(btn36);
        m_group->addButton(btn37);

        connect(btn4, &QRadioButton::clicked, [&](){ GlobalValue::par_ztm = 4; });
        connect(btn9, &QRadioButton::clicked, [&](){ GlobalValue::par_ztm = 9; });
        connect(btn16, &QRadioButton::clicked, [&](){ GlobalValue::par_ztm = 16; });
        connect(btn25, &QRadioButton::clicked, [&](){ GlobalValue::par_ztm = 25; });
        connect(btn36, &QRadioButton::clicked, [&](){ GlobalValue::par_ztm = 36; });
        connect(btn37, &QRadioButton::clicked, [&](){ GlobalValue::par_ztm = 37; });

        QPushButton *sureBtn = new QPushButton(GlobalString::contextMenu_sure, menu);
        connect(sureBtn, &QPushButton::clicked, [&](){
            createZernikeTable();

            if ( !m_algorithm->zernikeCoef_37.empty() ) {
                float decimal = 5 / pow(10, GlobalValue::com_dcl+1);
                int divisor = pow(10, GlobalValue::com_dcl);

                if ( ui->zernike_table->item(0, 1)->text() == "nan" ) {
                    updateZernikeData(decimal, divisor, false);
                } else {
                    updateZernikeData(decimal, divisor, true);
                }
            }

            menu->close();
        });

        QPushButton *exportBtn = new QPushButton(GlobalString::contextMenu_export, menu);
        connect(exportBtn, &QPushButton::clicked, [&](){
            QString path = GlobalFun::getCurrentPath() + "/zernike_" + GlobalFun::getCurrentTime(1) + ".csv";
            QString file = QFileDialog::getSaveFileName(nullptr, GlobalString::action_save, path, "Text files (*.csv)");
            if ( file == "" ) {
                return;
            }
            GlobalFun::exportCSV(file, ui->zernike_table);
            menu->close();
        });

        layout->addWidget(btn4);
        layout->addWidget(btn9);
        layout->addWidget(btn16);
        layout->addWidget(btn25);
        layout->addWidget(btn36);
        layout->addWidget(btn37);
        layout->addWidget(sureBtn);
        layout->addWidget(exportBtn);
        menu->setLayout(layout);
    } break;
    case 3: {
        QAction* clear = new QAction(QIcon(":/images/Clear.png"), GlobalString::contextMenu_clear, menu);
        QAction* cutoff = new QAction(QIcon(":/images/Delete.png"), GlobalString::contextMenu_delete, menu);
        QAction* exportData = new QAction(QIcon(":/images/Folder.png"), GlobalString::contextMenu_export, menu);

        connect(clear, &QAction::triggered, [&](){
            ui->data_table->clearContents();
            ui->data_table->setRowCount(0);

            ui->data_table->horizontalHeader()->setSectionResizeMode(ui->data_table->columnCount() - 1, QHeaderView::Fixed);
            ui->data_table->setColumnWidth(ui->data_table->columnCount() - 1, 80);
            ui->data_table->setColumnWidth(0, 80);
        });

        connect(cutoff, &QAction::triggered, [&](){
            int index = ui->data_table->currentRow();
            if (index != -1) {
                ui->data_table->removeRow(index);
            }

            if ( ui->data_table->rowCount() == 0 ) {
                ui->data_table->horizontalHeader()->setSectionResizeMode(ui->data_table->columnCount() - 1, QHeaderView::Fixed);
                ui->data_table->setColumnWidth(ui->data_table->columnCount() - 1, 80);
                ui->data_table->setColumnWidth(0, 80);
            }
        });

        connect(exportData, &QAction::triggered, [&](){
            if ( ui->data_table->rowCount() == 0 ) {
                GlobalFun::showMessageBox(3, "No log to save !");
            } else {
                QString path = GlobalFun::getCurrentPath() + "/data_" + GlobalFun::getCurrentTime(1) + ".csv";
                QString file = QFileDialog::getSaveFileName(nullptr, GlobalString::action_save, path, "Text files (*.csv)");
                if ( file == "" ) {
                    return;
                }
                GlobalFun::exportCSV(file, ui->data_table);
            }
        });

        menu->addAction(clear);
        menu->addAction(cutoff);
        menu->addAction(exportData);
    } break;
    case 4: {
        QAction *setting = new QAction(QIcon(":/images/Gear.png"), GlobalString::action_setting, menu);
        connect(setting, &QAction::triggered, [&](){ createMenuDialog(10); });
        menu->addAction(setting);
    } break;
    case 5: {
    } break;
    case 6: {
        QAction *fitting_plot = new QAction(QIcon(m_2DPlotType == 1 ? ":/images/Dot.png" : ""), GlobalString::contextMenu_fitting, menu);
        QAction *residual_plot = new QAction(QIcon(m_2DPlotType == 2 ? ":/images/Dot.png" : ""), GlobalString::contextMenu_residual, menu);
        QAction *fillSpikes_plot = new QAction(QIcon(m_2DPlotType == 3 ? ":/images/Dot.png" : ""), GlobalString::contextMenu_fillSpikes, menu);
        connect(fitting_plot, &QAction::triggered, [&](){ change2DPlot(1); });
        connect(residual_plot, &QAction::triggered, [&](){ change2DPlot(2); });
        connect(fillSpikes_plot, &QAction::triggered, [&](){ change2DPlot(3); });
        menu->addAction(fitting_plot);
        menu->addAction(residual_plot);
        menu->addAction(fillSpikes_plot);
    } break;
    case 7: {
    } break;
    case 8: {
    } break;
    case 9: {
    } break;
    default: break;
    }

    menu->exec(QCursor::pos());
    delete menu;
}

void MainWindow::createMenuDialog(int type)
{
    QString temp = GlobalValue::lgn_tp == 1 ? "" : " ";

    switch (type)
    {
    case 1: {
    } break;
    case 2: {
    } break;
    case 3: {
    } break;
    case 4: {
    } break;
    case 5: {
    } break;
    case 6: {
        Customer_engineer_dialog *dialog = new Customer_engineer_dialog(this);
        dialog->exec();
        delete dialog;

        for ( int i = 0; i < ui->result_table->rowCount(); ++i )
        {
            if ( ui->result_table->item(i, 0)->text() == "SAG" ||
                 ui->result_table->item(i, 0)->text() == "IRR" ||
                 ui->result_table->item(i, 0)->text() == "RSI" ||
                 ui->result_table->item(i, 0)->text() == "RMSt" ||
                 ui->result_table->item(i, 0)->text() == "RMSa" ||
                 ui->result_table->item(i, 0)->text() == "RMSi" )
            {
                ui->result_table->item(i, 2)->setText(QString("fr@%1").arg(GlobalValue::par_i_w));
            }
        }

        updateGraphItemSize(-1);
    } break;
    case 7: {
    } break;
    case 8: {
        Unit_setting_dialog *dialog = new Unit_setting_dialog(GlobalString::contextMenu_unit, GlobalString::contextMenu_sure,
                                                              GlobalValue::com_unit, GlobalValue::com_value, this);

        connect(dialog, &Unit_setting_dialog::changeValue, [&](int type, double value){
            GlobalValue::com_unit = type;
            GlobalValue::com_value = value;

            QString unit = "";
            switch (GlobalValue::com_unit) {
            case 0: unit = QStringLiteral("λ"); ui->action_Company->setText(unit + "@" + QString::number(GlobalValue::com_value)); break;
            case 1: unit = QStringLiteral("fr"); ui->action_Company->setText(unit + "@" + QString::number(GlobalValue::com_value/2)); break;
            case 2: unit = QStringLiteral("nm"); ui->action_Company->setText(unit); break;
            case 3: unit = QStringLiteral("μm"); ui->action_Company->setText(unit); break;
            case 4: unit = QStringLiteral("mm"); ui->action_Company->setText(unit); break;
            default: unit = QStringLiteral("nm"); ui->action_Company->setText(unit); break;
            }

            for ( int i = 0; i < ui->result_table->rowCount(); ++i )
            {
                if ( ui->result_table->item(i, 0)->text() == "Seidel_Tilt_Clock" ||
                     ui->result_table->item(i, 0)->text() == "Seidel_Ast_Clock" ||
                     ui->result_table->item(i, 0)->text() == "Seidel_Coma_Clock" ) {
                    ui->result_table->item(i, 2)->setText("deg");
                } else if ( ui->result_table->item(i, 0)->text() == "SAG" ||
                            ui->result_table->item(i, 0)->text() == "IRR" ||
                            ui->result_table->item(i, 0)->text() == "RSI" ||
                            ui->result_table->item(i, 0)->text() == "RMSt" ||
                            ui->result_table->item(i, 0)->text() == "RMSa" ||
                            ui->result_table->item(i, 0)->text() == "RMSi" ) {
                    ui->result_table->item(i, 2)->setText(QString("fr@%1").arg(GlobalValue::par_i_w));
                } else if ( ui->result_table->item(i, 0)->text() == "Fringes" ) {
                    ui->result_table->item(i, 2)->setText("fr");
                } else if ( ui->result_table->item(i, 0)->text() == "Parallel_Theta" ) {
                    ui->result_table->item(i, 2)->setText("sec");
                } else if ( ui->result_table->item(i, 0)->text() == "Aperture" ||
                            ui->result_table->item(i, 0)->text() == "SizeX" ||
                            ui->result_table->item(i, 0)->text() == "SizeY" ||
                            ui->result_table->item(i, 0)->text() == "Curvature_Radius" ) {
                    ui->result_table->item(i, 2)->setText("mm");
                } else if ( ui->result_table->item(i, 0)->text() == "Strehl_Ratio" ||
                            ui->result_table->item(i, 0)->text() == "Concavity" ) {
                    ui->result_table->item(i, 2)->setText("/");
                } else if ( ui->result_table->item(i, 0)->text() == "GRMS" ) {
                    ui->result_table->item(i, 2)->setText(unit + "/mm");
                }
                else {
                    ui->result_table->item(i, 2)->setText(unit);
                }
            }

            for ( int i = 0; i < ui->zernike_table->rowCount(); ++i )
            {
                ui->zernike_table->item(i, 2)->setText(unit);
            }
        });

        dialog->exec();
        delete dialog;
    } break;
    case 9: {
        Result_setting_dialog *dialog = new Result_setting_dialog(GlobalString::action_setting, GlobalString::contextMenu_sure,
                                                                  m_algParameter, m_stateList, this);

        connect(dialog, &Result_setting_dialog::changeValue, [&](QStringList list){
            for ( int i = 0; i < m_algParameter.size(); ++i )
            {
                m_stateList.insert(m_algParameter.at(i), 0);
            }

            for ( int i = 0; i < list.size(); ++i )
            {
                m_stateList.insert(list.at(i), 1);
            }

            createResultTable();
            createDataTable();
        });

        dialog->exec();
        delete dialog;
    } break;
    case 10: {
        Limit_setting_dialog *dialog = new Limit_setting_dialog(GlobalString::action_setting, GlobalString::contextMenu_sure,
                                                                m_algParameter, m_stateList, m_limitList, this);

        connect(dialog, &Limit_setting_dialog::changeValue, [&](QStringList list, QList<double> minList, QList<double> maxList){
            for ( int i = 0; i < list.size(); ++i )
            {
                m_limitList.insert( list.at(i) + "_min", minList.at(i) );
                m_limitList.insert( list.at(i) + "_max", maxList.at(i) );
            }
        });

        dialog->exec();
        delete dialog;
    } break;
    case 11: {
        QString title = GlobalValue::lgn_tp == 1 ? QStringLiteral("切割分析") : "Slice Analysis";

        cv::Mat mat = m_algorithm->fittingSurface_fillSpikes.clone();

        Slice_analysis_Dialog *dialog = new Slice_analysis_Dialog(title, mat, m_algorithm->fittingSurface_all,
                                                                  m_algorithm->configParams, this);
        dialog->exec();
        delete dialog;

    } break;
    case 12: {
    } break;
    case 13: {
    } break;
    case 14: {
    } break;
    default: break;
    }
}

//******************************************************************************************************************
// [4] 调用算法

void MainWindow::startCalculation()
{
    m_log.write(GlobalFun::getCurrentTime(2) + " - Start calculation");

    // modify config
    modifyConfig();
    m_log.write(GlobalFun::getCurrentTime(2) + " - Modify config finished");

    // init mask
    m_manualMask = cv::Mat();
    m_rect = cv::Rect();
    m_fillMask = cv::Mat();
    m_edgeMask = cv::Mat();

    INPUT_DATAS input;
    if ( !GlobalFun::judVecAvailable(m_matList) ) {
        qDebug() << "Image and mask are all empty !";
        int picSize = GlobalValue::par_psi < 3 ? 5 : 9;
        input.oriPhase.clear();
        input.oriPhase.reserve(picSize);
        for ( int i = 0; i < picSize; ++i )
        {
            input.oriPhase.push_back(cv::Mat());
        }
        input.mask = cv::Mat();
    } else {
        if ( isSirius ) {
            input.oriPhase = getMatList();
            m_log.write(GlobalFun::getCurrentTime(2) + " - Get matList finished");

            input.mask = getMask(input.oriPhase.at(0));
            m_log.write(GlobalFun::getCurrentTime(2) + " - Get mask finished");
        } else {
            input.oriPhase.clear();
            input.oriPhase.push_back(m_phase.clone());

            input.mask = getMask(input.oriPhase.at(0));
        }
    }

    // algorithm process
    m_algorithm->process(input);
    m_log.write(GlobalFun::getCurrentTime(2) + " - Algorithm process finished");

    if ( GlobalValue::com_tp == 2 ) {
        m_fillMask = m_algorithm->retMask;
        m_edgeMask = m_algorithm->maskEdge;
    }

    m_log.write(GlobalFun::getCurrentTime(2) + " - Calculation finished");

    // 相位计算中保存掩膜
    m_mask = input.mask.clone();
    calc_type = 7;

    // fill data
    fillData();
}

void MainWindow::modifyConfig()
{
    m_algorithm->configParams.debugGrade = 0;
    m_algorithm->configParams.isPhaseAverage = false;
    m_algorithm->configParams.isASCFile = !isSirius;
    m_algorithm->configParams.checkFlag = GlobalValue::par_psi == 5;
    m_algorithm->configParams.removeErrorFlag = false;
    m_algorithm->configParams.removeResidualFlag = GlobalValue::par_rm_rsl == 1;
    m_algorithm->configParams.isSingleHole = true;
    m_algorithm->configParams.isFillSpikes = true;
    m_algorithm->configParams.zernikeTerm = 37;
    m_algorithm->configParams.residuesNumThresh = GlobalValue::com_rnt;
    m_algorithm->configParams.scaleFactor = GlobalValue::par_i_s_f;
    m_algorithm->configParams.highPassFilterCoef = GlobalValue::par_hp_fc;
    m_algorithm->configParams.holeType = HOLE_TYPE_CG;

    if ( GlobalValue::com_tp == 2 && m_scene->getGraphicsItemList().size() > 0 ) {
        m_algorithm->configParams.isEdgeEroding = m_scene->getGraphicsItemList()[0]->getAutoType() ==
                                                    BGraphicsItem::AutoType::Auto_Recognise ? true : false;
        m_algorithm->configParams.edge_erode_r = m_scene->getGraphicsItemList()[0]->getIndentPixel();
    } else {
        m_algorithm->configParams.isEdgeEroding = false;
        m_algorithm->configParams.edge_erode_r = 0;
    }

    m_algorithm->configParams.checkThr.phaseShiftThr = 10;
    m_algorithm->configParams.checkThr.stdPhaseHistThr = 0.025f;
    m_algorithm->configParams.checkThr.resPvThr = 0;
    m_algorithm->configParams.checkThr.resRmsThr = 0;

    m_algorithm->configParams.psiMethod = (PSI_METHOD)GlobalValue::par_psi;
    m_algorithm->configParams.unwrapMethod = (UNWRAP_METHOD)GlobalValue::par_unw;
    m_algorithm->configParams.filterParams.filterType = (FILTER_TYPE)GlobalValue::par_flt;
    m_algorithm->configParams.filterParams.filterWindowSize = GlobalValue::par_fws;
    m_algorithm->configParams.filterParams.removeSpikesParams.rsFlag = GlobalValue::par_rm_spk == 1;
    m_algorithm->configParams.filterParams.removeSpikesParams.removeSize = GlobalValue::par_srs;
    m_algorithm->configParams.filterParams.removeSpikesParams.slopeSize = 3;
    m_algorithm->configParams.filterParams.removeSpikesParams.rsThreshCoef = GlobalValue::par_sth;

    if ( GlobalValue::com_tp == 1 ) {
        if ( m_scene->getGraphicsItemList().size() == 1 &&
             m_scene->getGraphicsItemList().at(0)->getType() == BGraphicsItem::ItemType::Circle ) {
            m_algorithm->configParams.zernikeMethod = ZERNIKE_METHOD_CIRCLE;
        } else {
            m_algorithm->configParams.zernikeMethod = ZERNIKE_METHOD_ORTHO;
        }

        m_algorithm->configParams.edgeDetecParams.isUseScale = false;
        m_algorithm->configParams.edgeDetecParams.detecScale = 1;
        m_algorithm->configParams.edgeDetecParams.maskShape = MASK_SHAPE_MANUAL;
    } else {
        BGraphicsItem *item = m_scene->getGraphicsItemList().at(0);

        if ( item->getAutoType() == BGraphicsItem::AutoType::Auto_Circle ) {
            m_algorithm->configParams.zernikeMethod = ZERNIKE_METHOD_CIRCLE;
        } else {
            m_algorithm->configParams.zernikeMethod = ZERNIKE_METHOD_ORTHO;
        }

        m_algorithm->configParams.edgeDetecParams.isUseScale = item->getIfUseScale();
        m_algorithm->configParams.edgeDetecParams.detecScale = item->getAutoScale();

        switch ( item->getAutoType() )
        {
        case BGraphicsItem::AutoType::Auto_Circle: {
            m_algorithm->configParams.edgeDetecParams.maskShape = MASK_SHAPE_CIRCLE;
            m_algorithm->configParams.edgeDetecParams.rectSize = cv::Size2f(0, 0);
            m_algorithm->configParams.edgeDetecParams.inputRadius = item->getAutoCirRadius()/2;
        } break;
        case BGraphicsItem::AutoType::Auto_Ellipse: {
            m_algorithm->configParams.edgeDetecParams.maskShape = MASK_SHAPE_ELLIPSE;
            m_algorithm->configParams.edgeDetecParams.rectSize.width = item->getAutoWidth();
            m_algorithm->configParams.edgeDetecParams.rectSize.height = item->getAutoHeight();
            m_algorithm->configParams.edgeDetecParams.inputRadius = 0;
        } break;
        case BGraphicsItem::AutoType::Auto_Pill: {
            m_algorithm->configParams.edgeDetecParams.maskShape = MASK_SHAPE_PILL;
            m_algorithm->configParams.edgeDetecParams.rectSize.width = item->getAutoWidth();
            m_algorithm->configParams.edgeDetecParams.rectSize.height = item->getAutoHeight();
            m_algorithm->configParams.edgeDetecParams.inputRadius = 0;
        } break;
        case BGraphicsItem::AutoType::Auto_Chamfer: {
            m_algorithm->configParams.edgeDetecParams.maskShape = MASK_SHAPE_RECT_CHAMFER;
            m_algorithm->configParams.edgeDetecParams.rectSize.width = item->getAutoWidth();
            m_algorithm->configParams.edgeDetecParams.rectSize.height = item->getAutoHeight();
            m_algorithm->configParams.edgeDetecParams.inputRadius = item->getAutoChaRadius();
        } break;
        case BGraphicsItem::AutoType::Auto_RotateRec: {
            m_algorithm->configParams.edgeDetecParams.maskShape = MASK_SHAPE_ROTATED_RECT;
            m_algorithm->configParams.edgeDetecParams.rectSize.width = item->getAutoWidth();
            m_algorithm->configParams.edgeDetecParams.rectSize.height = item->getAutoHeight();
            m_algorithm->configParams.edgeDetecParams.inputRadius = 0;
        } break;
        case BGraphicsItem::AutoType::Auto_RoundEdgeRec: {
            m_algorithm->configParams.edgeDetecParams.maskShape = MASK_SHAPE_CIRCLE_RECT;
            m_algorithm->configParams.edgeDetecParams.rectSize.width = item->getAutoDis1();
            m_algorithm->configParams.edgeDetecParams.rectSize.height = item->getAutoDis2();
            m_algorithm->configParams.edgeDetecParams.inputRadius = item->getAutoCirRadius()/2;
        } break;
        case BGraphicsItem::AutoType::Auto_Recognise: {
            m_algorithm->configParams.edgeDetecParams.maskShape = MASK_SHAPE_AUTO;
        } break;
        default: {
            m_algorithm->configParams.edgeDetecParams.maskShape = MASK_SHAPE_CIRCLE;
            m_algorithm->configParams.edgeDetecParams.rectSize = cv::Size2f(0, 0);
            m_algorithm->configParams.edgeDetecParams.inputRadius = item->getAutoCirRadius()/2;
        } break;
        }
    }

    m_algorithm->configParams.removeZernikeFlags.positionFlag = GlobalValue::zer_pis == 1;
    m_algorithm->configParams.removeZernikeFlags.tiltFlag = GlobalValue::zer_tilt == 1;
    m_algorithm->configParams.removeZernikeFlags.powerFlag = GlobalValue::zer_pow == 1;
    m_algorithm->configParams.removeZernikeFlags.astFlag = GlobalValue::zer_ast == 1;
    m_algorithm->configParams.removeZernikeFlags.comaFlag = GlobalValue::zer_coma == 1;
    m_algorithm->configParams.removeZernikeFlags.sphericalFlag = GlobalValue::zer_sph == 1;

    m_algorithm->configParams.calcTotalResultFlags.pvRmsResultFlags.pvFlag = m_stateList.value("PV") == 1;
    m_algorithm->configParams.calcTotalResultFlags.pvRmsResultFlags.pvxFlag = m_stateList.value("PV(x)") == 1;
    m_algorithm->configParams.calcTotalResultFlags.pvRmsResultFlags.pvyFlag = m_stateList.value("PV(y)") == 1;
    m_algorithm->configParams.calcTotalResultFlags.pvRmsResultFlags.pvxyFlag = m_stateList.value("PV(xy)") == 1;
    m_algorithm->configParams.calcTotalResultFlags.pvRmsResultFlags.pvrFlag = m_stateList.value("PVr") == 1;
    m_algorithm->configParams.calcTotalResultFlags.pvRmsResultFlags.pvresFlag = m_stateList.value("PV(res)") == 1;

    m_algorithm->configParams.calcTotalResultFlags.pvRmsResultFlags.rmsFlag = m_stateList.value("RMS") == 1;
    m_algorithm->configParams.calcTotalResultFlags.pvRmsResultFlags.rmsxFlag = m_stateList.value("RMS(x)") == 1;
    m_algorithm->configParams.calcTotalResultFlags.pvRmsResultFlags.rmsyFlag = m_stateList.value("RMS(y)") == 1;
    m_algorithm->configParams.calcTotalResultFlags.pvRmsResultFlags.rmsxyFlag = m_stateList.value("RMS(xy)") == 1;
    m_algorithm->configParams.calcTotalResultFlags.pvRmsResultFlags.rmsresFlag = m_stateList.value("RMS(res)") == 1;

    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.zernikeTiltFlag = m_stateList.value("Zernike_Tilt") == 1;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.zernikePowerFlag = m_stateList.value("Power") == 1;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.zernikePowerXFlag = m_stateList.value("Power_X") == 1;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.zernikePowerYFlag = m_stateList.value("Power_Y") == 1;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.zernikeAstFlag = m_stateList.value("Zernike_Ast") == 1;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.zernikeComaFlag = m_stateList.value("Zernike_Coma") == 1;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.zernikeSphericalFlag = m_stateList.value("Zernike_SA") == 1;

    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.seidelTiltFlag = true;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.seidelFocusFlag = true;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.seidelAstFlag = true;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.seidelComaFlag = true;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.seidelSphericalFlag = true;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.seidelTiltClockFlag = true;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.seidelAstClockFlag = true;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.seidelComaClockFlag = true;

    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.rmsPowerFlag = m_stateList.value("RMS(Power)") == 1;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.rmsAstFlag = m_stateList.value("RMS(Ast)") == 1;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.rmsComaFlag = m_stateList.value("RMS(Coma)") == 1;
    m_algorithm->configParams.calcTotalResultFlags.zerSeiResultFlags.rmsSaFlag = m_stateList.value("RMS(SA)") == 1;

    m_algorithm->configParams.calcTotalResultFlags.isoResultFlags.sagFlag = m_stateList.value("SAG") == 1;
    m_algorithm->configParams.calcTotalResultFlags.isoResultFlags.irrFlag = m_stateList.value("IRR") == 1;
    m_algorithm->configParams.calcTotalResultFlags.isoResultFlags.rsiFlag = m_stateList.value("RSI") == 1;
    m_algorithm->configParams.calcTotalResultFlags.isoResultFlags.rmstFlag = m_stateList.value("RMSt") == 1;
    m_algorithm->configParams.calcTotalResultFlags.isoResultFlags.rmsaFlag = m_stateList.value("RMSa") == 1;
    m_algorithm->configParams.calcTotalResultFlags.isoResultFlags.rmsiFlag = m_stateList.value("RMSi") == 1;

    m_algorithm->configParams.calcTotalResultFlags.ttvFlag = m_stateList.value("TTV") == 1;
    m_algorithm->configParams.calcTotalResultFlags.fringesFlag = m_stateList.value("Fringes") == 1;
    m_algorithm->configParams.calcTotalResultFlags.strehlFlag = m_stateList.value("Strehl_Ratio") == 1;
    m_algorithm->configParams.calcTotalResultFlags.parallelThetaFlag = m_stateList.value("Parallel_Theta") == 1;
    m_algorithm->configParams.calcTotalResultFlags.apertureFlag = true;
    m_algorithm->configParams.calcTotalResultFlags.sizeXFlag = true;
    m_algorithm->configParams.calcTotalResultFlags.sizeYFlag = true;
    m_algorithm->configParams.calcTotalResultFlags.grmsFlag = m_stateList.value("GRMS") == 1;
    m_algorithm->configParams.calcTotalResultFlags.concavityFlag = m_stateList.value("Concavity") == 1;

    m_algorithm->configParams.calcResultInputParams.px_1mm = GlobalValue::gra_pixel;
    m_algorithm->configParams.calcResultInputParams.refractiveIndex = GlobalValue::par_ref_index;
    m_algorithm->configParams.calcResultInputParams.testWavelength = GlobalValue::par_t_w;
    m_algorithm->configParams.calcResultInputParams.ISOWavelength = GlobalValue::par_i_w;
    m_algorithm->configParams.calcResultInputParams.disPlayWavelength = GlobalValue::com_value;
    m_algorithm->configParams.calcResultInputParams.scaleFactorForHoleType = 1;
    m_algorithm->configParams.calcResultInputParams.unitType = (UNIT_TYPE)(GlobalValue::com_unit);
}

std::vector<cv::Mat> MainWindow::getMatList()
{
    /*
     * 手动和自动传的都是矩形框的大小
     * 自动：场景中的矩形框
     * 手动：boundingRect + 扩充5个像素；如果是多边形，找出4个点再扩充5个像素
     *
     * 注意：图像与矩形框的与操作，如果矩形框超出图像了，与出来的结果是矩形框没有超出图像的那部分
     */
    std::vector<cv::Mat> ret;
    int offset = 0;
    if ( GlobalValue::com_tp == 1 ) { offset = 5; }

    if ( GlobalValue::com_tp == 1 )
    {
        QList<QPointF> totalList;
        for ( int i = 0; i < m_scene->getGraphicsItemList().size(); ++i )
        {
            if ( m_scene->getGraphicsItemList().at(i)->getType() == BGraphicsItem::ItemType::Polygon )
            {
                qreal minX = 10000, maxX = -10000, minY = 10000, maxY = -10000;
                QList<QPointF> list = m_scene->getGraphicsItemList().at(i)->getPointList();
                for (auto &temp : list)
                {
                    if ( temp.x() < minX ) { minX = temp.x(); }
                    if ( temp.x() > maxX ) { maxX = temp.x(); }
                    if ( temp.y() < minY ) { minY = temp.y(); }
                    if ( temp.y() > maxY ) { maxY = temp.y(); }
                }

                QPointF minP = m_scene->getGraphicsItemList().at(i)->mapToScene(QPointF(minX, minY));
                QPointF maxP = m_scene->getGraphicsItemList().at(i)->mapToScene(QPointF(maxX, maxY));
                totalList.append(minP);
                totalList.append(maxP);
            } else {
                QPolygonF polygon = m_scene->getGraphicsItemList().at(i)->mapToScene(m_scene->getGraphicsItemList().at(i)->boundingRect());
                totalList.append( polygon.boundingRect().topLeft() );
                totalList.append( polygon.boundingRect().bottomRight() );
            }
        }

        qreal minX = 10000, maxX = -10000, minY = 10000, maxY = -10000;
        for (auto &temp : totalList)
        {
            if ( temp.x() < minX ) { minX = temp.x(); }
            if ( temp.x() > maxX ) { maxX = temp.x(); }
            if ( temp.y() < minY ) { minY = temp.y(); }
            if ( temp.y() > maxY ) { maxY = temp.y(); }
        }

        QPointF minP = QPointF(minX, minY);
        QPointF maxP = QPointF(maxX, maxY);
        qreal x = minP.x() - offset;
        qreal y = minP.y() - offset;
        qreal w = maxP.x() - minP.x() + offset * 2;
        qreal h = maxP.y() - minP.y() + offset * 2;
        m_rect = cv::Rect(x + m_matList.at(0).cols/2, y + m_matList.at(0).rows/2, w, h) &
                 cv::Rect(0, 0, m_matList.at(0).cols, m_matList.at(0).rows);

        ret = GlobalFun::cvtBGR2GRAY(m_matList, m_rect, true);
        return ret;
    }
    else
    {
        QPolygonF polygon = m_scene->getGraphicsItemList().at(0)->mapToScene(m_scene->getGraphicsItemList().at(0)->boundingRect());
        qreal x = polygon.boundingRect().x() - offset;
        qreal y = polygon.boundingRect().y() - offset;
        qreal w = polygon.boundingRect().width() + offset * 2;
        qreal h = polygon.boundingRect().height() + offset * 2;
        m_rect = cv::Rect(x + m_matList.at(0).cols/2, y + m_matList.at(0).rows/2, w, h) &
                 cv::Rect(0, 0, m_matList.at(0).cols, m_matList.at(0).rows);

        ret = GlobalFun::cvtBGR2GRAY(m_matList, m_rect, true);
        return ret;
    }
}

cv::Mat MainWindow::getMask(cv::Mat image)
{
    /*
     * 手动：用上面的矩形框截取实际的图像大小，没有图像也是全0矩阵
     * 自动：全0矩阵
     */
    if ( GlobalValue::com_tp == 1 ) {
        m_manualMask = cv::Mat(m_matList.at(0).rows, m_matList.at(0).cols, CV_8UC1, cv::Scalar(0));

        for ( int i = 0; i < m_scene->getGraphicsItemList().size(); ++i )
        {
            if ( m_scene->getGraphicsItemList().at(i)->getIfDottedLine() ) {
                continue;
            }

            QPolygonF polygon = m_scene->getGraphicsItemList().at(i)->mapToScene(m_scene->getGraphicsItemList().at(i)->boundingRect());
            qreal centerX = polygon.boundingRect().center().x();
            qreal centerY = polygon.boundingRect().center().y();

            qreal width = m_scene->getGraphicsItemList().at(i)->getWidth();
            qreal height = m_scene->getGraphicsItemList().at(i)->getHeight();
            qreal radius = m_scene->getGraphicsItemList().at(i)->getRadius();
            qreal rotation = m_scene->getGraphicsItemList().at(i)->getRotation();
            BGraphicsItem::ItemType type = m_scene->getGraphicsItemList().at(i)->getType();

            cv::Point2i point1(centerX + m_manualMask.cols/2, centerY + m_manualMask.rows/2);
            cv::Point2f point2(centerX + m_manualMask.cols/2, centerY + m_manualMask.rows/2);
            cv::Size2f size(width, height);
            cv::RotatedRect rect(point2, size, rotation);

            switch (type)
            {
            case BGraphicsItem::ItemType::Circle: { calc::DrawCircle(m_manualMask, point1, width/2, 255, -1); } break;
            case BGraphicsItem::ItemType::Ellipse: { calc::DrawEllipse(m_manualMask, rect, 255, -1); } break;
            case BGraphicsItem::ItemType::Concentric_Circle: {
                qreal max = width > radius ? width : radius;
                qreal min = width < radius ? width : radius;
                calc::DrawCircle(m_manualMask, point1, max/2, 255, -1);
                calc::DrawCircle(m_manualMask, point1, min/2, 0, -1);
            } break;
            case BGraphicsItem::ItemType::Rectangle: { calc::DrawRotatedRect(m_manualMask, rect, 255, -1); } break;
            case BGraphicsItem::ItemType::Square: { calc::DrawRotatedRect(m_manualMask, rect, 255, -1); } break;
            case BGraphicsItem::ItemType::Polygon: {
                std::vector<cv::Point> vec;
                QList<QPointF> list = m_scene->getGraphicsItemList().at(i)->getPointList();
                for (int j = 0; j < list.size() - 1; ++j)
                {
                    QPointF point = m_scene->getGraphicsItemList().at(i)->mapToScene(list.at(j));
                    vec.push_back(cv::Point(point.x() + m_manualMask.cols/2, point.y() + m_manualMask.rows/2));
                }
                calc::DrawPolygon(m_manualMask, vec, 255, -1);
            } break;
            case BGraphicsItem::ItemType::Pill: { calc::DrawPill(m_manualMask, rect, 255, true, -1); } break;
            case BGraphicsItem::ItemType::Chamfer: { calc::DrawRotatedRectChamfer(m_manualMask, rect, radius, 255, -1); } break;
            default: break;
            }
        }

        for ( int i = 0; i < m_scene->getGraphicsItemList().size(); ++i )
        {
            if ( !m_scene->getGraphicsItemList().at(i)->getIfDottedLine() ) {
                continue;
            }

            QPolygonF polygon = m_scene->getGraphicsItemList().at(i)->mapToScene(m_scene->getGraphicsItemList().at(i)->boundingRect());
            qreal centerX = polygon.boundingRect().center().x();
            qreal centerY = polygon.boundingRect().center().y();

            qreal width = m_scene->getGraphicsItemList().at(i)->getWidth();
            qreal height = m_scene->getGraphicsItemList().at(i)->getHeight();
            qreal radius = m_scene->getGraphicsItemList().at(i)->getRadius();
            qreal rotation = m_scene->getGraphicsItemList().at(i)->getRotation();
            BGraphicsItem::ItemType type = m_scene->getGraphicsItemList().at(i)->getType();

            cv::Point2i point1(centerX + m_manualMask.cols/2, centerY + m_manualMask.rows/2);
            cv::Point2f point2(centerX + m_manualMask.cols/2, centerY + m_manualMask.rows/2);
            cv::Size2f size(width, height);
            cv::RotatedRect rect(point2, size, rotation);

            switch (type)
            {
            case BGraphicsItem::ItemType::Circle: { calc::DrawCircle(m_manualMask, point1, width/2, 0, -1); } break;
            case BGraphicsItem::ItemType::Ellipse: { calc::DrawEllipse(m_manualMask, rect, 0, -1); } break;
            case BGraphicsItem::ItemType::Concentric_Circle: {
                qreal max = width > radius ? width : radius;
                qreal min = width < radius ? width : radius;
                calc::DrawCircle(m_manualMask, point1, max/2, 0, -1);
                calc::DrawCircle(m_manualMask, point1, min/2, 0, -1);
            } break;
            case BGraphicsItem::ItemType::Rectangle: { calc::DrawRotatedRect(m_manualMask, rect, 0, -1); } break;
            case BGraphicsItem::ItemType::Square: { calc::DrawRotatedRect(m_manualMask, rect, 0, -1); } break;
            case BGraphicsItem::ItemType::Polygon: {
                std::vector<cv::Point> vec;
                QList<QPointF> list = m_scene->getGraphicsItemList().at(i)->getPointList();
                for (int j = 0; j < list.size() - 1; ++j)
                {
                    QPointF point = m_scene->getGraphicsItemList().at(i)->mapToScene(list.at(j));
                    vec.push_back(cv::Point(point.x() + m_manualMask.cols/2, point.y() + m_manualMask.rows/2));
                }
                calc::DrawPolygon(m_manualMask, vec, 0, -1);
            } break;
            case BGraphicsItem::ItemType::Pill: { calc::DrawPill(m_manualMask, rect, 0, true, -1); } break;
            case BGraphicsItem::ItemType::Chamfer: { calc::DrawRotatedRectChamfer(m_manualMask, rect, radius, 0, -1); } break;
            default: break;
            }
        }

        if ( isSirius ) {
            return m_manualMask(m_rect);
        } else {
            // 分析asc文件时，如果掩膜为空则分析的是文件中存储的掩膜的大小
            return m_manualMask;
        }
    } else {
        return cv::Mat(image.rows, image.cols, CV_8UC1, cv::Scalar(0));
    }
}

//******************************************************************************************************************
// [5] 调用算法计算后填充数据

void MainWindow::fillData()
{
    m_log.write(GlobalFun::getCurrentTime(2) + " - Start fill Data");

    // data_table 当前行数
    int dRow = ui->data_table->rowCount();
    ui->data_table->setRowCount(dRow+1);

    // 根据保留几位小数，确定变量的值
    float decimal = 5 / pow(10, GlobalValue::com_dcl+1);
    int divisor = pow(10, GlobalValue::com_dcl);

    //------------------------------------------------------------------------------

    QApplication::processEvents();

    // ProductID
    QTableWidgetItem *item = new QTableWidgetItem("");
    ui->data_table->setItem(dRow, 0, item);

    // 口径
    qreal aperture = 0;
    if ( true ) {
        if ( GlobalValue::com_tp == 1 && m_scene->getGraphicsItemList().size() == 1 &&
             m_scene->getGraphicsItemList().at(0)->getType() == BGraphicsItem::ItemType::Circle )
        {
            aperture = m_scene->getGraphicsItemList().at(0)->getWidth() / GlobalValue::gra_pixel;
            aperture = floor((aperture + decimal) * divisor) / divisor;
        }
        else if ( GlobalValue::com_tp == 2 &&
                  m_scene->getGraphicsItemList().at(0)->getAutoType() == BGraphicsItem::AutoType::Auto_Circle &&
                  !m_scene->getGraphicsItemList().at(0)->getIfUseScale() )
        {
            aperture = m_scene->getGraphicsItemList().at(0)->getAutoCirRadius() / GlobalValue::gra_pixel;
            aperture = floor((aperture + decimal) * divisor) / divisor;
        }
        else
        {
            if ( m_algorithm->errorType == ERROR_TYPE_NOT_ERROR ) {
                aperture = floor((m_algorithm->totalResult.aperture + decimal) * divisor) / divisor;
            } else {
                aperture = 0;
            }
        }
    }

    // 填充表格（result、data、quality）
    if ( m_algorithm->errorType == ERROR_TYPE_NOT_ERROR )
    {
        bool state = false;
        float value = 0;

        // data
        for ( int i = 0; i < ui->result_table->rowCount(); ++i )
        {
            QString str = ui->result_table->item(i, 0)->text();
            int count = m_algParameter.indexOf(str);

            if ( count < 41 ) {
                // PV ~ Parallel_Theta (result + data)
                QString min = str + "_min";
                QString max = str + "_max";
                float min_value = m_limitList.value(min);
                float max_value = m_limitList.value(max);
                value = 0;

                switch ( count )
                {
                case 0: value = floor((m_algorithm->totalResult.pvRmsResult.pv + decimal) * divisor) / divisor; break;
                case 1: value = floor((m_algorithm->totalResult.pvRmsResult.rms + decimal) * divisor) / divisor; break;
                case 2: value = floor((m_algorithm->totalResult.pvRmsResult.pvr + decimal) * divisor) / divisor; break;
                case 3: value = floor((m_algorithm->totalResult.pvRmsResult.pv_res + decimal) * divisor) / divisor; break;
                case 4: value = floor((m_algorithm->totalResult.pvRmsResult.rms_res + decimal) * divisor) / divisor; break;
                case 5: value = floor((m_algorithm->totalResult.pvRmsResult.pv_x + decimal) * divisor) / divisor; break;
                case 6: value = floor((m_algorithm->totalResult.pvRmsResult.rms_x + decimal) * divisor) / divisor; break;
                case 7: value = floor((m_algorithm->totalResult.pvRmsResult.pv_y + decimal) * divisor) / divisor; break;
                case 8: value = floor((m_algorithm->totalResult.pvRmsResult.rms_y + decimal) * divisor) / divisor; break;
                case 9: value = floor((m_algorithm->totalResult.pvRmsResult.pv_xy + decimal) * divisor) / divisor; break;
                case 10: value = floor((m_algorithm->totalResult.pvRmsResult.rms_xy + decimal) * divisor) / divisor; break;
                case 11: value = floor((m_algorithm->totalResult.grms + decimal) * divisor) / divisor; break;

                case 12: value = floor((m_algorithm->totalResult.zerSeiResult.zernikeTilt + decimal) * divisor) / divisor; break;
                case 13: value = floor((m_algorithm->totalResult.zerSeiResult.zernikePower + decimal) * divisor) / divisor; break;
                case 14: value = floor((m_algorithm->totalResult.zerSeiResult.zernikePowerX + decimal) * divisor) / divisor; break;
                case 15: value = floor((m_algorithm->totalResult.zerSeiResult.zernikePowerY + decimal) * divisor) / divisor; break;
                case 16: value = floor((m_algorithm->totalResult.zerSeiResult.zernikeAst + decimal) * divisor) / divisor; break;
                case 17: value = floor((m_algorithm->totalResult.zerSeiResult.zernikeComa + decimal) * divisor) / divisor; break;
                case 18: value = floor((m_algorithm->totalResult.zerSeiResult.zernikeSpherical + decimal) * divisor) / divisor; break;
                case 19: value = floor((m_algorithm->totalResult.zerSeiResult.rms_power + decimal) * divisor) / divisor; break;
                case 20: value = floor((m_algorithm->totalResult.zerSeiResult.rms_ast + decimal) * divisor) / divisor; break;
                case 21: value = floor((m_algorithm->totalResult.zerSeiResult.rms_coma + decimal) * divisor) / divisor; break;
                case 22: value = floor((m_algorithm->totalResult.zerSeiResult.rms_sa + decimal) * divisor) / divisor; break;
                case 23: value = floor((m_algorithm->totalResult.zerSeiResult.seidelTilt + decimal) * divisor) / divisor; break;
                case 24: value = floor((m_algorithm->totalResult.zerSeiResult.seidelTiltClock + decimal) * divisor) / divisor; break;
                case 25: value = floor((m_algorithm->totalResult.zerSeiResult.seidelFocus + decimal) * divisor) / divisor; break;
                case 26: value = floor((m_algorithm->totalResult.zerSeiResult.seidelAst + decimal) * divisor) / divisor; break;
                case 27: value = floor((m_algorithm->totalResult.zerSeiResult.seidelAstClock + decimal) * divisor) / divisor; break;
                case 28: value = floor((m_algorithm->totalResult.zerSeiResult.seidelComa + decimal) * divisor) / divisor; break;
                case 29: value = floor((m_algorithm->totalResult.zerSeiResult.seidelComaClock + decimal) * divisor) / divisor; break;
                case 30: value = floor((m_algorithm->totalResult.zerSeiResult.seidelSpherical + decimal) * divisor) / divisor; break;

                case 31: value = floor((m_algorithm->totalResult.isoResult.sag + decimal) * divisor) / divisor; break;
                case 32: value = floor((m_algorithm->totalResult.isoResult.irr + decimal) * divisor) / divisor; break;
                case 33: value = floor((m_algorithm->totalResult.isoResult.rsi + decimal) * divisor) / divisor; break;
                case 34: value = floor((m_algorithm->totalResult.isoResult.rmst + decimal) * divisor) / divisor; break;
                case 35: value = floor((m_algorithm->totalResult.isoResult.rmsa + decimal) * divisor) / divisor; break;
                case 36: value = floor((m_algorithm->totalResult.isoResult.rmsi + decimal) * divisor) / divisor; break;

                case 37: value = floor((m_algorithm->totalResult.ttv + decimal) * divisor) / divisor; break;
                case 38: value = floor((m_algorithm->totalResult.fringes + decimal) * divisor) / divisor; break;
                case 39: value = floor((m_algorithm->totalResult.strehl + decimal) * divisor) / divisor; break;
                case 40: value = floor((m_algorithm->totalResult.parallelTheta + decimal) * divisor) / divisor; break;
                default: value = 0; break;
                }

                if ( value < min_value || value > max_value ) { state = true; }
                ui->result_table->item(i, 1)->setText(QString::number(value));

                ui->data_table->setItem(dRow, i+1, new QTableWidgetItem(QString::number(value)));
            } else {
                // Aperture ~ Z9 (result + data)
                switch ( count )
                {
                case 41: {
                    ui->result_table->item(i, 1)->setText(QString::number(aperture));
                    ui->data_table->setItem(dRow, i+1, new QTableWidgetItem(QString::number(aperture)));
                } break;
                case 42: {
                    float value = floor((m_algorithm->totalResult.sizeX + decimal) * divisor) / divisor;
                    ui->result_table->item(i, 1)->setText(QString::number(value));
                    ui->data_table->setItem(dRow, i+1, new QTableWidgetItem(QString::number(value)));
                } break;
                case 43: {
                    float value = floor((m_algorithm->totalResult.sizeY + decimal) * divisor) / divisor;
                    ui->result_table->item(i, 1)->setText(QString::number(value));
                    ui->data_table->setItem(dRow, i+1, new QTableWidgetItem(QString::number(value)));
                } break;
                case 44: {
                    float value = floor((m_algorithm->totalResult.concavity + decimal) * divisor) / divisor;
                    ui->result_table->item(i, 1)->setText(QString::number(value));
                    ui->data_table->setItem(dRow, i+1, new QTableWidgetItem(QString::number(value)));
                } break;
                case 45: {
                    ui->result_table->item(i, 1)->setText("nan");
                    ui->data_table->setItem(dRow, i+1, new QTableWidgetItem("nan"));
                } break;
                default: {
                    int index = str.right(1).toInt();
                    float value = floor((m_algorithm->zernikeCoef_9.at<float>(index-1, 0) + decimal) * divisor) / divisor;
                    ui->result_table->item(i, 1)->setText(QString::number(value));
                    ui->data_table->setItem(dRow, i+1, new QTableWidgetItem(QString::number(value)));
                } break;
                }
            }
        }

        // QC
        if ( state ) {
            // NG
            ui->data_table->setItem(dRow, ui->data_table->columnCount() - 2, new QTableWidgetItem("NG"));
            ui->quality_table->item(0, 0)->setText("NG");
            ui->quality_table->item(0, 0)->setBackgroundColor(Qt::red);
        } else {
            // OK
            ui->data_table->setItem(dRow, ui->data_table->columnCount() - 2, new QTableWidgetItem("OK"));
            ui->quality_table->item(0, 0)->setText("OK");
            ui->quality_table->item(0, 0)->setBackgroundColor(Qt::green);
        }

        QFont font(GlobalFun::getTTF(1), 36);
        font.setBold(true);
        ui->quality_table->item(0, 0)->setFont(font);
    }
    else
    {
        // data
        for ( int i = 0; i < ui->result_table->rowCount(); ++i )
        {
            ui->result_table->item(i, 1)->setText("nan");
        }

        // 不考虑 ProductID、QC 和 Date
        for ( int i = 1; i < ui->data_table->columnCount() - 2; ++i )
        {
            QString str = ui->data_table->horizontalHeaderItem(i)->text();
            if ( str == "Aperture" ) {
                ui->data_table->setItem(dRow, i, new QTableWidgetItem(QString::number(aperture)));
            } else {
                ui->data_table->setItem(dRow, i, new QTableWidgetItem("nan"));
            }
        }

        // QC
        if ( m_algorithm->errorType == ERROR_TYPE_PSI_ERROR ) {
            ui->data_table->setItem(dRow, ui->data_table->columnCount() - 2, new QTableWidgetItem("E0"));
            ui->quality_table->item(0, 0)->setText("E0");
            ui->quality_table->item(0, 0)->setBackgroundColor(Qt::yellow);

            QFont font(GlobalFun::getTTF(1), 36);
            font.setBold(true);
            ui->quality_table->item(0, 0)->setFont(font);
        } else {
            QString errStr = "";
            switch ( m_algorithm->errorType )
            {
            case ERROR_TYPE_NO_ENOUGH_IMAGES: errStr = "no enough images !"; break;
            case ERROR_TYPE_NO_MASK: errStr = "no mask !"; break;
            case ERROR_TYPE_ROI_TOO_SMALL: errStr = "roi too small !"; break;
            case ERROR_TYPE_PSI_ERROR: errStr = "psi error !"; break;
            case ERROR_TYPE_UNWRAP_ERROR: errStr = "unwrap error !"; break;
            case ERROR_TYPE_ZERNIKE_ERROR: errStr = "zernike error !"; break;
            case ERROR_TYPE_SPIKES_TOO_MUCH: errStr = "too much spikes !"; break;
            case ERROR_TYPE_NOT_AUTOMASK: errStr = "cannot use auto mask !"; break;
            case ERROR_TYPE_PIC_SIZE: errStr = "picture size error !"; break;
            case ERROR_TYPE_NO_RESULT: errStr = "no result !"; break;
            default: break;
            }

            ui->data_table->setItem(dRow, ui->data_table->columnCount() - 2, new QTableWidgetItem("ER"));
            ui->quality_table->item(0, 0)->setText(QString("ER - %1").arg(errStr));
            ui->quality_table->item(0, 0)->setBackgroundColor(Qt::gray);

            QFont font(GlobalFun::getTTF(1), 24);
            font.setBold(true);
            ui->quality_table->item(0, 0)->setFont(font);
        }
    }

    //------------------------------------------------------------------------------

    QApplication::processEvents();

    // Date
    ui->data_table->setItem(dRow, ui->data_table->columnCount() - 1, new QTableWidgetItem( GlobalFun::getCurrentTime(6) ));

    // 日期格式化
    QTime m_time;
    m_time.setHMS(0, 0, 0, 0);
    ui->data_table->horizontalHeader()->setSectionResizeMode(ui->data_table->columnCount() - 1, QHeaderView::ResizeToContents);

    // 所有项目居中显示，并且不可修改
    for ( int i = 1; i < ui->data_table->columnCount(); ++i )
    {
        ui->data_table->item(dRow, i)->setFlags(ui->data_table->item(dRow, i)->flags() & ~Qt::ItemIsEditable);
        ui->data_table->item(dRow, i)->setTextAlignment(Qt::AlignCenter);
    }

    m_log.write(GlobalFun::getCurrentTime(2) + " - Fill result_table, data_table and quality_table finished");

    //------------------------------------------------------------------------------

    QApplication::processEvents();

    // 2D
    m_2DPlotType = 3;
    change2DPlot(3);

    // 3D
    m_3DPlotType = 3;
    change3DPlot(3);

    // zernike
    if ( m_algorithm->errorType == ERROR_TYPE_NOT_ERROR ) {
        updateZernikeData(decimal, divisor, true);
    } else {
        updateZernikeData(decimal, divisor, false);
    }

    // line chart
    if ( m_algorithm->errorType == ERROR_TYPE_NOT_ERROR ) {
        updateLineChart(true);
    } else {
        updateLineChart(false);
    }

    // PSF
    updatePSFPlot();

    m_log.write(GlobalFun::getCurrentTime(2) + " - Fill 2D, 3D, Zernike, PSF and line chart finished");

    //------------------------------------------------------------------------------

    QApplication::processEvents();

    // 如果在非实时模式下则需刷新图片
    m_scene->updateImage(m_matList);

    // 修改比例
    modifyScale();

    // 绘制掩膜
    drawMask();

    // 修改按钮状态
    changeStatus(true);

    // 焦点在最后一行，光标在第一列
    ui->data_table->setCurrentItem(ui->data_table->item(dRow, 0), QItemSelectionModel::Current);
    ui->data_table->setColumnWidth(0, 120);
    ui->data_table->editItem(item);

    m_log.write(GlobalFun::getCurrentTime(2) + " - Fill data finished\n");
}

void MainWindow::change2DPlot(int type)
{
    cv::Mat mat = cv::Mat();

    switch (type) {
    case 1: mat = m_algorithm->fittingSurface.clone(); break;
    case 2: mat = m_algorithm->zernikeResidual.clone(); break;
    case 3: mat = m_algorithm->fittingSurface_fillSpikes.clone(); break;
    default: break;
    }

    if ( mat.empty() ) {
        qDebug() << "mat is empty!";
        return;
    }

    m_2DPlotType = type;

    QCustomPlot *customPlot = static_cast<QCustomPlot *>( ui->view_tab1->children().at(0) );
    if ( customPlot != nullptr )
    {
        customPlot->clearItems();
        customPlot->clearGraphs();
        customPlot->clearMask();
        customPlot->clearPlottables();
        customPlot->plotLayout()->remove( customPlot->plotLayout()->element(0, 1) );

        QCPColorGradient cpg;
        cpg.setColorStopAt(1, Qt::red);
        cpg.setColorStopAt(0.55, Qt::yellow);
        cpg.setColorStopAt(0.44, Qt::green);
        cpg.setColorStopAt(0, Qt::blue);

        QCPColorScale *colorScale = new QCPColorScale(customPlot);
        colorScale->axis()->setTicks(true);
        colorScale->axis()->setRange(0, 1);
        colorScale->setType(QCPAxis::atRight);
        colorScale->setRangeDrag(false);
        colorScale->setRangeZoom(false);
        colorScale->setBarWidth(8);

        QCPColorMap *colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);

        // 常规测量
        if ( m_algorithm->errorType == ERROR_TYPE_NOT_ERROR  ) {
            bool landscape = mat.cols >= mat.rows;
            int max = landscape ? mat.cols : mat.rows;
            colorMap->data()->setSize(max, max);
            colorMap->data()->setRange(QCPRange(0, max), QCPRange(0, max));

            double minV = 0, maxV = 0;
            cv::minMaxIdx(mat, &minV, &maxV, nullptr, nullptr, mat == mat);
            colorScale->axis()->setRange(minV, maxV);

            for ( int x = 0; x < mat.cols; ++x )
            {
                for ( int y = 0; y < mat.rows; ++y )
                {
                    float value = mat.at<float>(y, x);
                    if ( isnan(value) ) {
                        colorMap->data()->setAlpha(landscape ? x : x + (mat.rows-mat.cols)/2,
                                                   landscape ? mat.rows - 1 - y + (mat.cols-mat.rows)/2 : mat.rows - 1 - y,
                                                   0);
                    } else {
                        colorMap->data()->setCell(landscape ? x : x + (mat.rows-mat.cols)/2,
                                                  landscape ? mat.rows - 1 - y + (mat.cols-mat.rows)/2 : mat.rows - 1 - y,
                                                  value);
                    }
                }
            }

            if ( landscape ) {
                qreal minY = (mat.cols - mat.rows)/2;
                qreal maxY = (mat.cols + mat.rows)/2;
                for ( int x = 0; x < mat.cols; ++x )
                {
                    for ( int y = 0; y < minY; ++y )
                    {
                        colorMap->data()->setAlpha(x, y, 0);
                    }

                    for ( int y = maxY; y < mat.cols; ++y )
                    {
                        colorMap->data()->setAlpha(x, y, 0);
                    }
                }
            } else {
                qreal minX = (mat.rows - mat.cols)/2;
                qreal maxX = (mat.rows + mat.cols)/2;
                for ( int x = 0; x < mat.rows; ++x )
                {
                    for ( int y = 0; y < minX; ++y )
                    {
                        colorMap->data()->setAlpha(y, x, 0);
                    }

                    for ( int y = maxX; y < mat.rows; ++y )
                    {
                        colorMap->data()->setAlpha(y, x, 0);
                    }
                }
            }
        }  else {
            colorMap->data()->setSize(200, 200);
            colorMap->data()->setRange(QCPRange(0, 200), QCPRange(0, 200));
        }

        colorMap->setColorScale(colorScale);
        colorMap->setGradient(cpg);
        colorMap->rescaleDataRange(true);

        customPlot->plotLayout()->addElement(0, 1, colorScale);
        customPlot->rescaleAxes();
        customPlot->replot();
    }
}

void MainWindow::change3DPlot(int type)
{
    cv::Mat mat = cv::Mat();

    switch (type) {
    case 1: mat = m_algorithm->fittingSurface.clone(); break;
    case 2: mat = m_algorithm->zernikeResidual.clone(); break;
    case 3: mat = m_algorithm->fittingSurface_fillSpikes.clone(); break;
    default: break;
    }

    if ( mat.empty() ) {
        return;
    }

    m_3DPlotType = type;

    if ( m_algorithm->errorType == ERROR_TYPE_NOT_ERROR ) {
        double minV = 0, maxV = 0;
        cv::minMaxIdx(mat, &minV, &maxV, nullptr, nullptr, mat == mat);
        QSurfaceDataArray *array = new QSurfaceDataArray();

        for ( int x = 0; x < mat.rows; ++x )
        {
            QSurfaceDataRow *dataRow = new QSurfaceDataRow();

            for ( int y = 0; y < mat.cols; ++y )
            {
                float value = mat.at<float>(x, y);
                *dataRow << QVector3D(y, value, mat.rows - 1 - x);
            }
            *array << dataRow;
        }

        m_graph->axisX()->setRange(0, mat.cols);
        m_graph->axisY()->setRange(minV, maxV);
        m_graph->axisZ()->setRange(0, mat.rows);
        m_sProxy->resetArray(array);
    } else {
        QSurfaceDataArray *array = new QSurfaceDataArray;
        m_sProxy->resetArray(array);
    }
}

void MainWindow::updateZernikeData(float dec, int div, bool isOK)
{
    if ( isOK ) {
        if ( m_algorithm->zernikeCoef_37.empty() ) {
            return;
        }

        for ( int i = 0; i < ui->zernike_table->rowCount(); ++i )
        {
            float value = floor((m_algorithm->zernikeCoef_37.at<float>(i, 0) + dec) * div) / div;
            ui->zernike_table->item(i, 1)->setText( QString::number(value) );
        }
    } else {
        for ( int i = 0; i < ui->zernike_table->rowCount(); ++i )
        {
            ui->zernike_table->item(i, 1)->setText("nan");
        }
    }
}

void MainWindow::updateLineChart(bool isOK)
{
    float decimal = 5 / pow(10, 1+1);
    int divisor = pow(10, 1);

    // init
    xSeries->clear();
    ySeries->clear();
    psd_xSeries->clear();
    psd_ySeries->clear();
    psd_chart->removeAxis(axisXX);
    psd_chart->removeAxis(axisYY);

    for ( int i = 0; i < axisXX->categoriesLabels().size(); )
    {
        axisXX->remove(axisXX->categoriesLabels().at(i));
        axisYY->remove(axisYY->categoriesLabels().at(i));
    }

    if ( !isOK ) {
        psdShowEmpty();
    } else {
        // line chart
        if ( m_algorithm->xSlice.empty() || m_algorithm->ySlice.empty() ) {
            return;
        }

        QVector<QPointF> vec;
        for ( int i = 0; i != m_algorithm->xSlice.cols; ++i )
        {
            float value = m_algorithm->xSlice.at<float>(0, i);
            if ( !isinf(value) && !isnan(value) ) {
                vec.append(QPointF(i, value));
            }
        }
        xSeries->replace(vec);

        vec.clear();
        for ( int i = 0; i != m_algorithm->ySlice.cols; ++i )
        {
            float value = m_algorithm->ySlice.at<float>(0, i);
            if ( !isinf(value) && !isnan(value) ) {
                vec.append(QPointF(i, value));
            }
        }
        ySeries->replace(vec);

        //---------------------------------------------

        if ( m_algorithm->PSD_X.empty() || m_algorithm->PSD_Y.empty() || m_algorithm->PSD_X.cols < 2 || m_algorithm->PSD_Y.cols < 2 ) {
            return;
        }

        // 第一列代表数值，第二列代表坐标
        cv::Mat x_value = m_algorithm->PSD_X.col(0);
        cv::Mat x_axis = m_algorithm->PSD_X.col(1);
        cv::Mat y_value = m_algorithm->PSD_Y.col(0);
        cv::Mat y_axis = m_algorithm->PSD_Y.col(1);

        double x_min = 9999;
        double x_max = -9999;
        double y_min = 9999;
        double y_max = -9999;

        // 横轴 两个Mat 求最值
        //        minMaxLoc(x_axis, &x_min, &x_max, NULL, NULL);
        //        minMaxLoc(y_axis, &y_min, &y_max, NULL, NULL);

        for ( int i = 0; i < x_axis.rows; ++i )
        {
            float value = x_axis.at<float>(i, 0);
            if ( !isinf(value) && !isnan(value) ) {
                if ( value < x_min ) {
                    x_min = value;
                }
                if ( value > x_max ) {
                    x_max = value;
                }
            }
        }

        for ( int i = 0; i < y_axis.rows; ++i )
        {
            float value = y_axis.at<float>(i, 0);
            if ( !isinf(value) && !isnan(value) ) {
                if ( value < y_min ) {
                    y_min = value;
                }
                if ( value > y_max ) {
                    y_max = value;
                }
            }
        }

        double min = x_min < y_min ? x_min : y_min;
        double max = x_max > y_max ? x_max : y_max;

        axisXX->setRange(min, max);
        axisXX->setStartValue(min);
        float det = (max - min)/5.0;

        for ( int i = 0; i < 6; ++i )
        {
            float dir = floor((min + i * det + decimal) * divisor) / divisor;
            if ( dir >= 0 ) {
                axisXX->append(QString("1E+%1").arg(dir), dir);
            } else {
                axisXX->append(QString("1E%1").arg(dir), dir);
            }
        }

        //------------------------------------------------

        x_min = 9999;
        x_max = -9999;
        y_min = 9999;
        y_max = -9999;

        // 纵轴 两个Mat 求最值
        //        minMaxLoc(x_value, &x_min, &x_max, NULL, NULL);
        //        minMaxLoc(y_value, &y_min, &y_max, NULL, NULL);

        for ( int i = 0; i < x_value.rows; ++i )
        {
            float value = x_value.at<float>(i, 0);
            if ( !isinf(value) && !isnan(value) ) {
                if ( value < x_min ) {
                    x_min = value;
                }
                if ( value > x_max ) {
                    x_max = value;
                }
            }
        }

        for ( int i = 0; i < y_value.rows; ++i )
        {
            float value = y_value.at<float>(i, 0);
            if ( !isinf(value) && !isnan(value) ) {
                if ( value < y_min ) {
                    y_min = value;
                }
                if ( value > y_max ) {
                    y_max = value;
                }
            }
        }

        min = x_min < y_min ? x_min : y_min;
        max = x_max > y_max ? x_max : y_max;

        axisYY->setRange(min, max);
        axisYY->setStartValue(min);
        det = (max - min)/5.0;

        for ( int i = 0; i < 6; ++i )
        {
            float dir = floor((min + i * det + decimal) * divisor) / divisor;
            if ( dir >= 0 ) {
                axisYY->append(QString("1E+%1").arg(dir), dir);
            } else {
                axisYY->append(QString("1E%1").arg(dir), dir);
            }
        }

        //------------------------------------------------

        vec.clear();
        for ( int i = 0; i != m_algorithm->PSD_X.rows; ++i )
        {
            float y = m_algorithm->PSD_X.at<float>(i, 0);
            float x = m_algorithm->PSD_X.at<float>(i, 1);
            if ( !isinf(y) && !isnan(y) && !isinf(x) && !isnan(x)) {
                vec.append(QPointF(x, y));
            }
        }
        psd_xSeries->replace(vec);

        vec.clear();
        for ( int i = 0; i != m_algorithm->PSD_Y.rows; ++i )
        {
            float y = m_algorithm->PSD_Y.at<float>(i, 0);
            float x = m_algorithm->PSD_Y.at<float>(i, 1);
            if ( !isinf(y) && !isnan(y) && !isinf(x) && !isnan(x)) {
                vec.append(QPointF(x,y));
            }
        }
        psd_ySeries->replace(vec);
    }

    for ( auto ser : m_chart->series() )
    {
        m_chart->removeSeries(ser);
    }

    m_chart->addSeries(xSeries);
    m_chart->addSeries(ySeries);
    m_chart->createDefaultAxes();

    ui->line_chart->setChart(m_chart);

    // psd
    for ( auto ser : psd_chart->series() )
    {
        psd_chart->removeSeries(ser);
    }

    psd_chart->addAxis(axisXX, Qt::AlignBottom);
    psd_chart->addAxis(axisYY, Qt::AlignLeft);
    axisXX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisYY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    psd_chart->addSeries(psd_xSeries);
    psd_chart->addSeries(psd_ySeries);

    psd_xSeries->attachAxis(axisXX);
    psd_xSeries->attachAxis(axisYY);
    psd_ySeries->attachAxis(axisXX);
    psd_ySeries->attachAxis(axisYY);

    ui->psdXLineChart->setChart(psd_chart);
}

void MainWindow::updatePSFPlot()
{
    cv::Mat mat = m_algorithm->psf.clone();

    if ( mat.empty() ) {
        return;
    }

    if ( m_algorithm->errorType == ERROR_TYPE_NOT_ERROR ) {
        double minV = 0, maxV = 0;
        cv::minMaxIdx(mat, &minV, &maxV, nullptr, nullptr, mat == mat);
        QSurfaceDataArray *array = new QSurfaceDataArray();

        for ( int x = 0; x < mat.rows; ++x )
        {
            QSurfaceDataRow *dataRow = new QSurfaceDataRow();

            for ( int y = 0; y < mat.cols; ++y )
            {
                float value = mat.at<float>(x, y);
                *dataRow << QVector3D(y, value, mat.rows - 1 - x);
            }
            *array << dataRow;
        }

        psf_graph->axisX()->setRange(0, mat.cols);
        psf_graph->axisY()->setRange(minV, maxV);
        psf_graph->axisZ()->setRange(0, mat.rows);
        psf_proxy->resetArray(array);

        if (abs(maxV - minV) > 0.00000001) {
            mat = (mat - minV) / (maxV - minV) * 255 * 2;
        }
        mat.convertTo(mat, CV_8UC1);
        QPixmap pixmap = QPixmap::fromImage(GlobalFun::convertMatToQImage(mat));
        ui->psf_image->setPixmap(pixmap.scaled(ui->psf_image->width(), ui->psf_image->height(), Qt::KeepAspectRatio));
    } else {
        psf_proxy->resetArray(new QSurfaceDataArray);
        ui->psf_image->setPixmap(QPixmap::fromImage(QImage()));
    }
}

void MainWindow::drawMask()
{
    if ( GlobalValue::com_tp == 1 || !isSirius )
    {
        // 如果是加载asc文件，因为asc文件中存储了一个掩膜，所以需要与界面上绘制的掩膜取交集
        if ( !isSirius ) {
            m_manualMask = m_algorithm->retMask.clone();
        }

        QImage image(m_manualMask.cols, m_manualMask.rows, QImage::Format_RGBA8888);
        image.fill(QColor(0, 255, 0, 35));

        for ( int i = 0; i != m_manualMask.rows; ++i )
        {
            for ( int j = 0; j != m_manualMask.cols; ++j )
            {
                if ( m_manualMask.at<uchar>(i, j) != 0 )
                    image.setPixelColor(j, i, QColor(255, 255, 255, 0));
            }
        }

        m_scene->setMask(image);
    }
    else
    {
        if ( !GlobalFun::judVecAvailable(m_matList) || m_fillMask.empty() || m_edgeMask.empty() )
        {
            return;
        }

        cv::Mat fillMask = cv::Mat(m_matList.at(0).rows, m_matList.at(0).cols, CV_8UC1, cv::Scalar(0));
        cv::Mat edgeMask = cv::Mat(m_matList.at(0).rows, m_matList.at(0).cols, CV_8UC1, cv::Scalar(0));

        QPolygonF polygon = m_scene->getGraphicsItemList().at(0)->mapToScene(m_scene->getGraphicsItemList().at(0)->boundingRect());
        int startY = polygon.boundingRect().topLeft().y() + fillMask.rows/2;
        int endY = startY + m_fillMask.rows;
        endY = Max(endY, endY - startY);
        startY = Max(startY, 0);

        int startX = polygon.boundingRect().topLeft().x() + fillMask.cols/2;
        int endX = startX + m_fillMask.cols;
        endX = Max(endX, endX - startX);
        startX = Max(startX, 0);

        m_fillMask.copyTo(fillMask(cv::Range(startY, endY), cv::Range(startX, endX)));
        m_edgeMask.copyTo(edgeMask(cv::Range(startY, endY), cv::Range(startX, endX)));

        QImage image(fillMask.cols, fillMask.rows, QImage::Format_RGBA8888);
        image.fill(QColor(0, 255, 0, 35));

        if ( m_algorithm->errorType == ERROR_TYPE_NOT_ERROR ) {
            // 自动掩膜出错时不绘制掩膜
            for ( int i = 0; i != fillMask.rows; ++i )
            {
                for ( int j = 0; j != fillMask.cols; ++j )
                {
                    if ( fillMask.at<uchar>(i, j) != 0 )
                        image.setPixelColor(j, i, QColor(255, 255, 255, 0));

                    if ( edgeMask.at<uchar>(i, j) != 0 )
                        image.setPixelColor(j, i, QColor(255, 0, 0));
                }
            }
        }

        m_scene->setMask(image);
    }
}

void MainWindow::modifyTableHead()
{
    // modify table horizontalHeader
    QString mouth = "/" + GlobalFun::getCurrentTime(4);
    GlobalFun::isDirExist(mouth);

    QString path = mouth + "/" + GlobalFun::getCurrentTime(3) + ".csv";
    bool isFileExist = GlobalFun::isFileExist(path);

    QFile file(path);
    if ( file.open(QFile::WriteOnly | QFile::Append) ) {
        QTextStream data(&file);
        QStringList linelist;
        for (int i = 0; i != ui->data_table->columnCount(); ++i) {
            linelist.append(ui->data_table->horizontalHeaderItem(i)->text());
        }
        if ( isFileExist ) {
            data << "\n" + linelist.join(",") + "\n";
        } else {
            data << linelist.join(",") + "\n";
        }
        file.close();
    }
}

void MainWindow::psdShowEmpty()
{
    axisXX->setMin(0);
    axisXX->setMax(1);
    axisXX->setStartValue(0);
    axisYY->setMin(0);
    axisYY->setMax(1);
    axisYY->setStartValue(0);

    for ( int i = 0; i < 6; ++i )
    {
        axisXX->append(QString("1E-%1").arg(i), i*0.2);
    }

    for ( int i = 0; i < 6; ++i )
    {
        axisYY->append(QString("1E-%1").arg(i), i*0.2);
    }
}

//******************************************************************************************************************
// [12] 相位计算模块

void MainWindow::loadOneAsc()
{
    // 判断文件加载路径是否为空
    QString path = GlobalFun::getCurrentPath();
    if ( GlobalValue::file_load_path != "null" ) { path = GlobalValue::file_load_path; }

    QStringList list = QFileDialog::getOpenFileNames(nullptr, GlobalString::action_load, path,
                                                     "ASC Files (*.asc)");

    if ( list.size() == 0 ) {
        return;
    }

    if ( list.size() != 1 ) {
        GlobalFun::showMessageBox(3, "Only one file can be loaded !");
        return;
    }

    // 保存文件加载路
    QStringList list2 = list.at(0).split("/");
    QString str = "";
    for ( int i = 0; i < list2.size() - 1; ++i )
    {
        str += list2.at(i) + "/";
    }
    GlobalValue::file_load_path = str;

    loadASC(list.at(0));
}
