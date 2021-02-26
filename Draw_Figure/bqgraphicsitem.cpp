#include "bqgraphicsitem.h"
#include "globalfun.h"
#include <QVector>

#include <QMenu>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QWidgetAction>
#include <QRadioButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

BGraphicsItem::BGraphicsItem(ItemType type) : m_type(type)
{
    resetAutoData();

    m_isSelected = false;
    m_isLocked = false;
    m_isDottedLine = false;
    m_width = m_height = m_radius = m_rotation = 0;

    m_pen_noSelected.setColor(QColor(255, 255, 255));
    m_pen_noSelected.setWidth(2);
    m_pen_isSelected.setColor(QColor(0, 255, 0));
    m_pen_isSelected.setWidth(2);
    this->setPen(m_pen_noSelected);

    QFont font;
    font.setPixelSize(60);
    m_textItem.setFont(font);
    m_textItem.setBrush(Qt::yellow);
    m_textItem.setParentItem(this);
}

void BGraphicsItem::changeType(BGraphicsItem::ItemType type)
{
    m_type = type;
    m_width = m_height = m_radius = m_rotation = 0;

    switch (m_type) {
    case BGraphicsItem::ItemType::Circle: {
        m_width = m_height = GlobalValue::gra_c_rad;
    } break;
    case BGraphicsItem::ItemType::Ellipse: {
        m_width = GlobalValue::gra_e_wid;
        m_height = GlobalValue::gra_e_hei;
    } break;
    case BGraphicsItem::ItemType::Concentric_Circle: {
        qreal radius1 = GlobalValue::gra_cc_rad_1;
        qreal radius2 = GlobalValue::gra_cc_rad_2;
        m_width = m_height = radius1 >= radius2 ? radius1 : radius2;
        m_radius = radius1 >= radius2 ? radius2 : radius1;
    } break;
    case BGraphicsItem::ItemType::Rectangle: {
        m_width = GlobalValue::gra_r_wid;
        m_height = GlobalValue::gra_r_hei;
    } break;
    case BGraphicsItem::ItemType::Square: {
        m_width = m_height = GlobalValue::gra_s_len;
    } break;
    case BGraphicsItem::ItemType::Polygon: {
        m_list.clear();
        for ( auto &temp : m_pointList )
        {
            delete temp;
            m_pointList.removeOne(temp);
        }
        is_create_finished = false;
        emit startCreatePolygon(m_textItem.text());
    } break;
    case BGraphicsItem::ItemType::Pill: {
        m_width = GlobalValue::gra_p_wid;
        m_height = GlobalValue::gra_p_hei;
    } break;
    case BGraphicsItem::ItemType::Chamfer: {
        m_width = GlobalValue::gra_ch_wid;
        m_height = GlobalValue::gra_ch_hei;
        m_radius = GlobalValue::gra_ch_rad;
    } break;
    default: break;
    }

    updateScreen();
}

void BGraphicsItem::resetAutoData()
{
    m_autoType = BGraphicsItem::AutoType::Auto_Circle;
    is_value = true;
    m_autoWidth = GlobalValue::gra_pixel * GlobalValue::gra_def_size /3*2;
    m_autoHeight = GlobalValue::gra_pixel * GlobalValue::gra_def_size /3*2;
    m_autoCircleRadius = GlobalValue::gra_pixel * GlobalValue::gra_def_size /3*2;
    m_autoChamferRadius = GlobalValue::gra_pixel * GlobalValue::gra_def_size /6;
    m_distance1 = GlobalValue::gra_pixel * GlobalValue::gra_def_size /3;
    m_distance2 = GlobalValue::gra_pixel * GlobalValue::gra_def_size /3*(-1);
    m_autoScale = 1;
    m_indentPixel = 1;
}

void BGraphicsItem::setEnable(bool enable)
{
    m_isLocked = !enable;
    this->setFlag(QGraphicsItem::ItemIsSelectable, enable);
    this->setFlag(QGraphicsItem::ItemIsMovable, enable);
    this->setFlag(QGraphicsItem::ItemIsFocusable, enable);
}

void BGraphicsItem::updatePolygon(QString text, QList<QPointF> list, bool isFinished)
{
    if (text == m_textItem.text() && m_type == BGraphicsItem::ItemType::Polygon && !is_create_finished) {
        is_create_finished = isFinished;
        if ( !isFinished ) {
            QPointF p = list.at(list.length()-1);
            m_list.append(p);

            BPointItem *edge = new BPointItem(this, p, BPointItem::Edge);
            edge->setParentItem(this);
            m_pointList.append(edge);
        }

        m_center = getCentroid(m_list);
        if ( isFinished ) {
            m_list.append(m_center);
            m_pointList.append(new BPointItem(this, m_center, BPointItem::Center));
            m_pointList.setRandColor();
        }

        getMaxLength();
        updateScreen();
    }
}

QPointF BGraphicsItem::getCentroid(QList<QPointF> list)
{
    qreal x = 0;
    qreal y = 0;
    for (auto &temp : list)
    {
        x += temp.x();
        y += temp.y();
    }
    x = x/list.size();
    y = y/list.size();
    return QPointF(x,y);
}

void BGraphicsItem::getMaxLength()
{
    QVector<qreal> vec;
    for (auto &temp : m_list)
    {
        qreal dis = sqrt(pow(m_center.x() - temp.x(), 2) + pow(m_center.y() - temp.y(), 2));
        vec.append(dis);
    }

    qreal ret = 0;
    for (auto &temp : vec)
    {
        if (temp > ret) {
            ret = temp;
        }
    }
    m_width = ret * 2;
    m_height = ret * 2;
}

//------------------------------------------------------------------------------

