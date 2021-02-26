#include "gpdf.h"


Gpdf::Gpdf(const QString &file)
{
    m_file = new QFile(file);
    m_file->open(QIODevice::WriteOnly);

    m_pdfWriter = new QPdfWriter(m_file);                               // 创建 PDF 写入器
    m_pdfWriter->setPageSize(QPagedPaintDevice::A4);                    // 设置纸张大小为 A4
    m_pdfWriter->setPageOrientation(QPageLayout::Portrait);             // 设置写入方向为横向
    m_pdfWriter->setResolution(300);                                    // 设置纸张的分辨率为 300, 因此其像素为 3508 x 2479

    int imargins = 60;
    m_pdfWriter->setPageMargins(QMarginsF(imargins, imargins, imargins, imargins));     // 设置页边距
    m_pdfWriter->setCreator(QStringLiteral("上海乾曜光学科技有限公司"));

    m_painter = new QPainter(m_pdfWriter);                              // 绘图工具

    cols = 0;
    cols2 = 0;
    xDistance = 0;
    yDistance = 80;
    surplus = 0;
}

Gpdf::~Gpdf()
{
    m_painter->end();
    delete m_painter;

    delete m_pdfWriter;

    m_file->close();
    delete m_file;
}

void Gpdf::setPen(int width, QColor color, Qt::PenStyle style)
{
    QPen pen = m_painter->pen();
    pen.setWidth(width);
    pen.setColor(color);
    pen.setStyle(style);
    m_painter->setPen(pen);
}

void Gpdf::setFont(int size, bool bold, QString family)
{
    QFont font = m_painter->font();
    font.setPixelSize(size);
    font.setBold(bold);
    font.setFamily(family == "default" ? GlobalFun::getTTF(5) : family);
    m_painter->setFont(font);
}

void Gpdf::drawLogo(QImage image)
{
    logo = image;
    QRect rect(0, -140, vw(), 130);
    QImage pic = image.scaled(rect.width(), rect.height(), Qt::KeepAspectRatio);
    rect.setSize(QSize(pic.width(), pic.height()));
    m_painter->drawImage(rect, pic, pic.rect());
}

void Gpdf::drawCompanyOrUrl(QString str)
{
    companyNet = str;
    setFont(60);
    QRect rect(0, vh() + 5, vw(), 130);
    m_painter->drawText(rect, Qt::AlignLeft, str);
}

void Gpdf::drawPageNumber(int number, int count)
{
   setFont(60);
   QRect rect(0, vh() + 5, vw(), 130);
   QString str = QString::number(number) + " / " + QString::number(count);
   m_painter->drawText(rect, Qt::AlignRight, str);
}

void Gpdf::drawTime()
{
    setFont(60);
    QString str = GlobalFun::getCurrentTime(3);
    QRect rect(0, -120, vw(), 80);
    m_painter->drawText(rect, Qt::AlignRight, str);
}

void Gpdf::drawTable(bool type, int number, QStringList &tableHead)
{
    // type - 表格类型  （true - 全新的页，从头开始， false - 当前页，绘制第二个表格）
    // number - 绘制表格行数
    // 根据表格列数 确定单元格宽度
    xDistance = (vw() - 80)*1.0 / cols <= (vw() - 80)*1.0 / cols2 ? (vw() - 80)*1.0 / cols : (vw() - 80)*1.0 / cols2 ;

    if ( type ) {
        // 从头开始
        m_painter->fillRect(QRectF(0, 0, xDistance * tableHead.size(), yDistance), QBrush(Qt::gray));

        setPen(6, Qt::darkGray);
        // 列 横向
        for (int i = 0; i < tableHead.size() + 1; ++i)
        {
            m_painter->drawLine(QPointF(xDistance * i, 0), QPointF(xDistance * i, yDistance * number));
        }

        // 行 纵向
        for (int j = 0; j < number + 1; ++j)
        {
            m_painter->drawLine(QPointF(0, yDistance * j), QPointF(vw() - 80, yDistance * j));
        }

        setPen(5, Qt::black);
        // 填充表头内容 横向
        for(int j = 0; j < tableHead.size(); ++j)
        {
            m_painter->drawText(QRect(xDistance * j, 0, xDistance, yDistance), Qt::AlignCenter, tableHead.at(j));
        }
    } else {
        // 当前页的第二个表格
        m_painter->fillRect(QRectF(0, yDistance * (surplus + 1) + 60, xDistance * tableHead.size(), yDistance), QBrush(Qt::gray));

        setPen(6, Qt::darkGray);
        // 列 横向
        for (int i = 0; i < tableHead.size() + 1; ++i)
        {
            m_painter->drawLine(QPointF(xDistance * i, yDistance * (surplus + 1) + 60),
                                QPointF(xDistance * i, yDistance * (surplus + 1 + number) + 60));
        }

        // 行 纵向
        for (int j = 0; j < number + 1; ++j)
        {
            m_painter->drawLine(QPointF(0, yDistance * (surplus + 1 + j) + 60),
                                QPointF(vw() - 80, yDistance * (surplus + 1 + j) + 60));
        }

        setPen(5,Qt::black);
        // 填充表头内容 横向
        for(int j = 0; j < tableHead.size(); ++j)
        {
            m_painter->drawText(QRect(xDistance * j, yDistance * (surplus + 1) + 60, xDistance, yDistance),
                                Qt::AlignCenter, tableHead.at(j));
        }
    }
}

