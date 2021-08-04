#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "globalfun.h"
#include "bqgraphicsscene.h"
#include "qcustomplot.h"
#include "algorithmMain.h"
#include "blog.h"
#include "bq3dsurface.h"
#include "bpdfcreater.h"

// QGraphicsView 提升至 QChartView 需要
#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // [0] 初始化数据
    void initData();                            // 初始化数据
    void initView();                            // 初始化界面
    void initGraphicsScene();                   // 初始化场景

    // [1] 基础操作（配置文件、动态界面、动态语言）
    void createDefaultConfig();                 // 创建默认配置文件
    void loadConfig();                          // 加载配置文件
    void saveConfig();                          // 保存配置文件
    void updateView(bool state);                // 更新界面
    void modifyScale();                         // 修改图像的显示比例
    void changeLanguage(int type);              // 改变语言
    void changeStatus(bool status);             // 改变状态
    void hideConfig(QString path);              // 隐藏配置文件
    void updateGraphBtn(bool type);             // 改变图形按钮状态
    void updateGraphItemSize(qreal m_pix);      // 更新图形界面图元尺寸

    // [2] 文件加载、保存 + 导出报表
    void loadResource();                        // 加载资源
    void saveResource();                        // 保存资源
    bool exportReport();                        // 导出报表
    void writeHead(QTextStream &out, int type); // 编写标头信息
    void saveXYZ(QString path);                 // 保存 XYZ 文件
    void loadXYZ(QString path);                 // 加载 XYZ 文件
    void saveACS(QString path);                 // 保存 ASC 文件
    void loadASC(QString path);                 // 加载 ASC 文件
    void saveSirius(QString path);              // 保存 Sirius 文件
    void loadSirius(QString path);              // 加载 Sirius 文件

    // [3] 创建表格 + 弹窗
    void createResultTable();                   // 创建 result table
    void createZernikeTable();                  // 创建 zernike table
    void createDataTable();                     // 创建 data table
    void create2DView();                        // 创建 2D view
    void create3DView();                        // 创建 3D view
    void createLineChart();                     // 创建 line chart
    void createPSFView();                       // 创建 PSF view
    void createQualityTable();                  // 创建 quality table
    void createContextMenu(int type);           // 创建右键菜单栏
    void createMenuDialog(int type);            // 创建菜单栏弹窗

    // [4] 调用算法
    void startCalculation();                    // 开始计算
    void modifyConfig();                        // 修改配置参数
    std::vector<cv::Mat> getMatList();          // 获取传给算法的图像
    cv::Mat getMask(cv::Mat image);             // 获取传给算法的掩膜

    // [5] 调用算法计算后填充数据
    void fillData();                            // 填充数据
    void change2DPlot(int type);                // 切换 2D 图形
    void change3DPlot(int type);                // 切换 3D 图形
    void updateZernikeData(float dec, int div, bool isOK);  // 更新 zernike table 数据
    void updateLineChart(bool isOK);            // 更新 line chart 数据
    void updatePSFPlot();                       // 更新 PSF 图形
    void drawMask();                            // 画掩膜
    void modifyTableHead();                     // 修改日志表头
    void psdShowEmpty();                        // PSD折线图初始状态

protected:
    // [6] 事件
    virtual bool event(QEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

signals:
    void screenshot(QString str);               // 截屏

private slots:
    // [12] 相位计算模块
    void loadOneAsc();

private:
    Ui::MainWindow *ui;

    BQGraphicsScene *m_scene;                   // 场景实例
    BLog m_log;                                 // 日志实例

    QStringList m_algParameter;                 // result 表格中的参数名称
    QStringList m_algParameterLimit;            // result 表格中的参数上下限名称
    QMap<QString, int> m_stateList;             // result 表格中参数的状态
    QMap<QString, double> m_limitList;          // result 表格中参数的数值上下限

    QLineSeries *xSeries;                       // 折线图中x切片数据
    QLineSeries *ySeries;                       // 折线图中y切片数据
    QChart *m_chart;                            // 折线图表

    QLineSeries *psd_xSeries;                   // psd 折线图x切片数据
    QLineSeries *psd_ySeries;                   // psd 折线图y切片数据
    QCategoryAxis *axisXX;                      // psd x坐标轴
    QCategoryAxis *axisYY;                      // psd y坐标轴
    QLabel *label_x;                            // x:log10
    QLabel *label_y;                            // y:log10
    QChart *psd_chart;                          // psd 折线图表

    QWidget *psf_container;                     // PSF 曲面容器
    BQ3DSurface *psf_graph;                     // PSF 三维曲面
    QSurfaceDataProxy *psf_proxy;               // PSF 三维曲面数据代理

    QWidget *container;                         // 容器
    BQ3DSurface *m_graph;                       // 三维曲面
    QSurfaceDataProxy *m_sProxy;                // 三维曲面数据代理

    AlgorithmMain *m_algorithm;                 // 算法实例
    std::vector<cv::Mat> m_matList;             // 图像列表
    cv::Mat m_manualMask;                       // 手动掩膜
    cv::Rect m_rect;                            // 截取掩膜的矩形框
    cv::Mat m_fillMask;                         // 填充掩膜
    cv::Mat m_edgeMask;                         // 边缘掩膜

    int m_2DPlotType;                           // 2D 图形类别 1-拟合图 2-残差图 3-毛刺填充图
    int m_3DPlotType;                           // 3D 图形类别
    bool loadFileState;                         // 读取文件状态
    bool isSirius;                              // 是否是sirius文件
    cv::Mat m_phase;                            // 相位图

    cv::Mat m_calc_base;                        // 相位计算 基础相位图
    cv::Mat m_calc_ope;                         // 相位计算 操作相位图
    cv::Mat m_mask;                             // 相位计算 基础 mask
    int calc_type;                              // 计算类型 1 - 加载   2 - 加   3 - 减   4 - 无
    double asc_res;                             // 相位计算 像素值 无需分析asc文件的情况下使用
    double m_px_1mm;
};
#endif // MAINWINDOW_H
