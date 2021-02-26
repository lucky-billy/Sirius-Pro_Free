#ifndef BQGRAPHICSSCENE_H
#define BQGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsPixmapItem>
#include "bqgraphicsitem.h"

#include <opencv2/opencv.hpp>

class BQGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    BQGraphicsScene(QObject *parent = nullptr);

    void setPixmapSize(int width, int height);                              // 设置默认图像的大小
    void setMask(QImage image);                                             // 设置掩膜
    QImage getMask();                                                       // 获取掩膜
    void createBGraphicsItem(BGraphicsItem::ItemType type, QString text);   // 创建图元
    QList<BGraphicsItem *> getGraphicsItemList();                           // 获取图元列表
    QGraphicsPixmapItem& getUnlivePixmapItem();                             // 获取非实时模式下的图像
    void updateImage(std::vector<cv::Mat> &list);                           // 更新图像
    void saveItemToConfig(QString fileName);                                // 关闭程序时，保存场景中所有图元到配置文件中
    void loadItemToScene(QString fileName);                                 // 打开程序时，从配置文件加载图元到场景中

    void connectCirclePushButton(QPushButton *btn);                         // 绑定界面上按钮 - 圆
    void connectEllipsePushButton(QPushButton *btn);                        // 绑定界面上按钮 - 椭圆
    void connectConCirclePushButton(QPushButton *btn);                      // 绑定界面上按钮 - 同心圆
    void connectRectanglePushButton(QPushButton *btn);                      // 绑定界面上按钮 - 矩形
    void connectSquarePushButton(QPushButton *btn);                         // 绑定界面上按钮 - 正方形
    void connectPolygonPushButton(QPushButton *btn);                        // 绑定界面上按钮 - 多边形
    void connectPillPushButton(QPushButton *btn);                           // 绑定界面上按钮 - 远端矩形
    void connectChamferPushButton(QPushButton *btn);                        // 绑定界面上按钮 - 圆角矩形
    void connectClearPushButton(QPushButton *btn);                          // 绑定界面上按钮 - 清空
    void connectSavePushButton(QPushButton *btn);                           // 绑定界面上按钮 - 保存
    void connectLoadPushButton(QPushButton *btn);                           // 绑定界面上按钮 - 加载
    void connectChangePushButton(QPushButton *btn);                         // 绑定界面上按钮 - 切换自动手动

    void initStatus();                                                      // 初始化界面上按钮的禁用状态（自动掩膜时需禁用按钮）
    void changeBtnState(bool enable, bool exceptLast = false);              // 改变按钮的禁用状态
    void removeItemDFromList(BGraphicsItem *item);                          // 从列表中删除图元（只删除一个）
    void clearItem();                                                       // 清空所有图元

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event) override;

signals:
    void updatePoint(QString text, QList<QPointF> list, bool isFinished);
    void changeMode();

protected:
    QList<QPointF> m_list;
    bool is_creating_BPolygon;
    bool is_ignore_contextMenu;
    QString current_polygon;
    QList<QPushButton *> m_btnList;
    bool is_homo_test;

    // contextMenu
    int auto_type;

    QGraphicsPixmapItem m_unlivePixmap;
    QGraphicsSimpleTextItem m_simpleText;
    QGraphicsPixmapItem *m_mask;
    std::vector<cv::Mat> m_matList;
    QList<BGraphicsItem *> m_graphicsItemList;
};

#endif // BQGRAPHICSSCENE_H
