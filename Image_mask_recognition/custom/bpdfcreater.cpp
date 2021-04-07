#include "bpdfcreater.h"
#include "globalfun.h"

BPdfCreater::BPdfCreater(const QString &file)
{
    m_file = new QFile(file);
    m_file->open(QIODevice::WriteOnly);

    m_pdfWriter = new QPdfWriter(m_file);                               // 创建 PDF 写入器
    m_pdfWriter->setPageSize(QPagedPaintDevice::A4);                    // 设置纸张大小为 A4
    m_pdfWriter->setPageOrientation(QPageLayout::Landscape);            // 设置写入方向为横向
    m_pdfWriter->setResolution(300);                                    // 设置纸张的分辨率为 300, 因此其像素为 3508 x 2479

    int imargins = 40;
    m_pdfWriter->setPageMargins(QMarginsF(imargins, imargins, imargins, imargins));     // 设置页边距
    m_pdfWriter->setCreator(QStringLiteral("上海乾曜光学科技有限公司"));
    m_pdfWriter->setTitle(QStringLiteral("检测报告"));

    m_painter = new QPainter(m_pdfWriter);                              // 绘图工具
    height = 24;

    setPen(5,Qt::lightGray);
    m_painter->drawLine(QPointF(vw()/2, 0), QPointF(vw()/2, vh()));     // 绘制左侧
    m_painter->drawLine(QPointF(vw()/2, 0), QPointF(vw(), 0));          // 绘制上方
    setPen(5,Qt::darkGray);
    m_painter->drawLine(QPointF(vw()/2, vh()), QPointF(vw(), vh()));    // 绘制下方
    m_painter->drawLine(QPointF(vw(), 0), QPointF(vw(), vh()));         // 绘制上方
    setPen(5,Qt::black);
}

BPdfCreater::~BPdfCreater()
{
    m_painter->end();
    delete m_painter;

    delete m_pdfWriter;

    m_file->close();
    delete m_file;
}

void BPdfCreater::setFont(int size, bool bold, QString family)
{
    QFont font = m_painter->font();
    font.setPixelSize(size);
    font.setBold(bold);
    font.setFamily(family == "default" ? GlobalFun::getTTF(5) : family);
    m_painter->setFont(font);
}

void BPdfCreater::setPen(int width, QColor color, Qt::PenStyle style)
{
    QPen pen = m_painter->pen();
    pen.setWidth(width);
    pen.setColor(color);
    pen.setStyle(style);
    m_painter->setPen(pen);
}

void BPdfCreater::drawTitle(QString title)
{
    setFont(120);
    QRect rect(0, -140, vw(), 130);
    m_painter->drawText(rect, Qt::AlignCenter, title);
}

void BPdfCreater::drawLogo(QImage image)
{
    QRect rect(0, -140, vw(), 130);
    QImage pic = image.scaled(rect.width(), rect.height(), Qt::KeepAspectRatio);
    rect.setSize(QSize(pic.width(), pic.height()));
    m_painter->drawImage(rect, pic, pic.rect());
}

void BPdfCreater::drawCompanyOrUrl(QString str)
{
    setFont(60);
    QRect rect(0, vh() + 5, vw(), 130);
    m_painter->drawText(rect, Qt::AlignLeft, str);
}

void BPdfCreater::drawCompanyName(QString str)
{
    setFont(54, true);
    QRect rect(0, -110, vw(), 130);
    m_painter->drawText(rect, Qt::AlignRight, str);
}

void BPdfCreater::drawWaveAndUnit()
{
    QString unit = "";
    switch (GlobalValue::com_unit) {
    case 0: unit = QStringLiteral("λ") + "@" + QString::number(GlobalValue::par_r_w) + " nm"; break;
    case 1: unit = QStringLiteral("fr") + "@" + QString::number(GlobalValue::par_r_w) + " nm"; break;
    case 2: unit = QStringLiteral("nm"); break;
    case 3: unit = QStringLiteral("μm"); break;
    case 4: unit = QStringLiteral("mm"); break;
    default: unit = QStringLiteral("λ"); break;
    }

    QString str = "Test@" + QString::number(GlobalValue::par_t_w) + " nm; " +
                  "ISO@" + QString::number(GlobalValue::par_i_w) + " nm; " + "Unit: " + unit;

    setFont(60);
    QRect rect(0, vh() + 5, vw(), 130);
    m_painter->drawText(rect, Qt::AlignRight, str);
}

void BPdfCreater::drawMat(QImage image)
{
    QRect rect(40, 40, vw()/2 - 80, vh()/2 - 80);
    QImage pic = image.scaled(rect.width(), rect.height(), Qt::KeepAspectRatio);
    rect.translate( (rect.width() - pic.width()) / 2.0, (rect.height() - pic.height()) / 2.0);
    rect.setSize(QSize(pic.width(), pic.height()));
    m_painter->drawImage(rect, pic, pic.rect());
}

