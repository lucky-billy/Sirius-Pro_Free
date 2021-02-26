#include "slice_analysis_dialog.h"
#include "ui_slice_analysis_dialog.h"
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QMenu>

Slice_analysis_Dialog::Slice_analysis_Dialog(QString title, cv::Mat mat, cv::Mat matAll, CONFIG_PARAMS configParams, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Slice_analysis_Dialog)
{
    ui->setupUi(this);

    // 设置窗口属性
    setWindowTitle(title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMaximumSize(this->width(), this->height());
    setMinimumSize(this->width(), this->height());

    // 初始化界面
    ui->row_spinBox->setValue(GlobalValue::ana_row);
    ui->col_spinBox->setValue(GlobalValue::ana_col);
    ui->per_spinBox->setValue(GlobalValue::ana_per);

    // 初始化实例
    algorithm = new AlgorithmMain(configParams);
    fittingSurface = mat.clone();
    fittingSurfaceAll = matAll.clone();

    // 初始化数据
    initData();

    // 创建表格
    createTable();

    // 绘制网格
    drawGrid();

    // 绑定事件
    ui->gray->installEventFilter(this);
    connect(ui->drawBtn, &QPushButton::clicked, [&](){ drawGrid(); });
    connect(ui->regionTable, &QTableWidget::currentCellChanged, [&](int row){ updateViewFromIndex(row); });
    connect(ui->regionTable, &QTableWidget::customContextMenuRequested, [&](){ createContextMenu(); });
    connect(ui->summaryTable, &QTableWidget::customContextMenuRequested, [&](){ createContextMenu(); });
}

Slice_analysis_Dialog::~Slice_analysis_Dialog()
{
    delete algorithm;
    delete ui;
}

bool Slice_analysis_Dialog::eventFilter(QObject *obj, QEvent *ev)
{
    if ( qobject_cast<QLabel*>(obj) == ui->gray )
    {
        if ( ev->type() == QEvent::MouseButtonRelease ) {
            QMouseEvent *event = static_cast<QMouseEvent *>(ev);

            if ( event->button() == Qt::LeftButton ) {
                updatePos(event->pos().x(), event->pos().y());
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        return QDialog::eventFilter(obj, ev);
    }
}

void Slice_analysis_Dialog::initData()
{
    // 获取灰度图
    cv::Mat mat = fittingSurface.clone();
    cv::Mat rgb = GlobalFun::dataProcessing(mat, false);
    cv::Mat gray;
    cvtColor(rgb, gray, cv::COLOR_BGR2GRAY);
    cv::normalize(gray, gray, 0, 255, cv::NORM_MINMAX);

    // 缩小图片到合适尺寸
    QImage image = GlobalFun::convertMatToQImage(gray);
    image_gray = image.scaled(ui->gray->width(), ui->gray->height(), Qt::KeepAspectRatio);

    // 调整显示位置
    int wl = ui->gray->width();
    int hl = ui->gray->height();
    int wp = image_gray.width();
    int hp = image_gray.height();
    ui->gray->setGeometry( 40 + (wl - wp) / 2.0, 40 + (hl - hp) / 2.0, wp, hp );
    ui->view->setGeometry( 610 + (wl - wp) / 2.0, 40 + (hl - hp) / 2.0, wp, hp );
    ui->view->setAlignment(Qt::AlignCenter);
    ui->rgb->raise();

    // 背景填充黑色
    QPixmap pixmap(ui->background->width(), ui->background->height());
    pixmap.fill(Qt::black);
    ui->background->setPixmap(pixmap);

    // 语言设置
    if ( GlobalValue::lgn_tp == 1 ) {
        ui->label_rows->setText(QStringLiteral("行数:"));
        ui->label_cols->setText(QStringLiteral("列数:"));
        ui->label_percentage->setText(QStringLiteral("百分比:"));
        ui->drawBtn->setText(QStringLiteral("绘制"));
    } else {
        ui->label_rows->setText(QStringLiteral("Rows:"));
        ui->label_cols->setText(QStringLiteral("Cols:"));
        ui->label_percentage->setText(QStringLiteral("Percentage:"));
        ui->drawBtn->setText(QStringLiteral("Draw"));
    }
}

void Slice_analysis_Dialog::createTable()
{
    // unit
    QString unit = "";
    switch (GlobalValue::com_unit) {
    case 0: unit = QStringLiteral("λ"); break;
    case 1: unit = QStringLiteral("fr"); break;
    case 2: unit = QStringLiteral("nm"); break;
    case 3: unit = QStringLiteral("μm"); break;
    case 4: unit = QStringLiteral("mm"); break;
    default: unit = QStringLiteral("nm"); break;
    }

    // column count
    int region_columnCount = 6;
    region_columnHead << "Region" << "NPoints" << QString("PV (%1)").arg(unit) << QString("RMS (%1)").arg(unit)
                      << QString("LTV (%1)").arg(unit) << QString("Mean (%1)").arg(unit);

    ui->regionTable->setColumnCount(region_columnCount);
    ui->regionTable->setHorizontalHeaderLabels(region_columnHead);

    // setting
    ui->regionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->regionTable->horizontalHeader()->setStyleSheet(GlobalString::table_style);
    ui->regionTable->horizontalHeader()->setSectionsClickable(false);
    ui->regionTable->verticalHeader()->setVisible(false);
    ui->regionTable->setAlternatingRowColors(true);
    ui->regionTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->regionTable->setFocusPolicy(Qt::StrongFocus);
    ui->regionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->regionTable->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->regionTable->setColumnWidth(0, 75);
    ui->regionTable->setColumnWidth(1, 75);
    ui->regionTable->setColumnWidth(2, 75);
    ui->regionTable->setColumnWidth(3, 75);
    ui->regionTable->setColumnWidth(4, 75);
    ui->regionTable->setColumnWidth(5, 75);

    //---------------------------------

    // column count
    int summary_columnCount = 6;
    summary_columnHead << "Statistic" << "NPoints" << QString("PV (%1)").arg(unit) << QString("RMS (%1)").arg(unit)
                      << QString("LTV (%1)").arg(unit) << QString("Mean (%1)").arg(unit);

    ui->summaryTable->setColumnCount(summary_columnCount);
    ui->summaryTable->setHorizontalHeaderLabels(summary_columnHead);

    // setting
    ui->summaryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->summaryTable->horizontalHeader()->setStyleSheet(GlobalString::table_style);
    ui->summaryTable->horizontalHeader()->setSectionsClickable(false);
    ui->summaryTable->verticalHeader()->setVisible(false);
    ui->summaryTable->setAlternatingRowColors(true);
    ui->summaryTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->summaryTable->setFocusPolicy(Qt::StrongFocus);
    ui->summaryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->summaryTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->summaryTable->setRowCount(6);

    ui->summaryTable->setColumnWidth(0, 75);
    ui->summaryTable->setColumnWidth(1, 75);
    ui->summaryTable->setColumnWidth(2, 75);
    ui->summaryTable->setColumnWidth(3, 75);
    ui->summaryTable->setColumnWidth(4, 75);
    ui->summaryTable->setColumnWidth(5, 75);

    ui->summaryTable->setItem(0, 0, new QTableWidgetItem( "Min" ));
    ui->summaryTable->setItem(1, 0, new QTableWidgetItem( "Max" ));
    ui->summaryTable->setItem(2, 0, new QTableWidgetItem( "Range" ));
    ui->summaryTable->setItem(3, 0, new QTableWidgetItem( "Mean" ));
    ui->summaryTable->setItem(4, 0, new QTableWidgetItem( "Std Dev" ));
    ui->summaryTable->setItem(5, 0, new QTableWidgetItem( "2 Sigma" ));

    for ( int i = 0; i < 6; ++i )
    {
        for ( int j = 1; j < 6; ++j )
        {
            ui->summaryTable->setItem(i, j, new QTableWidgetItem( "0" ));
        }
    }

    // 所有项目居中显示，并且不可修改
    for ( int i = 0; i < 6; ++i )
    {
        for ( int j = 0; j < 6; ++j )
        {
            ui->summaryTable->item(i, j)->setFlags(ui->summaryTable->item(i, j)->flags() & ~Qt::ItemIsEditable);
            ui->summaryTable->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void Slice_analysis_Dialog::createContextMenu()
{
    QMenu *menu = new QMenu(this);

    QAction *pdf = new QAction(QIcon(":/images/ExportPDF.png"), GlobalValue::lgn_tp == 1 ? QStringLiteral("导出PDF") : "Export PDF", menu);
    connect(pdf, &QAction::triggered, [&](){
        if ( exportPdf() ) {
            GlobalFun::showMessageBox(2, "Save PDF file successfully !");
        }
    });

    menu->addAction(pdf);
    menu->exec(QCursor::pos());
    delete menu;
}

void Slice_analysis_Dialog::drawGrid()
{
    // 计算的图像、绘制的图像
    cv::Mat mat = fittingSurface.clone();
    QImage pic = image_gray.convertToFormat(QImage::Format_RGB888);

    // 行数、列数、百分比
    int rows = ui->row_spinBox->value();
    int cols = ui->col_spinBox->value();
    int per = ui->per_spinBox->value();
    GlobalValue::ana_row = ui->row_spinBox->value();
    GlobalValue::ana_col = ui->col_spinBox->value();
    GlobalValue::ana_per = ui->per_spinBox->value();

    // 确定计算所需的间距
    int cal_wSpacing = mat.cols / cols;
    int cal_hSpacing = mat.rows / rows;
    float qw = mat.cols * 1.0f / cols - cal_wSpacing;
    float qh = mat.rows * 1.0f / rows - cal_hSpacing;

    // 确定绘图所需的间距
    qreal wSpacingF = pic.width() * 1.0f / cols;
    qreal hSpacingF = pic.height() * 1.0f / rows;

    // 画笔
    QPainter painter(&pic);
    QPen pen(Qt::white);
    pen.setWidth(2);
    painter.setPen(pen);

    int index = 0;
    int totalWidth = 0;
    int totalHeight = 0;
    int tempWidth = 0;
    int tempHeight = 0;
    float diffw = 0.0f;
    float diffh = 0.0f;

    // 清空数据
    ui->view->setVisible(false);
    ui->rgb->setVisible(false);

    ui->regionTable->clearContents();
    ui->regionTable->setRowCount(0);

    for ( int i = 0; i < 6; ++i )
    {
        for ( int j = 1; j < 6; ++j )
        {
            ui->summaryTable->item(i, j)->setText("");
        }
    }

    gridIndex.clear();
    gridMat.clear();
    gridResult.clear();
    vector.clear();

    // 根据保留几位小数，确定变量的值
    float decimal = 5 / pow(10, GlobalValue::com_dcl+1);
    int divisor = pow(10, GlobalValue::com_dcl);

    // 根据零值的占比，调用算法计算并绘制
    for ( int i = 0; i < rows; ++i )
    {
        // 确定截图高度（动态变化）
        diffh += qh;
        if ( i == rows - 1 ) {
            // 最后一行
            tempHeight = mat.rows - totalHeight;
            totalHeight = mat.rows;
        } else {
            if ( diffh >= 1.0 ) {
                tempHeight = cal_hSpacing + 1;
                diffh -= 1.0;
            } else {
                tempHeight = cal_hSpacing;
            }

            totalHeight += tempHeight;
        }

        // 初始化
        totalWidth = 0;
        diffw = 0.0;

        for ( int j = 0; j < cols; ++j )
        {
            // 确定截图宽度（动态变化）
            diffw += qw;
            if ( j == cols - 1 ) {
                // 最后一列
                tempWidth = mat.cols - totalWidth;
                totalWidth = mat.cols;
            } else {
                if ( diffw >= 1.0 ) {
                    tempWidth = cal_wSpacing + 1;
                    diffw -= 1.0;
                } else {
                    tempWidth = cal_wSpacing;
                }

                totalWidth += tempWidth;
            }

            // 截取图像
            cv::Mat part = mat(cv::Range(totalHeight-tempHeight, totalHeight), cv::Range(totalWidth-tempWidth, totalWidth));
            cv::Mat partAll = fittingSurfaceAll(cv::Range(totalHeight-tempHeight, totalHeight),
                                                cv::Range(totalWidth-tempWidth, totalWidth));
            cv::Mat dst = part == part; // 掩膜

            // 计算零值所占百分比
            std::vector<cv::Point2i> location;
            cv::findNonZero(dst, location);
            float percentage = location.size() * 1.0f / (tempWidth*tempHeight) * 100;

            if ( percentage >= per ) {
                // 超过百分比，计算 + 绘图
                cv::Mat mask = cv::Mat::zeros(part.rows, part.cols, CV_8UC1);
                mask.setTo(255, part == part);
                TOTAL_RESULT result = algorithm->ResultFromSurface(part, partAll, mask);

                // 数据填充
                cv::Mat ret = GlobalFun::dataProcessing(part, true);
                QImage white_image = GlobalFun::convertMatToQImage(ret);

                ui->regionTable->setRowCount(index+1);
                int width = ui->regionTable->columnWidth(0);
                int height = ui->regionTable->rowHeight(0) - 4;
                QImage regularImage = white_image.scaled(width, height, Qt::KeepAspectRatio);

                QLabel *label = new QLabel(ui->regionTable);
                label->setPixmap(QPixmap::fromImage(regularImage));
                label->setAlignment(Qt::AlignCenter);

                int pointnumber = result.pointnumber;
                qreal pv = floor((result.pvRmsResult.pv + decimal) * divisor) / divisor;
                qreal rms = floor((result.pvRmsResult.rms + decimal) * divisor) / divisor;
                qreal ttv = floor((result.ttv + decimal) * divisor) / divisor;
                qreal mean = floor((result.mean + decimal) * divisor) / divisor;

                ui->regionTable->setCellWidget(index, 0, label);
                ui->regionTable->setItem(index, 1, new QTableWidgetItem( QString("%1").arg(pointnumber) ));
                ui->regionTable->setItem(index, 2, new QTableWidgetItem( QString("%1").arg(pv) ));
                ui->regionTable->setItem(index, 3, new QTableWidgetItem( QString("%1").arg(rms) ));
                ui->regionTable->setItem(index, 4, new QTableWidgetItem( QString("%1").arg(ttv) ));
                ui->regionTable->setItem(index, 5, new QTableWidgetItem( QString("%1").arg(mean) ));

                // 数据保存
                ret = GlobalFun::dataProcessing(part, false);
                QImage black_image = GlobalFun::convertMatToQImage(ret);

                int value = i * 1000 + j;
                gridIndex.insert(value, index);
                gridMat.insert(value, black_image);
                gridResult.push_back(result);
                vector.push_back(white_image);
                index++;

                //---------------------------------

                painter.drawLine(QPointF(j * wSpacingF, i * hSpacingF), QPointF((j+1) * wSpacingF, i * hSpacingF));
                painter.drawLine(QPointF(j * wSpacingF, i * hSpacingF), QPointF(j * wSpacingF, (i+1) * hSpacingF));
                painter.drawLine(QPointF((j+1) * wSpacingF, i * hSpacingF), QPointF((j+1) * wSpacingF, (i+1) * hSpacingF));
                painter.drawLine(QPointF(j * wSpacingF, (i+1) * hSpacingF), QPointF((j+1) * wSpacingF, (i+1) * hSpacingF));
            }
        }
    }

    // 所有项目居中显示，并且不可修改
    for ( int i = 0; i < ui->regionTable->rowCount(); ++i )
    {
        for ( int j = 1; j < ui->regionTable->columnCount(); ++j )
        {
            ui->regionTable->item(i, j)->setFlags(ui->regionTable->item(i, j)->flags() & ~Qt::ItemIsEditable);
            ui->regionTable->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }

    // 确定选中区域
    if ( ui->regionTable->rowCount() > 0 ) {
        ui->view->setVisible(true);
        ui->rgb->setVisible(true);
        updateViewFromIndex(0);
    }

    // 统计结果
    if ( gridResult.size() > 0 ) {
        statistics();
    }

    ui->gray->setPixmap(QPixmap::fromImage(pic));
}

void Slice_analysis_Dialog::statistics()
{
    // 根据保留几位小数，确定变量的值
    float decimal = 5 / pow(10, GlobalValue::com_dcl+1);
    int divisor = pow(10, GlobalValue::com_dcl);

    // Min
    qreal nPoints_min = 99999999;
    qreal pv_min = 99999999;
    qreal rms_min = 99999999;
    qreal ltv_min = 99999999;
    qreal mean_min = 99999999;

    // Max
    qreal nPoints_max = -99999999;
    qreal pv_max = -99999999;
    qreal rms_max = -99999999;
    qreal ltv_max = -99999999;
    qreal mean_max = -99999999;

    // Range
    qreal nPoints_range = 0;
    qreal pv_range = 0;
    qreal rms_range = 0;
    qreal ltv_range = 0;
    qreal mean_range = 0;

    // Sum
    qreal nPoints_sum = 0;
    qreal pv_sum = 0;
    qreal rms_sum = 0;
    qreal ltv_sum = 0;
    qreal mean_sum = 0;

    // Vector
    std::vector<float> nPoints_vec;
    std::vector<float> pv_vec;
    std::vector<float> rms_vec;
    std::vector<float> ltv_vec;
    std::vector<float> mean_vec;

    for ( auto temp : gridResult )
    {
        if ( temp.pointnumber < nPoints_min ) {
            nPoints_min = temp.pointnumber;
        }
        if ( temp.pointnumber > nPoints_max ) {
            nPoints_max = temp.pointnumber;
        }
        nPoints_sum += temp.pointnumber;
        nPoints_vec.push_back(temp.pointnumber);

        if ( temp.pvRmsResult.pv < pv_min ) {
            pv_min = temp.pvRmsResult.pv;
        }
        if ( temp.pvRmsResult.pv > pv_max ) {
            pv_max = temp.pvRmsResult.pv;
        }
        pv_sum += temp.pvRmsResult.pv;
        pv_vec.push_back(temp.pvRmsResult.pv);

        if ( temp.pvRmsResult.rms < rms_min ) {
            rms_min = temp.pvRmsResult.rms;
        }
        if ( temp.pvRmsResult.rms > rms_max ) {
            rms_max = temp.pvRmsResult.rms;
        }
        rms_sum += temp.pvRmsResult.rms;
        rms_vec.push_back(temp.pvRmsResult.rms);

        if ( temp.ttv < ltv_min ) {
            ltv_min = temp.ttv;
        }
        if ( temp.ttv > ltv_max ) {
            ltv_max = temp.ttv;
        }
        ltv_sum += temp.ttv;
        ltv_vec.push_back(temp.ttv);

        if ( temp.mean < mean_min ) {
            mean_min = temp.mean;
        }
        if ( temp.mean > mean_max ) {
            mean_max = temp.mean;
        }
        mean_sum += temp.mean;
        mean_vec.push_back(temp.mean);
    }

    nPoints_range = nPoints_max - nPoints_min;
    pv_range = pv_max - pv_min;
    rms_range = rms_max - rms_min;
    ltv_range = ltv_max - ltv_min;
    mean_range = mean_max - mean_min;

    ui->summaryTable->item(0, 1)->setText(QString::number(nPoints_min));
    ui->summaryTable->item(0, 2)->setText(QString::number(floor((pv_min + decimal) * divisor) / divisor));
    ui->summaryTable->item(0, 3)->setText(QString::number(floor((rms_min + decimal) * divisor) / divisor));
    ui->summaryTable->item(0, 4)->setText(QString::number(floor((ltv_min + decimal) * divisor) / divisor));
    ui->summaryTable->item(0, 5)->setText(QString::number(floor((mean_min + decimal) * divisor) / divisor));

    ui->summaryTable->item(1, 1)->setText(QString::number(nPoints_max));
    ui->summaryTable->item(1, 2)->setText(QString::number(floor((pv_max + decimal) * divisor) / divisor));
    ui->summaryTable->item(1, 3)->setText(QString::number(floor((rms_max + decimal) * divisor) / divisor));
    ui->summaryTable->item(1, 4)->setText(QString::number(floor((ltv_max + decimal) * divisor) / divisor));
    ui->summaryTable->item(1, 5)->setText(QString::number(floor((mean_max + decimal) * divisor) / divisor));

    ui->summaryTable->item(2, 1)->setText(QString::number(nPoints_range));
    ui->summaryTable->item(2, 2)->setText(QString::number(floor((pv_range + decimal) * divisor) / divisor));
    ui->summaryTable->item(2, 3)->setText(QString::number(floor((rms_range + decimal) * divisor) / divisor));
    ui->summaryTable->item(2, 4)->setText(QString::number(floor((ltv_range + decimal) * divisor) / divisor));
    ui->summaryTable->item(2, 5)->setText(QString::number(floor((mean_range + decimal) * divisor) / divisor));

    //---------------------------------

    // Mean
    qreal nPoints_mean = nPoints_sum / gridResult.size();
    qreal pv_mean = pv_sum / gridResult.size();
    qreal rms_mean = rms_sum / gridResult.size();
    qreal ltv_mean = ltv_sum / gridResult.size();
    qreal mean_mean = mean_sum / gridResult.size();

    // Accumulate
    qreal nPoints_accum = 0;
    qreal pv_accum = 0;
    qreal rms_accum = 0;
    qreal ltv_accum = 0;
    qreal mean_accum = 0;

    std::for_each (std::begin(gridResult), std::end(gridResult), [&](const TOTAL_RESULT t) {
        nPoints_accum += (t.pointnumber - nPoints_mean) * (t.pointnumber - nPoints_mean);
        pv_accum += (t.pvRmsResult.pv - pv_mean) * (t.pvRmsResult.pv - pv_mean);
        rms_accum += (t.pvRmsResult.rms - rms_mean) * (t.pvRmsResult.rms - rms_mean);
        ltv_accum += (t.ttv - ltv_mean) * (t.ttv - ltv_mean);
        mean_accum += (t.mean - mean_mean) * (t.mean - mean_mean);
    });

    // Std Dev
    qreal nPoints_std = sqrt(nPoints_accum / (gridResult.size() - 1));
    qreal pv_std = sqrt(pv_accum / (gridResult.size() - 1));
    qreal rms_std = sqrt(rms_accum / (gridResult.size() - 1));
    qreal ltv_std = sqrt(ltv_accum / (gridResult.size() - 1));
    qreal mean_std = sqrt(mean_accum / (gridResult.size() - 1));

    ui->summaryTable->item(3, 1)->setText(QString::number((int)nPoints_mean));
    ui->summaryTable->item(3, 2)->setText(QString::number(floor((pv_mean + decimal) * divisor) / divisor));
    ui->summaryTable->item(3, 3)->setText(QString::number(floor((rms_mean + decimal) * divisor) / divisor));
    ui->summaryTable->item(3, 4)->setText(QString::number(floor((ltv_mean + decimal) * divisor) / divisor));
    ui->summaryTable->item(3, 5)->setText(QString::number(floor((mean_mean + decimal) * divisor) / divisor));

    ui->summaryTable->item(4, 1)->setText(gridResult.size() == 1 ? "" : QString::number((int)nPoints_std));
    ui->summaryTable->item(4, 2)->setText(gridResult.size() == 1 ? "" : QString::number(floor((pv_std + decimal) * divisor) / divisor));
    ui->summaryTable->item(4, 3)->setText(gridResult.size() == 1 ? "" : QString::number(floor((rms_std + decimal) * divisor) / divisor));
    ui->summaryTable->item(4, 4)->setText(gridResult.size() == 1 ? "" : QString::number(floor((ltv_std + decimal) * divisor) / divisor));
    ui->summaryTable->item(4, 5)->setText(gridResult.size() == 1 ? "" : QString::number(floor((mean_std + decimal) * divisor) / divisor));

    //---------------------------------

    // 2 Sigma
    qreal nPoints_sigma = BaseFunc::getTwoSigma(nPoints_vec);
    qreal pv_sigma = BaseFunc::getTwoSigma(pv_vec);
    qreal rms_sigma = BaseFunc::getTwoSigma(rms_vec);
    qreal ltv_sigma = BaseFunc::getTwoSigma(ltv_vec);
    qreal mean_sigma = BaseFunc::getTwoSigma(mean_vec);

    ui->summaryTable->item(5, 1)->setText(gridResult.size() == 1 ? "" : QString::number((int)nPoints_sigma));
    ui->summaryTable->item(5, 2)->setText(gridResult.size() == 1 ? "" : QString::number(floor((pv_sigma + decimal) * divisor) / divisor));
    ui->summaryTable->item(5, 3)->setText(gridResult.size() == 1 ? "" : QString::number(floor((rms_sigma + decimal) * divisor) / divisor));
    ui->summaryTable->item(5, 4)->setText(gridResult.size() == 1 ? "" : QString::number(floor((ltv_sigma + decimal) * divisor) / divisor));
    ui->summaryTable->item(5, 5)->setText(gridResult.size() == 1 ? "" : QString::number(floor((mean_sigma + decimal) * divisor) / divisor));
}

void Slice_analysis_Dialog::updatePos(qreal x, qreal y)
{
    // 确定在哪个矩形框
    qreal wSpacing = ui->gray->width()*1.0f / ui->col_spinBox->value();
    qreal hSpacing = ui->gray->height()*1.0f / ui->row_spinBox->value();
    int currentRow = y / hSpacing;
    int currentCol = x / wSpacing;

    // 更新数据
    updateViewFromPos(currentRow, currentCol);
}

void Slice_analysis_Dialog::updateViewFromPos(int row, int col)
{
    int key = row * 1000 + col;
    if ( gridIndex.contains(key) ) {
        int index = gridIndex.value(key);
        QImage image = gridMat.value(key);

        ui->regionTable->setFocus();
        ui->regionTable->setCurrentCell(index, QItemSelectionModel::Select);

        QImage pic = image.scaled(ui->view->width(), ui->view->height(), Qt::KeepAspectRatio);
        ui->view->setPixmap(QPixmap::fromImage(pic));

        qreal wSpacing = ui->gray->width() * 1.0f / ui->col_spinBox->value();
        qreal hSpacing = ui->gray->height() * 1.0f / ui->row_spinBox->value();
        ui->rgb->setGeometry( ui->gray->x() + col*wSpacing + 1, ui->gray->y() + row*hSpacing + 1, wSpacing - 2, hSpacing - 2 );
        QImage rgb = image.scaled(wSpacing, hSpacing, Qt::KeepAspectRatio);
        ui->rgb->setPixmap(QPixmap::fromImage(rgb));
    }
}

void Slice_analysis_Dialog::updateViewFromIndex(int index)
{
    int temp = 0;
    for ( auto key : gridIndex.keys() )
    {
        if ( gridIndex.value(key) == index ) {
            temp = key;
            break;
        }
    }

    int row = temp / 1000;
    int col = temp % 1000;
    updateViewFromPos(row, col);
}

bool Slice_analysis_Dialog::exportPdf()
{
    if ( ui->regionTable->rowCount() == 0 ) {
        GlobalFun::showMessageBox(3, "There is no data to export !");
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

    QStringList data1;

    int rows = ui->regionTable->rowCount();
    for ( int i = 0; i < rows; ++i )
    {
        for ( int j = 1; j < 6; ++j)
        {
            data1<<ui->regionTable->item(i, j)->text();
        }
    }

    QStringList data2;
    for ( int i = 0; i < 6; ++i )
    {
        for ( int j = 0; j < 6; ++j )
        {
            data2<<ui->summaryTable->item(i, j)->text();
        }
    }

    Gpdf pdf(file);
    pdf.drawLogo(QImage(":/images/Company_Logo.bmp"));
    pdf.drawCompanyOrUrl("http://www.shineoptics.com");
    pdf.drawPdf(region_columnHead, summary_columnHead, data1, data2, vector);

    return true;
}