void BGraphicsItem::setRadius(qreal radius)
{
    // second step - set m_radius
    m_radius = radius;
    BPointItem *special = new BPointItem(this, QPointF(0, 0), BPointItem::Special);

    if ( m_type == BGraphicsItem::ItemType::Concentric_Circle ) {
        special->setPoint(QPointF(m_radius/2/sqrt(2), m_radius/2/sqrt(2)));
    } else if ( m_type == BGraphicsItem::ItemType::Chamfer ) {

        if ( m_width >= m_height ) {
            special->setPoint(QPointF(m_width/2, -m_height/2 + m_radius));
        } else {
            special->setPoint(QPointF(m_width/2 - m_radius, -m_height/2));
        }
    }

    special->setParentItem(this);
    m_pointList.append(special);
    m_pointList.setRandColor();
}

void BGraphicsItem::updateScreen()
{
    if (m_width >= m_radius) {
        if ( m_type == BGraphicsItem::ItemType::Polygon ) {
            setTextPos( m_center.x() - m_width/2, m_center.y() - m_height/2 - getTextHeight() );
        } else {
            setTextPos( (-1)*m_width/2, (-1)*m_height/2 - getTextHeight() );
        }
    } else {
        setTextPos( (-1)*m_radius/2, (-1)*m_radius/2 - getTextHeight() );
    }
    setRotation(m_rotation);
}

void BGraphicsItem::updateItemSize(QPointF p, bool isSpecial)
{
    // third step - move the edge to modify the graphics
    switch (m_type)
    {
    case BGraphicsItem::ItemType::Circle: {
        qreal radius = sqrt(pow(m_center.x() - p.x(), 2) + pow(m_center.y() - p.y(), 2));
        m_width = m_height = radius * 2;
    } break;
    case BGraphicsItem::ItemType::Concentric_Circle: {
        if (isSpecial) {
            m_radius = sqrt(pow(m_center.x() - p.x(), 2) + pow(m_center.y() - p.y(), 2)) * 2;
        } else {
            qreal radius = sqrt(pow(m_center.x() - p.x(), 2) + pow(m_center.y() - p.y(), 2));
            m_width = m_height = radius * 2;
        }
    } break;
    case BGraphicsItem::ItemType::Ellipse:
    case BGraphicsItem::ItemType::Rectangle:
    case BGraphicsItem::ItemType::Square:
    case BGraphicsItem::ItemType::Pill: {
        m_width = abs(p.x()) * 2;
        m_height = abs(p.y()) * 2;
    } break;
    case BGraphicsItem::ItemType::Chamfer: {
        if (isSpecial) {
            qreal dw = m_width/2 - p.x();
            qreal dy = m_height/2 + p.y();
            m_radius = dw >= dy ? dw : dy;
        } else {
            m_width = abs(p.x()) * 2;
            m_height = abs(p.y()) * 2;

            if ( m_width >= m_height ) {
                m_pointList.at(2)->setPoint(QPointF(m_width/2, -m_height/2 + m_radius));
            } else {
                m_pointList.at(2)->setPoint(QPointF(m_width/2 - m_radius, -m_height/2));
            }
        }
    } break;
    case BGraphicsItem::ItemType::Polygon: break;
    default: break;
    }
  
    updateScreen();
}

void BGraphicsItem::updateItemEdge()
{
    // fourth step - roll the wheel to modify the edge
    if ( m_type != BGraphicsItem::ItemType::Polygon ) {
        if ( m_type == BGraphicsItem::ItemType::Circle )
        {
            m_pointList.at(1)->setPoint(QPointF(m_width/2/sqrt(2), m_height/2/sqrt(2)));
        }
        else if ( m_type == BGraphicsItem::ItemType::Concentric_Circle )
        {
            m_pointList.at(1)->setPoint(QPointF(m_width/2/sqrt(2), m_height/2/sqrt(2)));
            m_pointList.at(2)->setPoint(QPointF(m_radius/2/sqrt(2), m_radius/2/sqrt(2)));
        }
        else if ( m_type == BGraphicsItem::ItemType::Ellipse || m_type == BGraphicsItem::ItemType::Rectangle ||
                  m_type == BGraphicsItem::ItemType::Square || m_type == BGraphicsItem::ItemType::Pill )
        {
            m_pointList.at(1)->setPoint(QPointF(m_width/2, m_height/2));
        }
        else if ( m_type == BGraphicsItem::ItemType::Chamfer )
        {
            m_pointList.at(1)->setPoint(QPointF(m_width/2, m_height/2));
            if ( m_width >= m_height ) {
                m_pointList.at(2)->setPoint(QPointF(m_width/2, -m_height/2 + m_radius));
            } else {
                m_pointList.at(2)->setPoint(QPointF(m_width/2 - m_radius, -m_height/2));
            }
        }

        this->scene()->update();
    }
    updateScreen();
}

void BGraphicsItem::updateEdgeInPolygon(QPointF origin, QPointF end)
{
    m_list.clear();
    for ( int i = 0; i < m_pointList.size(); ++i )
    {
        if ( m_pointList.at(i)->getPoint() == origin ) {
            m_pointList.at(i)->setPoint(end);
        }
        m_list.append(m_pointList.at(i)->getPoint());
    }

    m_center = getCentroid(m_list);
    m_pointList.at(m_pointList.size()-1)->setPoint(m_center);

    getMaxLength();
}

void BGraphicsItem::updatePenType(bool isDottedLine)
{
    if ( m_textItem.text() != "" ) {
        return;
    }

    m_isDottedLine = isDottedLine;
    m_pen_noSelected.setStyle(m_isDottedLine ? Qt::DotLine : Qt::SolidLine);
    m_pen_isSelected.setStyle(m_isDottedLine ? Qt::DotLine : Qt::SolidLine);
}

void BGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->buttons() == Qt::LeftButton && !m_isLocked ) {
        this->setPen(m_pen_isSelected);
        this->m_isSelected = true;
    }

    QGraphicsItem::mousePressEvent(event);
}

void BGraphicsItem::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    this->setPen(m_pen_noSelected);
    this->m_isSelected = false;
}

void BGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if ( !m_isLocked && GlobalValue::com_tp == 1 ) {
        updatePenType(!this->m_isDottedLine);
    }

    QAbstractGraphicsShapeItem::mouseDoubleClickEvent(event);
}

void BGraphicsItem::keyPressEvent(QKeyEvent *event)
{
    if (!m_isLocked) {
        switch ( event->key() )
        {
        case Qt::Key_Up:
        case Qt::Key_W: {
            this->moveBy(0, -1);
        } break;
        case Qt::Key_Down:
        case Qt::Key_S: {
            this->moveBy(0, 1);
        } break;
        case Qt::Key_Left:
        case Qt::Key_A: {
            this->moveBy(-1, 0);
        } break;
        case Qt::Key_Right:
        case Qt::Key_D: {
            this->moveBy(1, 0);
        } break;
        case Qt::Key_Delete: {
            if ( GlobalValue::com_tp == 1 ) { emit removeMyself(this); }
        } break;
        case Qt::Key_Enter:
        case Qt::Key_L:
        case Qt::Key_F1:
        case Qt::Key_P:
        case Qt::Key_J:
        case Qt::Key_B:
        case Qt::Key_F12: {
            QAbstractGraphicsShapeItem::keyPressEvent(event);
        }
        default: break;
        }
    }
}

void BGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (!m_isLocked) {
        double value = GlobalValue::gra_pixel/10;

        switch (m_type)
        {
        case BGraphicsItem::ItemType::Concentric_Circle: {
            if ( event->delta() > 0 ) { m_radius += value; }
            if ( event->delta() < 0 && m_radius > value*3 ) { m_radius -= value; }
        }
        case BGraphicsItem::ItemType::Circle:
        case BGraphicsItem::ItemType::Ellipse:
        case BGraphicsItem::ItemType::Rectangle:
        case BGraphicsItem::ItemType::Square:
        case BGraphicsItem::ItemType::Pill: {
            if ( event->delta() > 0 ) { m_width += value; m_height += value; }
            if ( event->delta() < 0 && m_width > value*5 && m_height > value*5 ) { m_width -= value; m_height -= value; }
        } break;
        case BGraphicsItem::ItemType::Chamfer: {
            if ( event->delta() > 0 ) { m_width += value; m_height += value; }
            if ( event->delta() < 0 && m_width > value*5 && m_height > value*5 ) {
                m_width -= value; m_height -= value;
                m_width = m_width < m_radius*2 ? m_radius*2 : m_width;
                m_height = m_height < m_radius*2 ? m_radius*2 : m_height;
            }
        } break;
        case BGraphicsItem::ItemType::Polygon: {} break;
        default: break;
        }

        updateScreen();
        updateItemEdge();
    }
}

//------------------------------------------------------------------------------

BSyncytia::BSyncytia(ItemType type, qreal width, qreal height, QString text) : BGraphicsItem(type)
{
    m_width = width;
    m_height = height;
    m_textItem.setText(text);

    setTextPos( (-1)*m_width/2, (-1)*m_height/2 - getTextHeight() );

    // first step - set m_width and m_height
    if ( m_type != BGraphicsItem::ItemType::Polygon ) {
        m_center = QPointF(0, 0);
        m_pointList.append(new BPointItem(this, m_center, BPointItem::Center));

        QPointF m_edge;
        if ( m_type == BGraphicsItem::ItemType::Circle || m_type == BGraphicsItem::ItemType::Concentric_Circle )
        {
            m_edge = QPointF(width/2/sqrt(2), height/2/sqrt(2));
        }
        else if ( m_type == BGraphicsItem::ItemType::Ellipse || m_type == BGraphicsItem::ItemType::Rectangle ||
                  m_type == BGraphicsItem::ItemType::Square || m_type == BGraphicsItem::ItemType::Pill ||
                  m_type == BGraphicsItem::ItemType::Chamfer )
        {
            m_edge = QPointF(width/2, height/2);
        }

        BPointItem *edge = new BPointItem(this, m_edge, BPointItem::Edge);
        edge->setParentItem(this);
        m_pointList.append(edge);
        m_pointList.setRandColor();

        is_create_finished = true;
    } else {
        is_create_finished = false;
    }
}

QRectF BSyncytia::boundingRect() const
{
    int penWidth = m_pen_isSelected.width();

    if (m_type == BGraphicsItem::ItemType::Polygon) {
        return QRectF ( m_center.x() - m_width/2 - penWidth, m_center.y() - m_height/2 - penWidth,
                        m_width + penWidth*2, m_height + penWidth*2 );
    }

    if (m_width >= m_radius) {
        return QRectF ( (-1)*m_width/2 - penWidth, (-1)*m_height/2 - penWidth,
                        m_width + penWidth*2, m_height + penWidth*2 );
    } else {
        return QRectF ( (-1)*m_radius/2 - penWidth, (-1)*m_radius/2 - penWidth,
                        m_radius + penWidth*2, m_radius + penWidth*2 );
    }
}

void BSyncytia::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if ( m_isSelected ) {
        painter->setPen(m_pen_isSelected);
    } else {
        painter->setPen(m_pen_noSelected);
    }

    QRectF rect( (-1)*m_width/2, (-1)*m_height/2, m_width, m_height );

    switch (m_type) {
    case BGraphicsItem::ItemType::Circle: { painter->drawEllipse(rect); } break;
    case BGraphicsItem::ItemType::Ellipse: { painter->drawEllipse(rect); } break;
    case BGraphicsItem::ItemType::Concentric_Circle: {
        painter->drawEllipse(rect);
        painter->drawEllipse(QRectF( (-1)*m_radius/2, (-1)*m_radius/2, m_radius, m_radius ));
    } break;
    case BGraphicsItem::ItemType::Rectangle: { painter->drawRect(rect); } break;
    case BGraphicsItem::ItemType::Square: { painter->drawRect(rect); } break;
    case BGraphicsItem::ItemType::Polygon: {
        if (m_list.size() > 1) {
            if (is_create_finished) {
                for (int i = 1; i < m_list.size()-1; i++)
                {
                    painter->drawLine(m_list.at(i-1), m_list.at(i));
                }

                painter->drawLine(m_list.at(m_list.size()-2), m_list.at(0));
            } else {
                for (int i = 1; i < m_list.size(); i++)
                {
                    painter->drawLine(m_list.at(i-1), m_list.at(i));
                }
            }
        }
    } break;
    case BGraphicsItem::ItemType::Pill: { painter->drawRoundedRect(rect, m_height/2, m_height/2); } break;
    case BGraphicsItem::ItemType::Chamfer: { painter->drawRoundedRect(rect, m_radius, m_radius); } break;
    default: break;
    }
}