void BPdfCreater::drawPlot(QImage image)
{
    QRect rect(100, vh()/2 + 5, vw()/2 - 200, vh()/2 - 10);
    QImage pic = image.scaled(rect.width(), rect.height(), Qt::KeepAspectRatio);
    rect.translate( (rect.width() - pic.width()) / 2.0, (rect.height() - pic.height()) / 2.0);
    rect.setSize(QSize(pic.width(), pic.height()));
    m_painter->drawImage(rect, pic, pic.rect());
}

void BPdfCreater::drawProductInfo(bool state, QStringList &list)
{
    setPen(4, Qt::darkGray, Qt::DotLine);
    m_painter->drawRect(QRectF());
    m_painter->drawLine(QPointF(vw()/2+20, vh()/height*1), QPointF(vw()-20, vh()/height*1));

    m_painter->fillRect(QRectF(vw()/2, 0, vw()/2, vh()/height), QBrush(Qt::gray));

    setPen(5, Qt::black);
    setFont(80, true);
    m_painter->drawText(QRect(vw()/2, vh()/height*0, vw()/4, vh()/height), Qt::AlignCenter,
                        state ? QStringLiteral("产品信息"): "Product Info");

    setFont(60);
    m_painter->drawText(QRect(vw()/2, vh()/height*1, vw()/4, vh()/height), Qt::AlignCenter,
                        state ? QStringLiteral("试件名称:"): "Product Name:");
    m_painter->drawText(QRect(vw()/2, vh()/height*2, vw()/4, vh()/height), Qt::AlignCenter,
                        state ? QStringLiteral("试件编号:"): "Product ID:");
    m_painter->drawText(QRect(vw()/2, vh()/height*3, vw()/4, vh()/height), Qt::AlignCenter,
                        state ? QStringLiteral("检测人员:"): "Quality Tester:");
    m_painter->drawText(QRect(vw()/2, vh()/height*4, vw()/4, vh()/height), Qt::AlignCenter,
                        state ? QStringLiteral("日期:"): "Date:");
    m_painter->drawText(QRect(vw()/2, vh()/height*5, vw()/4, vh()/height), Qt::AlignCenter,
                        state ? QStringLiteral("设备序列号:"): "Device serial number:");
    m_painter->drawText(QRect(vw()/2, vh()/height*6, vw()/4, vh()/height), Qt::AlignCenter,
                        state ? QStringLiteral("备注:"): "Comments:");
    m_painter->drawText(QRect(vw()/2, vh()/height*7, vw()/4, vh()/height), Qt::AlignCenter,
                        state ? QStringLiteral("尺寸:"): "Dimension:");
    m_painter->drawText(QRect(vw()/2, vh()/height*8, vw()/4, vh()/height), Qt::AlignCenter,
                        state ? QStringLiteral("移除项:"): "Remove Items:");

    setFont(50);
    QStringList removeList = list.at(6).split(";");
    int rm_height = removeList.size() > 6 ? 2 : 1;
    m_painter->drawText(QRect(vw()/4*3, vh()/height*1, vw()/4, vh()/height), Qt::AlignCenter, list.at(0));
    m_painter->drawText(QRect(vw()/4*3, vh()/height*2, vw()/4, vh()/height), Qt::AlignCenter, list.at(1));
    m_painter->drawText(QRect(vw()/4*3, vh()/height*3, vw()/4, vh()/height), Qt::AlignCenter, list.at(2));
    m_painter->drawText(QRect(vw()/4*3, vh()/height*4, vw()/4, vh()/height), Qt::AlignCenter, list.at(3));
    m_painter->drawText(QRect(vw()/4*3, vh()/height*5, vw()/4, vh()/height), Qt::TextWordWrap | Qt::AlignCenter, list.at(4));
    m_painter->drawText(QRect(vw()/4*3, vh()/height*6, vw()/4, vh()/height), Qt::TextWordWrap | Qt::AlignCenter, list.at(5));
    m_painter->drawText(QRect(vw()/4*3, vh()/height*7, vw()/4, vh()/height), Qt::TextWordWrap | Qt::AlignCenter, list.at(6));
    m_painter->drawText(QRect(vw()/4*3, vh()/height*8, vw()/4, vh()/height*rm_height), Qt::TextWordWrap | Qt::AlignCenter, list.at(7));
}

