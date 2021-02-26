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

    void initData();                            // ��ʼ������
    void createTable();                         // �������
    void createContextMenu();                   // �Ҽ�����

    void drawGrid();                            // ��������
    void statistics();                          // ͳ������
    void updatePos(qreal x, qreal y);           // ��������
    void updateViewFromPos(int row, int col);   // ����ͼ��
    void updateViewFromIndex(int index);        // ����ͼ��
    bool exportPdf();                           // ����PDF

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    Ui::Slice_analysis_Dialog *ui;

    AlgorithmMain *algorithm;                   // �㷨ʵ��
    cv::Mat fittingSurface;                     // �������
    cv::Mat fittingSurfaceAll;                  // ������ݣ��������
    QImage image_gray;                          // �Ҷ�ͼ

    QMap<int, int> gridIndex;                   // �����б�
    QMap<int, QImage> gridMat;                  // ��Ĥ�б�
    QVector<TOTAL_RESULT> gridResult;           // ����б�
    QVector<QImage> vector;                     // ����ͼ��
    QStringList region_columnHead;
    QStringList summary_columnHead;
};

#endif // SLICE_ANALYSIS_DIALOG_H
