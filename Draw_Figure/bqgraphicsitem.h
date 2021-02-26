#ifndef BQGRAPHICSITEM_H
#define BQGRAPHICSITEM_H

#include <QObject>
#include <QAbstractGraphicsShapeItem>
#include <QPen>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include "bpointitem.h"

class BGraphicsItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT

public:
    enum ItemType {
        Circle = 0,         // 圆
        Ellipse,            // 椭圆
        Concentric_Circle,  // 同心圆
        Rectangle,          // 矩形
        Square,             // 正方形
        Polygon,            // 多边形
        Pill,               // 圆端矩形
        Chamfer             // 圆角矩形
    };

    enum AutoType {
        Auto_Circle = 0,    // 自动圆
        Auto_Ellipse,       // 自动椭圆
        Auto_Pill,          // 自动圆端矩形
        Auto_Chamfer,       // 自动圆角矩形
        Auto_RotateRec,     // 自动旋转矩形
        Auto_RoundEdgeRec,  // 自动圆边矩形
        Auto_Recognise,     // 全自动识别
    };

    bool getIfSelected() { return m_isSelected; }                           // 获取是否选中图元
    bool getIfLocked() { return m_isLocked; }                               // 获取是否为锁定状态
    bool getIfDottedLine() { return m_isDottedLine; }                       // 获取是否是虚线
    QPointF getCurrentCenter() { return this->mapToScene(QPointF(0,0)); }   // 获取图元中心点，并转换为场景坐标
    qreal getTextHeight() { return m_textItem.boundingRect().height(); }    // 获取文本的高度
    QString getTextCon() { return m_textItem.text(); }                      // 获取文本的内容
    void setTextCon(QString text) { m_textItem.setText(text); }             // 设置文本的内容
    void setTextPos(qreal x, qreal y) { m_textItem.setPos(x,y); }           // 设置文本的位置

    AutoType getAutoType() { return m_autoType; }                           // 自动掩膜 - 获取自动类型
    bool getIfUseScale() { return !is_value; }                              // 自动掩膜 - 获取是否使用缩放比例
    qreal getAutoWidth() { return m_autoWidth; }                            // 自动掩膜 - 获取宽度
    qreal getAutoHeight() { return m_autoHeight; }                          // 自动掩膜 - 获取高度
    qreal getAutoCirRadius() { return m_autoCircleRadius; }                 // 自动掩膜 - 获取圆的直径（光学领域用直径不用半径）
    qreal getAutoChaRadius() { return m_autoChamferRadius; }                // 自动掩膜 - 获取圆角矩形的半径
    qreal getAutoDis1() { return m_distance1; }                             // 自动掩膜 - 获取边长1
    qreal getAutoDis2() { return m_distance2; }                             // 自动掩膜 - 获取边长2
    qreal getAutoScale() { return m_autoScale; }                            // 自动掩膜 - 获取缩放比例
    int getIndentPixel() { return m_indentPixel; }                          // 自动掩膜 - 获取缩进像素值

    void setAutoType(AutoType type) { m_autoType = type; }                  // 自动掩膜 - 设置自动类型
    void setIsUseScale(bool useScale) { is_value = !useScale; }             // 自动掩膜 - 设置是否使用缩放比例
    void setAutoWidth(qreal width) { m_autoWidth = width; }                 // 自动掩膜 - 设置宽度
    void setAutoHeight(qreal height) { m_autoHeight = height; }             // 自动掩膜 - 设置高度
    void setAutoCirRadius(qreal circleRadius) { m_autoCircleRadius = circleRadius; }    // 自动掩膜 - 设置圆的直径
    void setAutoChaRadius(qreal chamferRadius) { m_autoChamferRadius = chamferRadius; } // 自动掩膜 - 设置圆角矩形的半径
    void setAutoDis1(qreal distance1) { m_distance1 = distance1; }          // 自动掩膜 - 设置边长1
    void setAutoDis2(qreal distance2) { m_distance2 = distance2; }          // 自动掩膜 - 设置边长2
    void setAutoScale(qreal scale) { m_autoScale = scale; }                 // 自动掩膜 - 设置缩放比例
    void setIndentPixel(int pixel) { m_indentPixel = pixel; }               // 自动掩膜 - 设置缩进像素值

    ItemType getType() { return m_type; }                                   // 手动掩膜 - 获取图元类型
    qreal getWidth() { return m_width; }                                    // 手动掩膜 - 获取宽度
    qreal getHeight() { return m_height; }                                  // 手动掩膜 - 获取高度
    qreal getRadius() { return m_radius; }                                  // 手动掩膜 - 获取直径
    qreal getRotation() { return m_rotation; }                              // 手动掩膜 - 获取旋转度数（-360度 ~ 360度）
    QList<QPointF> getPointList() { return m_list; }                        // 手动掩膜 - 获取所有点（用于多边形）

    void setType(ItemType type) { m_type = type; }                          // 手动掩膜 - 设置图元类型
    void setWidth(qreal width) { m_width = width; }                         // 手动掩膜 - 设置宽度
    void setHeight(qreal height) { m_height = height; }                     // 手动掩膜 - 设置高度
    void setRadius(qreal radius);                                           // 手动掩膜 - 设置直径
    void setMyRotation(qreal rotation) { m_rotation = rotation; }           // 手动掩膜 - 设置旋转度数

    void changeType(BGraphicsItem::ItemType type);                          // 修改图元类型
    void resetAutoData();                                                   // 重置自动测试参数
    void setEnable(bool enable);                                            // 设置图元属性（可移动、可选择、可获取焦点）
    QPointF getCentroid(QList<QPointF> list);                               // 获取多边形的质心（中心点）
    void getMaxLength();                                                    // 获取多边形最大边长，以此为半径绘制多边形（用于多边形）

    void updateScreen();                                                    // 更新图元
    void updateItemSize(QPointF p, bool isSpecial = false);                 // 更新图元大小（鼠标拖动边缘点改变大小）
    void updateItemEdge();                                                  // 更新图元大小（滚动鼠标中间改变大小）
    void updateEdgeInPolygon(QPointF origin, QPointF end);                  // 更新图元大小（多变形拖动边缘点改变大小）
    void updatePenType(bool isDottedLine);                                  // 更新画笔类型（虚实线转换）

signals:
    void startCreatePolygon(QString text);                                  // 开始创建多边形
    void removeMyself(BGraphicsItem *item);                                 // 删除图元

public slots:
    void updatePolygon(QString text, QList<QPointF> list, bool isFinished); // 刷新多边形

protected:
    BGraphicsItem(ItemType type);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event) override;

protected:
    ItemType m_type;
    bool m_isSelected;
    bool m_isLocked;
    bool m_isDottedLine;

    AutoType m_autoType;
    bool is_value;
    qreal m_autoWidth;
    qreal m_autoHeight;
    qreal m_autoCircleRadius;
    qreal m_autoChamferRadius;
    qreal m_distance1;
    qreal m_distance2;
    qreal m_autoScale;
    int m_indentPixel;

    qreal m_width;
    qreal m_height;
    qreal m_radius;
    qreal m_rotation;
    QGraphicsSimpleTextItem m_textItem;

    QPointF m_center;
    QPointF m_edge;
    QPointF m_special;
    BPointItemList m_pointList;
    QList<QPointF> m_list;
    bool is_create_finished;

    QPen m_pen_isSelected;
    QPen m_pen_noSelected;
};

//------------------------------------------------------------------------------

class BSyncytia : public BGraphicsItem
{
public:
    BSyncytia(ItemType type, qreal width, qreal height, QString text);

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

#endif // BQGRAPHICSITEM_H
