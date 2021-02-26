#ifndef GPDF_H
#define GPDF_H

#include <QPdfWriter>
#include <QPainter>
#include <QFile>
#include "globalfun.h"

class Gpdf
{
public:
    explicit Gpdf(const QString &file);
    ~Gpdf();

    inline int vw() { return m_painter->viewport().width(); }
    inline int vh() { return m_painter->viewport().height(); }

    void setFont(int size, bool bold = false, QString family = "default");
    void setPen(int width, QColor color = Qt::black, Qt::PenStyle style=Qt::SolidLine);

    void drawLogo(QImage image);                            // 左上角 logo
    void drawCompanyOrUrl(QString str);                     // 左下角 官网
    void drawPageNumber(int number, int count);             // 右下角 页码
    void drawTime();                                        // 右上角 时间

    void drawTable(bool type, int number, QStringList &tableHead);                             // 绘制表格 以及 表头
    void fillData(bool type, int tableNum, int start, int number, QStringList &data);          // 填充数据
    void drawMat(int start, int number, QVector<QImage> &pic);                                 // 填充切割图片
    void drawPdf(QStringList &tableHead, QStringList &tableHead2, QStringList &data, QStringList &data2, QVector<QImage> &pic);    // 整体绘制 入口

private:
    QFile *m_file;
    QPdfWriter *m_pdfWriter;
    QPainter *m_painter;
    QImage logo;
    QString companyNet;

    int cols;                   // 表格1 列数
    int cols2;                  // 表格2 列数
    double yDistance;           // 表格高度
    double xDistance;           // 表格 宽度
    int surplus;                // 表格1行数 % 35 （用于计算 表格2 起始纵坐标）
};

#endif // GPDF_H
