#include "globalfun.h"
#include "windows.h"

GlobalFun::GlobalFun()
{

}

QString GlobalFun::getProperty(const QString &source, QString section, QString key)
{
    QString path = section + "/" + key;
    QSettings settings(source, QSettings::IniFormat);
    return settings.value(path, "").toString();
}

void GlobalFun::setProperty(const QString &source, QString section, QString key, QString value)
{
    QString path = section + "/" + key;
    QSettings settings(source, QSettings::IniFormat);
    settings.setValue(path, value);
}

QJsonObject GlobalFun::getJsonObj(const QString &source)
{
    QFile file(source);

    if( !file.open(QFile::ReadOnly) ) {
        QJsonDocument doc = QJsonDocument::fromJson("", nullptr);
        return doc.object();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data, nullptr);
    return doc.object();
}

QString GlobalFun::getCurrentTime(int type)
{
    QDateTime time(QDateTime::currentDateTime());
    switch (type)
    {
    case 1: return time.toString("yyyy-MM-dd-hh_mm_ss");
    case 2: return time.toString("yyyy-MM-dd_hh:mm:ss.zzz");
    case 3: return time.toString("yyyy-MM-dd");
    case 4: return time.toString("yyyy-MM");
    case 5: return time.toString("yyyy-MM-dd hh:mm:ss");
    case 6: return time.toString("yyyy-MM-dd_hh:mm:ss");
    default: return "";
    }
}

QString GlobalFun::getCurrentPath()
{
    return QDir::currentPath();
}

QString GlobalFun::getCurrentThreadID()
{
    QString str = "";
    str.sprintf("%p", QThread::currentThread());
    return str;
}

void GlobalFun::bsleep(int second)
{
    QTime timer;
    timer.start();
    while ( timer.elapsed() < second )
    {
        QCoreApplication::processEvents();  // 非阻塞延时 - 不停地处理事件，让程序保持响应
    }
}

bool GlobalFun::equals(qreal a, qreal b)
{
    return abs(a-b) < pow(2, -52);
}

bool GlobalFun::isDirExist(QString &path)
{
    QDir dir(path);

    if( dir.exists() ) {
        return true;
    } else {
        return dir.mkdir(path);  // 只创建一级子目录，即必须保证上级目录存在
    }
}

bool GlobalFun::isFileExist(QString &fileName)
{
    QFileInfo fileInfo(fileName);
    return fileInfo.isFile();
}