void Gpdf::drawMat(int start, int number, QVector<QImage> &pic)
{
    // start - 起始位置
    // number - 个数
    for (int i = 1; i < number + 1; ++i)
    {
        QRect rect(0, i * yDistance + 5, xDistance, yDistance - 10);
        QImage image = pic.at(i - 1 + start).scaled(rect.width(), rect.height(), Qt::KeepAspectRatio);
        rect.translate((rect.width() - image.width()) / 2.0, (rect.height() - image.height()) / 2.0);
        rect.setSize(QSize(image.width(), image.height()));
        m_painter->drawImage(rect, image, image.rect());
    }
}

void Gpdf::fillData(bool type, int tableNum, int start, int number, QStringList &data)
{
    // type - 表格类型 （true - 表格1 第一列为图片， false - 表格2 内容均为data值）
    // tableNum - 填充当前页的第 1 / 2 个表格
    // start - data 中起始位置
    // number - 行数

    if ( type ) {
        // 表格 1
        for (int i = 0; i< number; ++i)  // 行
        {
            for(int j = 0; j < cols - 1; ++j)        // 列
            {
                m_painter->drawText(QRect(xDistance * (j + 1), yDistance * (i + 1), xDistance, yDistance),
                                    Qt::AlignCenter, data.at(i * (cols - 1) + j + start));
            }
        }
    } else {
        // 表格 2
        if ( tableNum == 1 ) {
            // 另起一页
            for (int i = 0; i < number; ++i)  // 行
            {
                for(int j = 0; j < cols2; ++j)        // 列
                {
                    m_painter->drawText(QRect(xDistance * j, yDistance * (i + 1), xDistance, yDistance),
                                        Qt::AlignCenter, data.at(i * cols2 + j + start));
                }
            }
        } else {
            // 当前页的第二个表格
            for (int i = 0; i < number; ++i)  // 行
            {
                for(int j = 0; j < cols2; ++j)        // 列
                {
                    m_painter->drawText(QRect(xDistance * j, yDistance * (i + 1 + surplus + 1) + 60, xDistance, yDistance),
                                        Qt::AlignCenter, data.at(i * cols2 + j + start));
                }
            }
        }
    }
}

void Gpdf::drawPdf(QStringList &tableHead, QStringList &tableHead2, QStringList &data, QStringList &data2, QVector<QImage> &pic)
{
    // tableHead - 表头1
    // tableHead2 - 表头2
    // data - 表格数据1 每组数据个数 == 表头1个数 - 1
    // data2 - 表格数据2  每组数据个数 == 表头2个数
    // pic - 表格1 图片
    // 一页纸 可写35行数据
    cols = tableHead.size();
    cols2 = tableHead2.size();
    int page1 = pic.size() / 35 ;  // 第一个表格 完整的页数
    surplus = pic.size() % 35;     // 表格1 剩余的行数
    int page2 = (surplus + data2.size() / cols2  + 1)/35;  // (表格1 剩余的部分 + 表格2)  完整的页数
    int dataSurPlus = (surplus + data2.size() / cols2 + 1) % 35;   // 最终剩余的部分
    int page = dataSurPlus == 0 ? page1 + page2 : page1 + page2 + 1;

    if ( page <= 1 ) {
        // 只需要创建一页内容
        drawPageNumber(1, 1);
        drawTable(true, pic.size() + 1, tableHead);
        drawTable(false, data2.size() / cols2 + 1, tableHead2);
        fillData(true, 1, 0, data.size() / (tableHead.size() - 1), data);
        fillData(false, 2, 0, data2.size() / cols2, data2);
        drawMat(0, pic.size(), pic);
        drawTime();
    } else {
        // 需要绘制多页
        //  表格1 有多页数据
        if ( page1 >= 1 ) {
            for ( int j = 1; j < page1 + 1; ++j )
            {
                if ( j != 1 ) {
                    m_pdfWriter->newPage();
                    drawLogo(logo);
                    drawCompanyOrUrl(companyNet);
                }
                drawPageNumber(j, page);
                drawTable(true, 36, tableHead);
                fillData(true, 1, (j - 1) * 35 * (cols - 1), 35, data);
                drawMat((j - 1) * 35, 35, pic);
                drawTime();
            }
        }

        if ( page2 == 0 ) {
            // 表格1 表格2 都有 且 未满1页
            m_pdfWriter->newPage();
            drawLogo(logo);
            drawCompanyOrUrl(companyNet);
            drawPageNumber(page1 + 1, page );

            drawTable(true, surplus + 1, tableHead);
            drawTable(false, data2.size() / cols2 + 1, tableHead2);
            fillData(true, 1, page1 * 35 * (cols - 1), surplus, data);
            fillData(false, 2, 0, data2.size() / cols2, data2);
            drawMat(page1 * 35, pic.size() - page1 * 35, pic);
            drawTime();
        } else if ( page2 == 1 ) {
            // 表格1 表格2 都有 满 1 页
            if ( page1 != 0 ) {
                m_pdfWriter->newPage();
                drawLogo(logo);
                drawCompanyOrUrl(companyNet);
            }
            drawPageNumber( page1 + 1, page );

            drawTable(true, surplus + 1, tableHead);
            drawTable(false, 35 - surplus, tableHead2);
            fillData(true, 1, page1 * 35 * (cols - 1), surplus, data);
            fillData(false, 2, 0, 34 - surplus, data2);
            drawMat(page1 * 35, pic.size() - page1 * 35, pic);
            drawTime();

            if ( dataSurPlus != 0 ) {
                // 表格2 有剩余的部分 需另起一页
                m_pdfWriter->newPage();
                drawLogo(logo);
                drawCompanyOrUrl(companyNet);
                drawPageNumber(page, page);

                drawTable(true, dataSurPlus + 1, tableHead2);
                fillData(false, 1, data2.size() - dataSurPlus * cols2, dataSurPlus, data2);
                drawTime();
            }
        }

    }

}