void BSyncytia::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if ( !this->isSelected() )
        return;

    if ( GlobalValue::com_tp == 1 )
    {
        QMenu* menu = new QMenu();
        QDoubleSpinBox* radius1_spinBox = new QDoubleSpinBox(menu);
        QDoubleSpinBox* radius2_spinBox = new QDoubleSpinBox(menu);
        QDoubleSpinBox* width_spinBox = new QDoubleSpinBox(menu);
        QDoubleSpinBox* height_spinBox = new QDoubleSpinBox(menu);

        qreal temp_r = m_radius;
        qreal temp_w = m_width;
        qreal temp_h = m_height;

        switch (m_type)
        {
        case BGraphicsItem::ItemType::Concentric_Circle: {
            radius1_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
            radius1_spinBox->setRange(0.1, 10000);
            radius1_spinBox->setPrefix("d1: ");
            radius1_spinBox->setSuffix(" mm");
            radius1_spinBox->setSingleStep(0.1);
            radius1_spinBox->setValue(m_radius/GlobalValue::gra_pixel);
            connect(radius1_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
                m_radius = v*GlobalValue::gra_pixel;
                updateScreen();
                updateItemEdge();
            });

            QWidgetAction* radius1_widgetAction = new QWidgetAction(menu);
            radius1_widgetAction->setDefaultWidget(radius1_spinBox);
            menu->addAction(radius1_widgetAction);
        }
        case BGraphicsItem::ItemType::Circle: {
            radius2_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
            radius2_spinBox->setRange(0.1, 10000);
            if (m_type == BGraphicsItem::ItemType::Concentric_Circle) {
                radius2_spinBox->setPrefix("d2: ");
            } else {
                radius2_spinBox->setPrefix("d: ");
            }
            radius2_spinBox->setSuffix(" mm");
            radius2_spinBox->setSingleStep(0.1);
            radius2_spinBox->setValue(m_width/GlobalValue::gra_pixel);
            connect(radius2_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
                m_width = v*GlobalValue::gra_pixel;
                m_height = v*GlobalValue::gra_pixel;
                updateScreen();
                updateItemEdge();
            });

            QWidgetAction* radius2_widgetAction = new QWidgetAction(menu);
            radius2_widgetAction->setDefaultWidget(radius2_spinBox);
            menu->addAction(radius2_widgetAction);
        } break;
        case BGraphicsItem::ItemType::Square: {
            width_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
            width_spinBox->setRange(0.1, 10000);
            width_spinBox->setPrefix("L: ");
            width_spinBox->setSuffix(" mm");
            width_spinBox->setSingleStep(0.1);
            width_spinBox->setValue(m_width/GlobalValue::gra_pixel);
            connect(width_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
                m_width = v*GlobalValue::gra_pixel;
                m_height = v*GlobalValue::gra_pixel;
                updateScreen();
                updateItemEdge();
            });

            QWidgetAction* width_widgetAction = new QWidgetAction(menu);
            width_widgetAction->setDefaultWidget(width_spinBox);
            menu->addAction(width_widgetAction);
        } break;
        case BGraphicsItem::ItemType::Chamfer: {
            radius1_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
            radius1_spinBox->setRange(0, 10000);
            radius1_spinBox->setPrefix("r: ");
            radius1_spinBox->setSuffix(" mm");
            radius1_spinBox->setSingleStep(0.1);
            radius1_spinBox->setValue(m_radius/GlobalValue::gra_pixel);
            connect(radius1_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
                double temp = m_width >= m_height ? m_height/2/GlobalValue::gra_pixel : m_width/2/GlobalValue::gra_pixel;
                if ( v > temp ) {
                    m_radius = temp*GlobalValue::gra_pixel;
                } else {
                    m_radius = v*GlobalValue::gra_pixel;
                }
                updateScreen();
                updateItemEdge();
            });

            QWidgetAction* radius1_widgetAction = new QWidgetAction(menu);
            radius1_widgetAction->setDefaultWidget(radius1_spinBox);
            menu->addAction(radius1_widgetAction);
        }
        case BGraphicsItem::ItemType::Ellipse:
        case BGraphicsItem::ItemType::Rectangle:
        case BGraphicsItem::ItemType::Pill: {
            width_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
            width_spinBox->setRange(0.1, 10000);
            width_spinBox->setPrefix("w: ");
            width_spinBox->setSuffix(" mm");
            width_spinBox->setSingleStep(0.1);
            width_spinBox->setValue(m_width/GlobalValue::gra_pixel);
            connect(width_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
                if ( m_type == BGraphicsItem::ItemType::Chamfer && v < m_radius*2/GlobalValue::gra_pixel ) {
                    m_width = m_radius*2;
                } else if ( m_type == BGraphicsItem::ItemType::Pill && v < m_height/GlobalValue::gra_pixel ) {
                    m_width = m_height;
                } else {
                    m_width = v*GlobalValue::gra_pixel;
                }
                updateScreen();
                updateItemEdge();
            });

            height_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
            height_spinBox->setRange(0.1, 10000);
            height_spinBox->setPrefix("h: ");
            height_spinBox->setSuffix(" mm");
            height_spinBox->setSingleStep(0.1);
            height_spinBox->setValue(m_height/GlobalValue::gra_pixel);
            connect(height_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
                if ( m_type == BGraphicsItem::ItemType::Chamfer && v < m_radius*2/GlobalValue::gra_pixel ) {
                    m_height = m_radius*2;
                } else if ( m_type == BGraphicsItem::ItemType::Pill && v > m_width/GlobalValue::gra_pixel ) {
                    m_height = m_width;
                } else {
                    m_height = v*GlobalValue::gra_pixel;
                }
                updateScreen();
                updateItemEdge();
            });

            QWidgetAction* width_widgetAction = new QWidgetAction(menu);
            width_widgetAction->setDefaultWidget(width_spinBox);
            menu->addAction(width_widgetAction);

            QWidgetAction* height_widgetAction = new QWidgetAction(menu);
            height_widgetAction->setDefaultWidget(height_spinBox);
            menu->addAction(height_widgetAction);
        } break;
        case BGraphicsItem::ItemType::Polygon: break;
        default: break;
        }

        if ( m_type != BGraphicsItem::ItemType::Polygon ) {
            QDoubleSpinBox* scale_spinBox = new QDoubleSpinBox(menu);
            scale_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
            scale_spinBox->setRange(0.1, 10);
            scale_spinBox->setPrefix("s: ");
            scale_spinBox->setSingleStep(0.05);
            scale_spinBox->setValue(1);
            connect(scale_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
                switch (m_type) {
                case BGraphicsItem::ItemType::Concentric_Circle: {
                    m_radius = temp_r * v;
                    radius1_spinBox->setValue(m_radius/GlobalValue::gra_pixel);
                }
                case BGraphicsItem::ItemType::Circle: {
                    m_width = temp_w * v;
                    m_height = temp_h * v;
                    radius2_spinBox->setValue(m_width/GlobalValue::gra_pixel);
                } break;
                case BGraphicsItem::ItemType::Square: {
                    m_width = temp_w * v;
                    width_spinBox->setValue(m_width/GlobalValue::gra_pixel);
                } break;
                case BGraphicsItem::ItemType::Chamfer:
                case BGraphicsItem::ItemType::Ellipse:
                case BGraphicsItem::ItemType::Rectangle:
                case BGraphicsItem::ItemType::Pill: {
                    m_width = temp_w * v;
                    m_height = temp_h * v;
                    width_spinBox->setValue(m_width/GlobalValue::gra_pixel);
                    height_spinBox->setValue(m_height/GlobalValue::gra_pixel);
                } break;
                default: break;
                }

                updateScreen();
                updateItemEdge();
            });

            QWidgetAction* scale_widgetAction = new QWidgetAction(menu);
            scale_widgetAction->setDefaultWidget(scale_spinBox);
            menu->addAction(scale_widgetAction);
        }

        QSpinBox* rotation_spinBox = new QSpinBox(menu);
        rotation_spinBox->setStyleSheet("QSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
        rotation_spinBox->setRange(-360, 360);
        rotation_spinBox->setPrefix("rotate: ");
        rotation_spinBox->setSuffix("");
        rotation_spinBox->setSingleStep(1);
        rotation_spinBox->setValue(m_rotation);
        connect(rotation_spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int v){
            m_rotation = v;
            updateScreen();
        });

        QWidgetAction* rotation_widgetAction = new QWidgetAction(menu);
        rotation_widgetAction->setDefaultWidget(rotation_spinBox);
        menu->addAction(rotation_widgetAction);

        menu->exec(QCursor::pos());
        delete menu;
    }
    else
    {
        QMenu* menu = new QMenu();

        QDoubleSpinBox* width_spinBox = new QDoubleSpinBox(menu);
        width_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
        width_spinBox->setRange(0.1, 10000);
        width_spinBox->setPrefix("L: ");
        width_spinBox->setSuffix(" mm");
        width_spinBox->setSingleStep(0.1);
        width_spinBox->setValue(m_width/GlobalValue::gra_pixel);
        connect(width_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
            m_width = v*GlobalValue::gra_pixel;
            m_height = v*GlobalValue::gra_pixel;
            updateScreen();
            updateItemEdge();
        });

        QWidgetAction* width_widgetAction = new QWidgetAction(menu);
        width_widgetAction->setDefaultWidget(width_spinBox);

        if ( m_autoType != BGraphicsItem::AutoType::Auto_Recognise ) {
            menu->addAction(width_widgetAction);
        }


        //------------------------------------------------------------------------------

        QDoubleSpinBox* auto_width_spinBox = new QDoubleSpinBox(menu);
        auto_width_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
        auto_width_spinBox->setRange(0.1, 10000);
        auto_width_spinBox->setPrefix("w: ");
        auto_width_spinBox->setSuffix(" mm");
        auto_width_spinBox->setSingleStep(0.1);
        auto_width_spinBox->setValue(m_autoWidth/GlobalValue::gra_pixel);
        connect(auto_width_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
            m_autoWidth = v*GlobalValue::gra_pixel;
        });

        QDoubleSpinBox* auto_height_spinBox = new QDoubleSpinBox(menu);
        auto_height_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
        auto_height_spinBox->setRange(0.1, 10000);
        auto_height_spinBox->setPrefix("h: ");
        auto_height_spinBox->setSuffix(" mm");
        auto_height_spinBox->setSingleStep(0.1);
        auto_height_spinBox->setValue(m_autoHeight/GlobalValue::gra_pixel);
        connect(auto_height_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
            m_autoHeight = v*GlobalValue::gra_pixel;
        });

        QDoubleSpinBox* auto_circleRadius_spinBox = new QDoubleSpinBox(menu);
        auto_circleRadius_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
        auto_circleRadius_spinBox->setRange(0.1, 10000);
        auto_circleRadius_spinBox->setPrefix("d: ");
        auto_circleRadius_spinBox->setSuffix(" mm");
        auto_circleRadius_spinBox->setSingleStep(0.1);
        auto_circleRadius_spinBox->setValue(m_autoCircleRadius/GlobalValue::gra_pixel);
        connect(auto_circleRadius_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
            m_autoCircleRadius = v*GlobalValue::gra_pixel;
        });

        QDoubleSpinBox* auto_chamferRadius_spinBox = new QDoubleSpinBox(menu);
        auto_chamferRadius_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
        auto_chamferRadius_spinBox->setRange(0.1, 10000);
        auto_chamferRadius_spinBox->setPrefix("r: ");
        auto_chamferRadius_spinBox->setSuffix(" mm");
        auto_chamferRadius_spinBox->setSingleStep(0.1);
        auto_chamferRadius_spinBox->setValue(m_autoChamferRadius/GlobalValue::gra_pixel);
        connect(auto_chamferRadius_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
            m_autoChamferRadius = v*GlobalValue::gra_pixel;
        });

        QDoubleSpinBox* auto_dis1_spinBox = new QDoubleSpinBox(menu);
        auto_dis1_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
        auto_dis1_spinBox->setRange(-10000, 10000);
        auto_dis1_spinBox->setPrefix("d1: ");
        auto_dis1_spinBox->setSuffix(" mm");
        auto_dis1_spinBox->setSingleStep(0.1);
        auto_dis1_spinBox->setValue(m_distance1/GlobalValue::gra_pixel);
        connect(auto_dis1_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
            m_distance1 = v*GlobalValue::gra_pixel;
        });

        QDoubleSpinBox* auto_dis2_spinBox = new QDoubleSpinBox(menu);
        auto_dis2_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
        auto_dis2_spinBox->setRange(-10000, 10000);
        auto_dis2_spinBox->setPrefix("d2: ");
        auto_dis2_spinBox->setSuffix(" mm");
        auto_dis2_spinBox->setSingleStep(0.1);
        auto_dis2_spinBox->setValue(m_distance2/GlobalValue::gra_pixel);
        connect(auto_dis2_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
            m_distance2 = v*GlobalValue::gra_pixel;
        });

        QDoubleSpinBox* auto_scale_spinBox = new QDoubleSpinBox(menu);
        auto_scale_spinBox->setStyleSheet("QDoubleSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
        auto_scale_spinBox->setRange(0.5, 1);
        auto_scale_spinBox->setPrefix("s: ");
        auto_scale_spinBox->setSuffix("");
        auto_scale_spinBox->setSingleStep(0.1);
        auto_scale_spinBox->setValue(m_autoScale);
        connect(auto_scale_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double v){
            m_autoScale = v;
        });

        QSpinBox* auto_pixel_spinBox = new QSpinBox(menu);
        auto_pixel_spinBox->setStyleSheet("QSpinBox{ width:150px; height:30px; font-size:16px; font-weight:bold; }");
        auto_pixel_spinBox->setRange(1, 4);
        auto_pixel_spinBox->setPrefix("erode: ");
        auto_pixel_spinBox->setSuffix("");
        auto_pixel_spinBox->setSingleStep(1);
        auto_pixel_spinBox->setValue(m_indentPixel);
        connect(auto_pixel_spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int v){
            m_indentPixel = v;
        });

        QWidgetAction* auto_width_widgetAction = new QWidgetAction(menu);
        auto_width_widgetAction->setDefaultWidget(auto_width_spinBox);

        QWidgetAction* auto_height_widgetAction = new QWidgetAction(menu);
        auto_height_widgetAction->setDefaultWidget(auto_height_spinBox);

        QWidgetAction* auto_circleRadius_widgetAction = new QWidgetAction(menu);
        auto_circleRadius_widgetAction->setDefaultWidget(auto_circleRadius_spinBox);

        QWidgetAction* auto_chamferRadius_widgetAction = new QWidgetAction(menu);
        auto_chamferRadius_widgetAction->setDefaultWidget(auto_chamferRadius_spinBox);

        QWidgetAction* auto_dis1_widgetAction = new QWidgetAction(menu);
        auto_dis1_widgetAction->setDefaultWidget(auto_dis1_spinBox);

        QWidgetAction* auto_dis2_widgetAction = new QWidgetAction(menu);
        auto_dis2_widgetAction->setDefaultWidget(auto_dis2_spinBox);

        QWidgetAction* auto_scale_widgetAction = new QWidgetAction(menu);
        auto_scale_widgetAction->setDefaultWidget(auto_scale_spinBox);

        QWidgetAction* auto_pixel_widgetAction = new QWidgetAction(menu);
        auto_pixel_widgetAction->setDefaultWidget(auto_pixel_spinBox);

        //------------------------------------------------------------------------------

        QRadioButton *value = new QRadioButton("Value");
        QRadioButton *scale = new QRadioButton("Scale");

        // 初始化
        if (is_value) {
            value->setChecked(true);
            if ( m_autoType != BGraphicsItem::AutoType::Auto_Recognise ) {
                if ( m_autoType == BGraphicsItem::AutoType::Auto_Circle ) {
                    menu->addAction(auto_circleRadius_widgetAction);
                } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Chamfer ) {
                    menu->addAction(auto_width_widgetAction);
                    menu->addAction(auto_height_widgetAction);
                    menu->addAction(auto_chamferRadius_widgetAction);
                } else if ( m_autoType == BGraphicsItem::AutoType::Auto_RoundEdgeRec ) {
                    menu->addAction(auto_dis1_widgetAction);
                    menu->addAction(auto_dis2_widgetAction);
                    menu->addAction(auto_circleRadius_widgetAction);
                } else {
                    menu->addAction(auto_width_widgetAction);
                    menu->addAction(auto_height_widgetAction);
                }
            } else {
                menu->addAction(auto_pixel_widgetAction);
            }

        } else {
            scale->setChecked(true);
            if ( m_autoType != BGraphicsItem::AutoType::Auto_Recognise ) {
                menu->addAction(auto_scale_widgetAction);
            } else {
                menu->addAction(auto_pixel_widgetAction);
            }
        }

        QHBoxLayout *patternLayout = new QHBoxLayout;
        patternLayout->addWidget(value);
        patternLayout->addWidget(scale);
        patternLayout->addStretch(1);
        QGroupBox *patternBox = new QGroupBox(menu);
        patternBox->setLayout(patternLayout);

        QWidgetAction* patternBox_widgetAction = new QWidgetAction(menu);
        patternBox_widgetAction->setDefaultWidget(patternBox);

        if ( m_autoType != BGraphicsItem::AutoType::Auto_Recognise ) {
            menu->addAction(patternBox_widgetAction);
        }

        connect(value, &QRadioButton::clicked, [&](){
            if (!is_value) {
                is_value = true;
                menu->removeAction(auto_scale_widgetAction);
                if ( m_autoType == BGraphicsItem::AutoType::Auto_Circle ) {
                    menu->insertAction(patternBox_widgetAction, auto_circleRadius_widgetAction);
                } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Chamfer ) {
                    menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                    menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                    menu->insertAction(patternBox_widgetAction, auto_chamferRadius_widgetAction);
                } else if ( m_autoType == BGraphicsItem::AutoType::Auto_RoundEdgeRec ) {
                    menu->insertAction(patternBox_widgetAction, auto_dis1_widgetAction);
                    menu->insertAction(patternBox_widgetAction, auto_dis2_widgetAction);
                    menu->insertAction(patternBox_widgetAction, auto_circleRadius_widgetAction);
                }  else {
                    menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                    menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                }
            }
        });

        connect(scale, &QRadioButton::clicked, [&](){
            if (is_value) {
                is_value = false;
                if ( m_autoType == BGraphicsItem::AutoType::Auto_Circle ) {
                    menu->removeAction(auto_circleRadius_widgetAction);
                } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Chamfer ) {
                    menu->removeAction(auto_width_widgetAction);
                    menu->removeAction(auto_height_widgetAction);
                    menu->removeAction(auto_chamferRadius_widgetAction);
                } else if ( m_autoType == BGraphicsItem::AutoType::Auto_RoundEdgeRec ) {
                    menu->removeAction(auto_dis1_widgetAction);
                    menu->removeAction(auto_dis2_widgetAction);
                    menu->removeAction(auto_circleRadius_widgetAction);
                } else {
                    menu->removeAction(auto_width_widgetAction);
                    menu->removeAction(auto_height_widgetAction);
                }
                if ( m_autoType != BGraphicsItem::AutoType::Auto_Recognise ) {
                    menu->insertAction(patternBox_widgetAction, auto_scale_widgetAction);
                }
            }
        });

        //------------------------------------------------------------------------------

        QRadioButton *circle = new QRadioButton(GlobalString::graphics_auto_circle);
        QRadioButton *ellipse = new QRadioButton(GlobalString::graphics_auto_ellipse);
        QRadioButton *pill = new QRadioButton(GlobalString::graphics_auto_pill);
        QRadioButton *chamfer = new QRadioButton(GlobalString::graphics_auto_chamfer);
        QRadioButton *graphics_auto_roundEdgeRec = new QRadioButton(GlobalString::graphics_auto_roundEdgeRec);
        QRadioButton *rotationRec = new QRadioButton(GlobalString::graphics_auto_rotateRec);
        QRadioButton *auto_recognise = new QRadioButton(GlobalString::graphics_auto_recognise);

        QVBoxLayout *vbox = new QVBoxLayout;
        vbox->addWidget(circle);
        vbox->addWidget(ellipse);
        vbox->addWidget(pill);
        vbox->addWidget(chamfer);
        vbox->addWidget(graphics_auto_roundEdgeRec);
        vbox->addWidget(rotationRec);
        vbox->addWidget(auto_recognise);
        vbox->addStretch(1);
        QGroupBox *box2 = new QGroupBox(menu);
        box2->setLayout(vbox);

        QWidgetAction* type_widgetAction = new QWidgetAction(menu);
        type_widgetAction->setDefaultWidget(box2);
        menu->addAction(type_widgetAction);

        connect(circle, &QRadioButton::clicked, [&](){
            if ( m_autoType != BGraphicsItem::AutoType::Auto_Circle ) {

                if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                    menu->removeAction(auto_pixel_widgetAction);
                }

                if (is_value) {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Chamfer ) {
                        menu->removeAction(auto_width_widgetAction);
                        menu->removeAction(auto_height_widgetAction);
                        menu->removeAction(auto_chamferRadius_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_RoundEdgeRec ) {
                        menu->removeAction(auto_dis1_widgetAction);
                        menu->removeAction(auto_dis2_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction,patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                    } else {
                        menu->removeAction(auto_width_widgetAction);
                        menu->removeAction(auto_height_widgetAction);
                    }

                    menu->insertAction(patternBox_widgetAction, auto_circleRadius_widgetAction);
                } else {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction, patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_scale_widgetAction);
                    }
                }

                m_autoType = BGraphicsItem::AutoType::Auto_Circle;
            }
        });
        connect(ellipse, &QRadioButton::clicked, [&](){
            if ( m_autoType != BGraphicsItem::AutoType::Auto_Ellipse ) {

                if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                    menu->removeAction(auto_pixel_widgetAction);
                }

                if (is_value) {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Circle ) {
                        menu->removeAction(auto_circleRadius_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Chamfer ) {
                        menu->removeAction(auto_chamferRadius_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_RoundEdgeRec ) {
                        menu->removeAction(auto_dis1_widgetAction);
                        menu->removeAction(auto_dis2_widgetAction);
                        menu->removeAction(auto_circleRadius_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction,patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                    }
                } else {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction, patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_scale_widgetAction);
                    }
                }

                m_autoType = BGraphicsItem::AutoType::Auto_Ellipse;
            }
        });
        connect(pill, &QRadioButton::clicked, [&](){
            if ( m_autoType != BGraphicsItem::AutoType::Auto_Pill ) {

                if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                    menu->removeAction(auto_pixel_widgetAction);
                }

                if (is_value) {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Circle ) {
                        menu->removeAction(auto_circleRadius_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Chamfer ) {
                        menu->removeAction(auto_chamferRadius_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_RoundEdgeRec ) {
                        menu->removeAction(auto_dis1_widgetAction);
                        menu->removeAction(auto_dis2_widgetAction);
                        menu->removeAction(auto_circleRadius_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction,patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                    }
                } else {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction, patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_scale_widgetAction);
                    }
                }

                m_autoType = BGraphicsItem::AutoType::Auto_Pill;
            }
        });
        connect(chamfer, &QRadioButton::clicked, [&](){
            if ( m_autoType != BGraphicsItem::AutoType::Auto_Chamfer ) {

                if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                    menu->removeAction(auto_pixel_widgetAction);
                }

                if (is_value) {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Circle ) {
                        menu->removeAction(auto_circleRadius_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_chamferRadius_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_RoundEdgeRec ) {
                        menu->removeAction(auto_dis1_widgetAction);
                        menu->removeAction(auto_dis2_widgetAction);
                        menu->removeAction(auto_circleRadius_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_chamferRadius_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction,patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_chamferRadius_widgetAction);
                    }else {
                        menu->insertAction(patternBox_widgetAction, auto_chamferRadius_widgetAction);
                    }
                } else {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction, patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_scale_widgetAction);
                    }
                }

                m_autoType = BGraphicsItem::AutoType::Auto_Chamfer;
            }
        });
        connect(graphics_auto_roundEdgeRec, &QRadioButton::clicked, [&](){
            if ( m_autoType != BGraphicsItem::AutoType::Auto_RoundEdgeRec ) {

                if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                    menu->removeAction(auto_pixel_widgetAction);
                }

                if (is_value) {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Circle ) {
                        menu->removeAction(auto_circleRadius_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_dis1_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_dis2_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_circleRadius_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Chamfer ) {
                        menu->removeAction(auto_width_widgetAction);
                        menu->removeAction(auto_height_widgetAction);
                        menu->removeAction(auto_chamferRadius_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_dis1_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_dis2_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_circleRadius_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction,patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_dis1_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_dis2_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_circleRadius_widgetAction);
                    } else {
                        menu->removeAction(auto_width_widgetAction);
                        menu->removeAction(auto_height_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_dis1_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_dis2_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_circleRadius_widgetAction);
                    }
                } else {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction, patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_scale_widgetAction);
                    }
                }

                m_autoType = BGraphicsItem::AutoType::Auto_RoundEdgeRec;
            }
        });
        connect(rotationRec, &QRadioButton::clicked, [&](){
            if ( m_autoType != BGraphicsItem::AutoType::Auto_RotateRec ) {

                if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                    menu->removeAction(auto_pixel_widgetAction);
                }

                if (is_value) {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Circle ) {
                        menu->removeAction(auto_circleRadius_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Chamfer ) {
                        menu->removeAction(auto_chamferRadius_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_RoundEdgeRec ) {
                        menu->removeAction(auto_dis1_widgetAction);
                        menu->removeAction(auto_dis2_widgetAction);
                        menu->removeAction(auto_circleRadius_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction,patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_height_widgetAction);
                    }
                } else {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Recognise ) {
                        menu->insertAction(type_widgetAction, patternBox_widgetAction);
                        menu->insertAction(patternBox_widgetAction, width_widgetAction);
                        menu->insertAction(patternBox_widgetAction, auto_scale_widgetAction);
                    }
                }

                m_autoType = BGraphicsItem::AutoType::Auto_RotateRec;
            }
        });
        connect(auto_recognise, &QAbstractButton::clicked, [&](){
            if ( m_autoType != BGraphicsItem::AutoType::Auto_Recognise ) {
                if ( is_value ) {
                    if ( m_autoType == BGraphicsItem::AutoType::Auto_Circle ) {
                        menu->removeAction(auto_circleRadius_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_Chamfer ) {
                        menu->removeAction(auto_width_widgetAction);
                        menu->removeAction(auto_height_widgetAction);
                        menu->removeAction(auto_chamferRadius_widgetAction);
                    } else if ( m_autoType == BGraphicsItem::AutoType::Auto_RoundEdgeRec ) {
                        menu->removeAction(auto_dis1_widgetAction);
                        menu->removeAction(auto_dis2_widgetAction);
                        menu->removeAction(auto_circleRadius_widgetAction);
                    } else {
                        menu->removeAction(auto_width_widgetAction);
                        menu->removeAction(auto_height_widgetAction);
                    }
                    menu->removeAction(width_widgetAction);
                    menu->removeAction(patternBox_widgetAction);
                } else {
                    menu->removeAction(width_widgetAction);
                    menu->removeAction(patternBox_widgetAction);
                    menu->removeAction(auto_scale_widgetAction);
                }

                menu->insertAction(type_widgetAction, auto_pixel_widgetAction);
            }

            m_autoType = BGraphicsItem::AutoType::Auto_Recognise;
        });

        switch (m_autoType)
        {
        case BGraphicsItem::AutoType::Auto_Circle: circle->setChecked(true); break;
        case BGraphicsItem::AutoType::Auto_Ellipse: ellipse->setChecked(true); break;
        case BGraphicsItem::AutoType::Auto_Pill: pill->setChecked(true); break;
        case BGraphicsItem::AutoType::Auto_Chamfer: chamfer->setChecked(true); break;
        case BGraphicsItem::AutoType::Auto_RoundEdgeRec: graphics_auto_roundEdgeRec->setChecked(true); break;
        case BGraphicsItem::AutoType::Auto_RotateRec: rotationRec->setChecked(true); break;
        case BGraphicsItem::AutoType::Auto_Recognise: auto_recognise->setChecked(true); break;
        default: break;
        }
        menu->exec(QCursor::pos());
        delete menu;
    }

    QGraphicsItem::contextMenuEvent(event);
}
