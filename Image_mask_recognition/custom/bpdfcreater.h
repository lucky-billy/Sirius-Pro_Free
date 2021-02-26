#ifndef BPDFCREATER_H
#define BPDFCREATER_H

#include <QPdfWriter>
#include <QPainter>
#include <QFile>

class BPdfCreater
{
public:
    explicit BPdfCreater(const QString &file);
    ~BPdfCreater();

    inline int vw() { return m_painter->viewport().width(); }
    inline int vh() { return m_painter->viewport().height(); }

    void setFont(int size, bool bold = false, QString family = "default");
    void setPen(int width, QColor color = Qt::black, Qt::PenStyle style=Qt::SolidLine);

    void drawTitle(QString title);                          // 标题
    void drawLogo(QImage image);                            // 左上角 logo
    void drawCompanyOrUrl(QString str);                     // 左下角 官网
    void drawWaveAndUnit();                                 // 右下角 波长和单位
    void drawCompanyName(QString str);                      // 右上角 公司名称（可修改为客户公司名称）

    void drawMat(QImage image);                             // 强度图+掩膜
    void drawPlot(QImage image);                            // 2D、3D图
    void drawProductInfo(bool state, QStringList &list);    // 产品信息
    void drawSeidelInfo(QStringList &list);                 // Seidel 信息
    void drawResultInfo(QVector<QStringList> &vec);         // Result 信息

private:
    QFile *m_file;
    QPdfWriter *m_pdfWriter;
    QPainter *m_painter;
    int height;
};

#endif // BPDFCREATER_H