bool GlobalFun::removeFolder(const QString &path)
{
    if ( path.isEmpty() ) {
        return false;
    }

    QDir dir(path);
    if( !dir.exists() ) {
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);     // 设置过滤
    QFileInfoList fileList = dir.entryInfoList();               // 获取所有的文件信息
    foreach (QFileInfo file, fileList)                          // 遍历文件信息
    {
        if ( file.isFile() ) {                                  // 是文件，删除
            file.dir().remove(file.fileName());
        } else {                                                // 递归删除
            removeFolder(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());                      // 删除文件夹
}

bool GlobalFun::removeFile(const QString &fileName)
{
    if( QFile::exists(fileName) ) {
        return QFile(fileName).remove();
    } else {
        return true;
    }
}

void GlobalFun::exportCSV(QString path, QTableWidget *widget)
{
    QFile file(path);
    if ( file.open(QFile::WriteOnly | QFile::Truncate) )
    {
        QTextStream data(&file);
        QStringList linelist;
        for (int i = 0; i != widget->columnCount(); ++i) {
            linelist.append(widget->horizontalHeaderItem(i)->text());
        }
        data << linelist.join(",") + "\n";

        for (int i = 0; i != widget->rowCount(); ++i) {
            linelist.clear();
            for (int j = 0; j != widget->columnCount(); ++j) {
                QString str = widget->item(i, j)->text();
                linelist.append(str);
            }
            data << linelist.join(",") + "\n";
        }
        file.close();

        showMessageBox(2, "Saved csv file successfully !");
    }
}

bool GlobalFun::judVecAvailable(std::vector<cv::Mat> vec)
{
    if ( vec.size() == 0 ) {
        return false;
    }

    for ( auto &temp: vec )
    {
        if ( temp.empty() ) {
            return false;
        }
    }

    return true;
}

void GlobalFun::showMessageBox(int type, QString info)
{
    switch ( type )
    {
    case 1: QMessageBox::question(nullptr, "Question", info, QMessageBox::Ok); break;       // 在正常操作中提出问题
    case 2: QMessageBox::information(nullptr, "Information", info, QMessageBox::Ok); break; // 用于报告有关正常操作的信息
    case 3: QMessageBox::warning(nullptr, "Warning", info, QMessageBox::Ok); break;         // 用于报告非关键错误
    case 4: QMessageBox::critical(nullptr, "Error", info, QMessageBox::Ok); break;          // 用于报告关键错误
    default: break;
    }
}

QString GlobalFun::getTTF(int id)
{
    QString fileName = "";

    switch (id)
    {
    case 1: fileName = ":/source/ttf/BalooBhaina-Regular.ttf"; break;
    case 2: fileName = ":/source/ttf/BRUX.otf"; break;
    case 3: fileName = ":/source/ttf/Aldrich-Regular.ttf"; break;
    case 4: fileName = ":/source/ttf/Branch-zystoo.otf"; break;
    case 5: fileName = ":/source/ttf/Times_New_Roman.ttf"; break;
    default: fileName = "微软雅黑"; break;
    }

    int fontId = QFontDatabase::addApplicationFont(fileName);
    QString ttf = QFontDatabase::applicationFontFamilies(fontId).at(0);
    return ttf;
}

QString GlobalFun::BCryptographicHash(QString str)
{
    std::wstring wString = str.toStdWString();
    QByteArray key = "QY_billy";

    for ( int i = 0; i < (int)wString.size(); ++i )
    {
        int index = i % key.size();
        wString[i] = wString[i] ^ key.at(index);
    }

    return QString::fromStdWString(wString).toUtf8();
}

//------------------------------------------------------------------------------

void GlobalFun::changeColorToRed(QImage& image)
{
    if ( image.format() == QImage::Format_RGB888 )
    {
        cv::Mat3b mat = cv::Mat(image.height(), image.width(), CV_8UC3, image.bits());
        std::vector<cv::Mat1b> rgbChannels;
        cv::split(mat, rgbChannels);
        cv::Mat1b red = rgbChannels[0];
        mat.setTo(cv::Vec3b{255, 0, 0}, red >= 254);
    }

    else if ( image.format() == QImage::Format_RGB32 )
    {
        cv::Mat4b mat = cv::Mat(image.height(), image.width(), CV_8UC4, image.bits());
        std::vector<cv::Mat1b> rgbChannels;
        cv::split(mat, rgbChannels);
        cv::Mat1b red = rgbChannels[0];
        mat.setTo(cv::Vec4b{0, 0, 255, 255}, red >= 254);
    }
}

void GlobalFun::overExposure(QImage& image)
{
//    DWORD start_time = GetTickCount();

    unsigned char *data = image.bits();
    int width = image.width();
    int height = image.height();

    if ( image.format() == QImage::Format_RGB32 )
    {
        for ( int i = 0; i < width; ++i )
        {
            for ( int j = 0; j < height; ++j )
            {
                if ( *data >= 254 && *(data + 1) >= 254 && *(data + 2) >= 254 )
                {
                    *data = 0;
                    *(data + 1) = 0;
                }
                data += 4;
            }
        }
    }
    else if ( image.format() == QImage::Format_RGB888 )
    {
        for ( int i = 0; i < width; ++i )
        {
            for ( int j = 0; j < height; ++j )
            {
                if ( *data >= 254 && *(data + 1) >= 254 && *(data + 2) >= 254 )
                {
                    *(data + 1) = 0;
                    *(data + 2) = 0;
                }
                data += 3;
            }
        }
    }

//    DWORD end_time = GetTickCount();
//    std::cout << "times = " << end_time - start_time << std::endl;
}

std::vector<cv::Mat> GlobalFun::cvtBGR2GRAY(const std::vector<cv::Mat> &vec, cv::Rect rect, bool state)
{
    std::vector<cv::Mat> ret;

    for ( auto &temp : vec )
    {
        if ( temp.empty() ) {
            qDebug() << "Image is empty !";
            ret.push_back(cv::Mat());
        } else {
            cv::Mat gray;
            cvtColor(temp, gray, cv::COLOR_BGR2GRAY);   // 转换为灰度图像
            gray.convertTo(gray, CV_32FC1);             // C1数据类型转换为32F
            if ( state ) {
                ret.push_back(gray(rect));
            } else {
                ret.push_back(gray);
            }
        }
    }

    return ret;
}

cv::Mat GlobalFun::convertQImageToMat(QImage &image)
{
    cv::Mat mat;

    switch( image.format() )
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    default: mat = cv::Mat(); break;
    }

    return mat.clone();
}

QImage GlobalFun::convertMatToQImage(const cv::Mat mat)
{
    if( mat.type() == CV_8UC1 )
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for( int i = 0; i < 256; i++ )
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for( int row = 0; row < mat.rows; row++ )
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }

    else if( mat.type() == CV_8UC3 )
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, (int)mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }

    else if( mat.type() == CV_8UC4 )
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, (int)mat.step, QImage::Format_RGB32);
        return image.copy();
    }

    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat GlobalFun::dataProcessing(cv::Mat mat, bool state)
{
    double minV = 0, maxV = 0;
    cv::Mat temp = mat.clone();     // clone mat

    cv::Mat tmpMask = temp == temp; // 不相等为0，相等为1（nan与nan不相等）

    cv::minMaxIdx(temp, &minV, &maxV, nullptr, nullptr, temp == temp);  // 找出最小、最大值
    if (abs(maxV - minV) > 0.00000001) {
        temp = (temp - minV) / (maxV - minV) * 255;     // 数值在0~255之间
    }

    temp.convertTo(temp, CV_8UC1);
    cv::applyColorMap(temp, temp, cv::COLORMAP_JET);

    if ( state ) {
        temp.setTo(cv::Vec3b(255, 255, 255), ~tmpMask);
    } else {
        temp.setTo(nan(""), ~tmpMask);
    }
    return temp;
}