void BPdfCreater::drawSeidelInfo(QStringList &list)
{
    setPen(4, Qt::darkGray, Qt::DotLine);
    m_painter->drawLine(QPointF(vw()/2+20, vh()/height*10.5), QPointF(vw()-20, vh()/height*9.5));

    m_painter->fillRect(QRectF(vw()/2, vh()/height*9.5, vw()/2, vh()/height), QBrush(Qt::gray));

    setPen(5, Qt::black);
    setFont(80, true);
    m_painter->drawText(QRect(vw()/2, vh()/height*9.5, vw()/4, vh()/height), Qt::AlignCenter, "Seidel Aberration");

    setFont(60, true);
    m_painter->drawText(QRect(vw()/2, vh()/height*10.5, vw()/4, vh()/height), Qt::AlignCenter, "Seidel");
    m_painter->drawText(QRect(vw()/64*47, vh()/height*10.5, vw()/8, vh()/height), Qt::AlignCenter, "Value");
    m_painter->drawText(QRect(vw()/8*7, vh()/height*10.5, vw()/8, vh()/height), Qt::AlignCenter, "Clock(deg)");

    setFont(60);
    m_painter->drawText(QRect(vw()/2, vh()/height*11.5, vw()/4, vh()/height), Qt::AlignCenter, "Tilt");
    m_painter->drawText(QRect(vw()/2, vh()/height*12.5, vw()/4, vh()/height), Qt::AlignCenter, "Focus");
    m_painter->drawText(QRect(vw()/2, vh()/height*13.5, vw()/4, vh()/height), Qt::AlignCenter, "Astigmatism");
    m_painter->drawText(QRect(vw()/2, vh()/height*14.5, vw()/4, vh()/height), Qt::AlignCenter, "Coma");
    m_painter->drawText(QRect(vw()/2, vh()/height*15.5, vw()/4, vh()/height), Qt::AlignCenter, "Spherical");

    m_painter->drawText(QRect(vw()/64*47, vh()/height*11.5, vw()/8, vh()/height), Qt::AlignCenter, list.at(0));
    m_painter->drawText(QRect(vw()/64*47, vh()/height*12.5, vw()/8, vh()/height), Qt::AlignCenter, list.at(1));
    m_painter->drawText(QRect(vw()/64*47, vh()/height*13.5, vw()/8, vh()/height), Qt::AlignCenter, list.at(2));
    m_painter->drawText(QRect(vw()/64*47, vh()/height*14.5, vw()/8, vh()/height), Qt::AlignCenter, list.at(3));
    m_painter->drawText(QRect(vw()/64*47, vh()/height*15.5, vw()/8, vh()/height), Qt::AlignCenter, list.at(4));

    m_painter->drawText(QRect(vw()/8*7, vh()/height*11.5, vw()/8, vh()/height), Qt::AlignCenter, list.at(5));
    m_painter->drawText(QRect(vw()/8*7, vh()/height*12.5, vw()/8, vh()/height), Qt::AlignCenter, list.at(6));
    m_painter->drawText(QRect(vw()/8*7, vh()/height*13.5, vw()/8, vh()/height), Qt::AlignCenter, list.at(7));
    m_painter->drawText(QRect(vw()/8*7, vh()/height*14.5, vw()/8, vh()/height), Qt::AlignCenter, list.at(8));
    m_painter->drawText(QRect(vw()/8*7, vh()/height*15.5, vw()/8, vh()/height), Qt::AlignCenter, list.at(9));
}

void BPdfCreater::drawResultInfo(QVector<QStringList> &vec)
{
    setPen(4, Qt::darkGray, Qt::DotLine);
    m_painter->drawLine(QPointF(vw()/2+20, vh()/height*18), QPointF(vw()-20, vh()/height*18));
    m_painter->fillRect(QRectF(vw()/2, vh()/height*17, vw()/2, vh()/height), QBrush(Qt::gray));

    setPen(5, Qt::black);
    setFont(80, true);
    m_painter->drawText(QRect(vw()/2, vh()/height*17, vw()/4, vh()/height), Qt::AlignCenter, "Result");

    QStringList list = vec.at(0);
    QStringList vulist = vec.at(1);
    if ( list.size() <= 6 ) {
        setFont(60);
        for ( int i = 0; i < list.size(); ++i )
        {
            m_painter->drawText(QRect(vw()/2, vh()/height*(18+i), vw()/4, vh()/height), Qt::AlignCenter, list.at(i));
        }
        for ( int i = 0; i < vulist.size(); ++i )
        {
            m_painter->drawText(QRect(vw()/4*3, vh()/height*(18+i), vw()/4, vh()/height), Qt::AlignCenter, vulist.at(i));
        }
    } else {
        for ( int i = 0; i < list.size(); ++i )
        {
            setFont(list.at(i).size() > 14 ? 46 : 60);
            if ( i < 6 ) {
                m_painter->drawText(QRect(vw()/2, vh()/height*(18+i), vw()/9, vh()/height), Qt::AlignCenter, list.at(i));
            } else {
                m_painter->drawText(QRect(vw()/18*14, vh()/height*(18+i-6), vw()/9, vh()/height), Qt::AlignCenter, list.at(i));
            }
        }
        for ( int i = 0; i < vulist.size(); ++i )
        {
            if ( i < 6 ) {
                m_painter->drawText(QRect(vw()/18*11, vh()/height*(18+i), vw()/9, vh()/height), Qt::AlignCenter, vulist.at(i));
            } else {
                m_painter->drawText(QRect(vw()/9*8, vh()/height*(18+i-6), vw()/9, vh()/height), Qt::AlignCenter, vulist.at(i));
            }
        }
    }
}
