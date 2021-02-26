#ifndef SLICE_ANALYSIS_DIALOG_H
#define SLICE_ANALYSIS_DIALOG_H

#include <QDialog>
#include "globalfun.h"
#include "algorithmMain.h"
#include "gpdf.h"

namespace Ui {
class Slice_analysis_Dialog;
}

class Slice_analysis_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Slice_analysis_Dialog(QString title, cv::Mat mat, cv::Mat matAll, CONFIG_PARAMS configParams, QWidget *parent = nullptr);
    ~Slice_analysis_Dialog();

    void initData();                            // 初始化数据
    void createTable();                         // 创建表格
    void createContextMenu();                   // 右键弹窗

    void drawGrid();                            // 绘制网格
    void statistics();                          // 统计数据
    void updatePos(qreal x, qreal y);           // 更新区域
    void updateViewFromPos(int row, int col);   // 更新图像
    void updateViewFromIndex(int index);        // 更新图像
    bool exportPdf();                           // 导出PDF

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    Ui::Slice_analysis_Dialog *ui;

    AlgorithmMain *algorithm;                   // 算法实例
    cv::Mat fittingSurface;                     // 拟合数据
    cv::Mat fittingSurfaceAll;                  // 拟合数据（含所有项）
    QImage image_gray;                          // 灰度图

    QMap<int, int> gridIndex;                   // 索引列表
    QMap<int, QImage> gridMat;                  // 掩膜列表
    QVector<TOTAL_RESULT> gridResult;           // 结果列表
    QVector<QImage> vector;                     // 导出图像
    QStringList region_columnHead;
    QStringList summary_columnHead;
};

#endif // SLICE_ANALYSIS_DIALOG_H