cv::Mat GlobalFun::createAlpha(cv::Mat& src)
{
    cv::Mat alpha = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
    cv::Mat gray = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

    cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);

    for ( int i = 0; i < src.rows; ++i )
    {
        for ( int j = 0; j < src.cols; ++j )
        {
            alpha.at<uchar>(i, j) = 255 - gray.at<uchar>(i, j);
        }
    }

    return alpha;
}

int GlobalFun::addAlpha(cv::Mat& src, cv::Mat& dst, cv::Mat& alpha)
{
    if ( src.channels() == 4 ) {
        return -1;
    } else if ( src.channels() == 1 ) {
        cv::cvtColor(src, src, cv::COLOR_GRAY2RGB);
    }

    dst = cv::Mat(src.rows, src.cols, CV_8UC4);

    std::vector<cv::Mat> srcChannels;
    std::vector<cv::Mat> dstChannels;

    //分离通道
    cv::split(src, srcChannels);

    dstChannels.push_back(srcChannels[0]);
    dstChannels.push_back(srcChannels[1]);
    dstChannels.push_back(srcChannels[2]);

    //添加透明度通道
    dstChannels.push_back(alpha);

    //合并通道
    cv::merge(dstChannels, dst);

    return 0;
}

//------------------------------------------------------------------------------

void GlobalFun::autoAim(cv::Mat src, cv::Mat ori, qreal centerXDis, qreal centerYDis,
                        qreal min_radius, qreal &xDis, qreal &yDis, qreal &zMult, float &radius)
{
    // 确定中心点
    int centerX = src.cols/2 + centerXDis;
    int centerY = src.rows/2 + centerYDis;
    int boundary = 1;
    cv::Point centerPoint;

    // 得到包含二维点集的最小圆的圆心的半径
    cv::Point2f center;
    getMinCircle(src, ori, centerX, centerY, center, radius);

    if ( radius == 0 ) {
        xDis = 0;
        yDis = 0;
        zMult = 0;
    } else {
        centerPoint.x = center.x;
        centerPoint.y = center.y;

        qreal x = centerX - centerPoint.x;
        qreal y = centerY - centerPoint.y;
        qreal z = radius / min_radius;
        xDis = abs(x) >= boundary ? x : 0;
        yDis = abs(y) >= boundary ? y : 0;
        zMult = z > 1 ? z : 0;
    }

    //---------------------------------

//    // 绘制圆形
//    if ( radius != 0 )
//    {
//        cv::RNG rng(12345);
//        cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255) );
//        circle( src, center, (int)radius, color, 2, 8, 0 );
//    }

//    if ( xDis != 0 || yDis != 0 )
//    {
//        line( src, cv::Point(centerX, centerY), centerPoint, cv::Scalar(0, 0, 255), 1, cv::LINE_AA );
//    }
}

void GlobalFun::heightCalibration(cv::Mat src, cv::Mat def, cv::Mat ori, double distance, qreal &radius, qreal &scale_mm)
{
    // 得到包含二维点集的最小圆的圆心的半径
    cv::Point2f src_center, def_center;
    float src_radius, def_radius;
    getMinCircle(src, ori, src_center, src_radius);
    getMinCircle(def, ori, def_center, def_radius);

    if ( src_radius == 0 || def_radius == 0 ) {
        radius = -1;
        scale_mm = -1;
    } else {
        // 计算标定值
        qreal scale = 0;

        if ( src_radius < def_radius ) {
            radius = src_radius;
            scale = def_radius / src_radius;
        } else {
            radius = def_radius;
            scale = src_radius / def_radius;
        }

        scale_mm = distance / scale;

        cv::RNG rng(12345);
        cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255) );
        circle( src, src_center, (int)src_radius, color, 2, 8, 0 );
        circle( def, def_center, (int)def_radius, color, 2, 8, 0 );

        string str = QString::number(def_radius).toStdString();
        cv::Point2f org;
        org.x = def_center.x - def_radius - 5;
        org.y = def_center.y - def_radius - 20;
        putText( def, str, org, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 1, cv::LINE_AA );
    }
}

void GlobalFun::planeCalibration(cv::Mat src, cv::Mat def, cv::Mat ori, double distance, qreal &pix_mm)
{
    // 得到包含二维点集的最小圆的圆心的半径
    cv::Point2f src_center, def_center;
    float src_radius, def_radius;
    getMinCircle(src, ori, src_center, src_radius);
    getMinCircle(def, ori, def_center, def_radius);

    if ( src_radius == 0 || def_radius == 0 ) {
        pix_mm = -1;
    } else {
        qreal dx = abs(def_center.x - src_center.x);
        qreal dy = abs(def_center.y - src_center.y);
        qreal pix = dx < dy ? dy : dx;
        pix_mm = distance / pix;

        cv::Point2f org;
        if ( pix == dy ) {
            org.x = src_center.x + 20;
            org.y = (src_center.y + def_center.y) / 2;
        } else {
            org.x = src_center.x < def_center.x ? src_center.x : def_center.x;
            org.y = src_center.y - 20;
        }

        string str = QString::number(pix).toStdString();

        putText( def, str, org, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 1, cv::LINE_AA );
        line( def, src_center, def_center, cv::Scalar(0, 0, 255), 1, cv::LINE_AA );

        cv::RNG rng(12345);
        cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255) );
        circle( def, src_center, (int)src_radius, color, 2, 8, 0 );
        circle( def, def_center, (int)def_radius, color, 2, 8, 0 );
    }
}

void GlobalFun::centerCalibration(cv::Mat input, cv::Mat &output, qreal &dx, qreal &dy)
{
    // 取图像中心 100x100
    cv::Mat dst = input(cv::Range(input.rows/2 - 50, input.rows/2 + 50), cv::Range(input.cols/2 - 50, input.cols/2 + 50));

    // 转化成灰度图像并进行平滑处理
    cv::Mat src_gray;
    cvtColor(dst, src_gray, cv::COLOR_BGR2GRAY);
    blur(src_gray, src_gray, cv::Size(3,3));

    // 阈值化
    cv::Mat threshold_output;
    threshold(src_gray, threshold_output, 100, 255, cv::THRESH_BINARY_INV); // 像素值小于100变成0，大于100变成255

    // 边缘检测
    cv::Mat edge;
    Canny(threshold_output, edge, 50, 200, 3);

    // 霍夫线变换
    vector<cv::Vec2f> lines;
    HoughLines(edge, lines, 1, CV_PI/180, 20, 0, 0);

    vector<cv::Vec4i> landscape_lines;
    vector<cv::Vec4i> portrait_lines;

    // 判断是横线还是竖线，并画出所有线条
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        cv::Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 100*(-b));
        pt1.y = cvRound(y0 + 100*(a));
        pt2.x = cvRound(x0 - 100*(-b));
        pt2.y = cvRound(y0 - 100*(a));

        if ( abs(pt2.x - pt1.x) < 10 ) {
            portrait_lines.push_back(cv::Vec4i(pt1.x, pt1.y, pt2.x, pt2.y));
        }
        if ( abs(pt2.y - pt1.y) < 10 ) {
            landscape_lines.push_back(cv::Vec4i(pt1.x, pt1.y, pt2.x, pt2.y));
        }
    }

    // 计算交点
    vector<cv::Point2f> vec;
    for ( size_t i = 0; i < landscape_lines.size() && i < 2; ++i )
    {
        for ( size_t j = 0; j < portrait_lines.size() && j < 2; ++j )
        {
            vec.push_back(getCrossPoint(landscape_lines.at(i), portrait_lines.at(j)));
        }
    }

    // 计算中心点
    cv::Point2f ret(0, 0);
    for ( auto temp : vec )
    {
        ret.x += temp.x;
        ret.y += temp.y;
    }

    // 计算像素差
    dx = ret.x / vec.size() - dst.cols/2;
    dy = ret.y / vec.size() - dst.rows/2;

    output = dst.clone();
    line(output, cv::Point(0, 0), cv::Point(output.cols, output.rows), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
    line(output, cv::Point(output.cols, 0), cv::Point(0, output.rows), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
    line(output, cv::Point(output.cols/2+dx, output.rows/2+dy),
         cv::Point(output.cols/2, output.rows/2), cv::Scalar(255, 0, 0), 1, cv::LINE_AA);
}

void GlobalFun::getMinCircle(cv::Mat src, cv::Mat ori, cv::Point2f &center, float &radius)
{
    // 转化成灰度图像并进行平滑处理
    cv::Mat src_gray, ori_gray, gray;
    cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
    cvtColor(ori, ori_gray, cv::COLOR_BGR2GRAY);
    gray = src_gray - ori_gray;
    blur(gray, gray, cv::Size(3, 3));

    // 自适应阈值法
    cv::Mat threshold_output;
    threshold(gray, threshold_output, 0, 255, cv::THRESH_OTSU);

    // 过滤小于设置点数的图形
    bwareaopen(threshold_output, threshold_output, 30);

    // 找到所有轮廓
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    findContours(threshold_output, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

    // 数据初始化
    center = cv::Point2f(0, 0);
    radius = 0;

    if ( contours.size() != 0 )
    {
        size_t index = 0;
        size_t maxSize = 0;

        // 寻找最大的点集
        for ( size_t i = 0; i < contours.size(); ++i )
        {
            if ( contours[i].size() > maxSize ) {
                maxSize = contours[i].size();
                index = i;
            }
        }

        // 得到包含二维点集的最小圆的圆心的半径
        vector<cv::Point> contours_poly = contours[index];
        minEnclosingCircle(contours_poly, center, radius);
    }
}

void GlobalFun::getMinCircle(cv::Mat src, cv::Mat ori, qreal centerX, qreal centerY, cv::Point2f &center, float &radius)
{
    // 数据初始化
    center = cv::Point2f(0, 0);
    radius = 0;

    // 转化成灰度图像并进行平滑处理
    cv::Mat src_gray, ori_gray, gray;
    cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
    cvtColor(ori, ori_gray, cv::COLOR_BGR2GRAY);
    gray = src_gray - ori_gray;
    blur(gray, gray, cv::Size(3, 3));

    // 中心填充
    int x = (int)centerX - 3;
    int y = (int)centerY - 3;
    for ( int i = 0; i < 7; ++i )
    {
        for ( int j = 0; j < 7; ++j )
        {
            gray.at<char>(y + j, x + i) = 127;
        }
    }

    // 自适应阈值法
    cv::Mat threshold_output;
    threshold(gray, threshold_output, 0, 255, cv::THRESH_OTSU);

    // 过滤小于设置点数的图形
    bwareaopen(threshold_output, threshold_output, 30);

    // 找到所有轮廓
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    findContours(threshold_output, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

    if ( contours.size() != 0 )
    {
        size_t index = 0;
        size_t maxSize = 0;

        // 寻找最大的点集
        for ( size_t i = 0; i < contours.size(); ++i )
        {
            if ( contours[i].size() > maxSize ) {
                maxSize = contours[i].size();
                index = i;
            }
        }

        // 得到包含二维点集的最小圆的圆心的半径
        vector<cv::Point> contours_poly = contours[index];
        minEnclosingCircle(contours_poly, center, radius);
    }
}

void GlobalFun::bwareaopen(cv::Mat src, cv::Mat &dst, double min_area)
{
    dst = src.clone();
    std::vector<std::vector<cv::Point> >  contours;
    std::vector<cv::Vec4i>    hierarchy;

    cv::findContours(src, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
    if ( !contours.empty() && !hierarchy.empty() )
    {
        std::vector< std::vector<cv::Point> >::const_iterator itc = contours.begin();

        while ( itc != contours.end() )
        {
            cv::Rect rect = cv::boundingRect( cv::Mat(*itc) );
            double area = contourArea(*itc);

            if ( area < min_area ) {
                for ( int i = rect.y; i < rect.y + rect.height; ++i )
                {
                    uchar *output_data = dst.ptr<uchar>(i);
                    for ( int j = rect.x; j < rect.x + rect.width; ++j )
                    {
                        if ( output_data[j] == 255 ) {
                            output_data[j] = 0;
                        }
                    }
                }
            }
            ++itc;
        }
    }
}

cv::Point2f GlobalFun::getCrossPoint(cv::Vec4i lineA, cv::Vec4i lineB)
{
    cv::Point2f pt;
    double X1 = lineA[2] - lineA[0];
    double Y1 = lineA[3] - lineA[1];

    double X2 = lineB[2] - lineB[0];
    double Y2 = lineB[3] - lineB[1];

    double X21 = lineB[0] - lineA[0];
    double Y21 = lineB[1] - lineA[1];

    double D = Y1*X2 - Y2*X1;

    if ( D == 0 ) return cv::Point2f();

    pt.x = (X1*X2*Y21 + Y1*X2*lineA[0] - Y2*X1*lineB[0]) / D;
    pt.y = -(Y1*Y2*X21 + X1*Y2*lineA[1] - X2*Y1*lineB[1]) / D;

    if ((abs(pt.x - lineA[0] - X1 / 2) <= abs(X1 / 2)) &&
            (abs(pt.y - lineA[1] - Y1 / 2) <= abs(Y1 / 2)) &&
            (abs(pt.x - lineB[0] - X2 / 2) <= abs(X2 / 2)) &&
            (abs(pt.y - lineB[1] - Y2 / 2) <= abs(Y2 / 2)))
    {
        return pt;
    }

    return cv::Point2f();
}

//******************************************************************************************************************

QString GlobalString::table_style = "";

QString GlobalString::menu_file = "";
QString GlobalString::menu_language = "";
QString GlobalString::menu_help = "";

QString GlobalString::action_load = "";
QString GlobalString::action_save = "";
QString GlobalString::action_screenshot = "";
QString GlobalString::action_export_report = "";
QString GlobalString::action_setting = "";
QString GlobalString::action_chinese = "";
QString GlobalString::action_english = "";
QString GlobalString::action_about = "";

QString GlobalString::graphics_circle = "";
QString GlobalString::graphics_ellipse = "";
QString GlobalString::graphics_concentric_circle = "";
QString GlobalString::graphics_rectangle = "";
QString GlobalString::graphics_square = "";
QString GlobalString::graphics_polygon = "";
QString GlobalString::graphics_pill = "";
QString GlobalString::graphics_chamfer = "";

QString GlobalString::graphics_auto_circle = "";
QString GlobalString::graphics_auto_ellipse = "";
QString GlobalString::graphics_auto_pill = "";
QString GlobalString::graphics_auto_chamfer = "";
QString GlobalString::graphics_auto_roundEdgeRec = "";
QString GlobalString::graphics_auto_rotateRec = "";
QString GlobalString::graphics_auto_recognise = "";

QString GlobalString::graphics_clear = "";
QString GlobalString::graphics_change = "";
QString GlobalString::graphics_manual = "";
QString GlobalString::graphics_auto = "";

QString GlobalString::contextMenu_clear = "";
QString GlobalString::contextMenu_delete = "";
QString GlobalString::contextMenu_export = "";
QString GlobalString::contextMenu_show = "";
QString GlobalString::contextMenu_sure = "";
QString GlobalString::contextMenu_unit = "";
QString GlobalString::contextMenu_fitting = "";
QString GlobalString::contextMenu_residual = "";
QString GlobalString::contextMenu_fillSpikes = "";

QString GlobalString::phase_add = "";
QString GlobalString::phase_minus = "";
QString GlobalString::phase_rotate = "";
QString GlobalString::phase_inverse = "";
QString GlobalString::phase_multiply = "";

//******************************************************************************************************************

// common
int GlobalValue::com_tp = 0;
double GlobalValue::com_p_sle = 0;
int GlobalValue::com_unit = 0;
int GlobalValue::com_dcl = 0;
int GlobalValue::com_fps = 0;
int GlobalValue::com_rnt = 0;
int GlobalValue::com_dimensionType = 0;

// analysis
int GlobalValue::ana_row = 0;
int GlobalValue::ana_col = 0;
int GlobalValue::ana_per = 0;

// language
int GlobalValue::lgn_tp = 0;

// graphicsItem
int GlobalValue::gra_def_size = 0;
double GlobalValue::gra_pixel = 0;
double GlobalValue::gra_c_rad = 0;
double GlobalValue::gra_e_wid = 0;
double GlobalValue::gra_e_hei = 0;
double GlobalValue::gra_cc_rad_1 = 0;
double GlobalValue::gra_cc_rad_2 = 0;
double GlobalValue::gra_r_wid = 0;
double GlobalValue::gra_r_hei = 0;
double GlobalValue::gra_s_len = 0;
double GlobalValue::gra_p_wid = 0;
double GlobalValue::gra_p_hei = 0;
double GlobalValue::gra_ch_wid = 0;
double GlobalValue::gra_ch_hei = 0;
double GlobalValue::gra_ch_rad = 0;

// zernike
int GlobalValue::zer_pis = 0;
int GlobalValue::zer_tilt = 0;
int GlobalValue::zer_pow = 0;
int GlobalValue::zer_ast = 0;
int GlobalValue::zer_coma = 0;
int GlobalValue::zer_sph = 0;

// file
QString GlobalValue::file_load_path = "";
QString GlobalValue::file_save_path = "";
QString GlobalValue::file_pdf_path = "";
QString GlobalValue::file_pdt_name = "";
QString GlobalValue::file_pdt_id = "";
QString GlobalValue::file_qua_tester = "";
QString GlobalValue::file_device_name = "";
QString GlobalValue::file_device_id = "";
QString GlobalValue::file_company_name = "";

// parameter
int GlobalValue::par_psi = 0;
int GlobalValue::par_unw = 0;
int GlobalValue::par_ztm = 0;
double GlobalValue::par_i_s_f = 0;
double GlobalValue::par_t_w = 0;
double GlobalValue::par_i_w = 0;
double GlobalValue::par_r_w = 0;
double GlobalValue::par_hp_fc = 0;

int GlobalValue::par_flt = 0;
int GlobalValue::par_fws = 0;
int GlobalValue::par_rm_spk = 0;
int GlobalValue::par_srs = 0;
int GlobalValue::par_sth = 0;
int GlobalValue::par_rm_rsl = 0;
double GlobalValue::par_ref_index = 0;

//******************************************************************************************************************
