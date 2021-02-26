#include "bqgraphicsscene.h"
#include "globalfun.h"

#include <QMenu>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>

BQGraphicsScene::BQGraphicsScene(QObject *parent) : QGraphicsScene(parent)
{
    is_creating_BPolygon = false;
    is_ignore_contextMenu = false;
    is_homo_test = false;

    auto_type = 0;

    m_mask = new QGraphicsPixmapItem(&m_unlivePixmap);

    QFont font;
    font.setPixelSize(60);
    m_simpleText.setFont(font);
    m_simpleText.setBrush(Qt::yellow);
    m_simpleText.setParentItem(&m_unlivePixmap);

    this->addItem(&m_unlivePixmap);
    this->setBackgroundBrush(Qt::gray);

    QString file = GlobalFun::getCurrentPath() + "/item.ini";
    if ( GlobalFun::isFileExist(file) ) {
        loadItemToScene(file);
    }
}

void BQGraphicsScene::setPixmapSize(int width, int height)
{
    QPixmap pixmap(QSize(width, height));
    pixmap.fill(QColor(50, 50, 50));
    m_unlivePixmap.setPixmap(pixmap);
    m_unlivePixmap.setPos(-width/2, -height/2);
}

void BQGraphicsScene::setMask(QImage image)
{
    m_mask->setPixmap(QPixmap::fromImage(image));
}

QImage BQGraphicsScene::getMask()
{
    return m_mask->pixmap().toImage();
}

void BQGraphicsScene::createBGraphicsItem(BGraphicsItem::ItemType type, QString text)
{
    BSyncytia *item;

    switch (type) {
    case BGraphicsItem::ItemType::Circle: {
        qreal radius = GlobalValue::gra_c_rad;
        item = new BSyncytia(BGraphicsItem::ItemType::Circle, radius, radius, text);
    } break;
    case BGraphicsItem::ItemType::Ellipse: {
        qreal width = GlobalValue::gra_e_wid;
        qreal height = GlobalValue::gra_e_hei;
        item = new BSyncytia(BGraphicsItem::ItemType::Ellipse, width, height, text);
    } break;
    case BGraphicsItem::ItemType::Concentric_Circle: {
        qreal radius1 = GlobalValue::gra_cc_rad_1;
        qreal radius2 = GlobalValue::gra_cc_rad_2;
        item = new BSyncytia(BGraphicsItem::ItemType::Concentric_Circle, radius2, radius2, text);
        item->setRadius(radius1);
    } break;
    case BGraphicsItem::ItemType::Rectangle: {
        qreal width = GlobalValue::gra_r_wid;
        qreal height = GlobalValue::gra_r_hei;
        item = new BSyncytia(BGraphicsItem::ItemType::Rectangle, width, height, text);
    } break;
    case BGraphicsItem::ItemType::Square: {
        qreal length = GlobalValue::gra_s_len;
        item = new BSyncytia(BGraphicsItem::ItemType::Square, length, length, text);
    } break;
    case BGraphicsItem::ItemType::Polygon: {
        is_creating_BPolygon = true;
        item = new BSyncytia(BGraphicsItem::ItemType::Polygon, 0, 0, text);
    } break;
    case BGraphicsItem::ItemType::Pill: {
        qreal width = GlobalValue::gra_p_wid;
        qreal height = GlobalValue::gra_p_hei;
        item = new BSyncytia(BGraphicsItem::ItemType::Pill, width, height, text);
    } break;
    case BGraphicsItem::ItemType::Chamfer: {
        qreal width = GlobalValue::gra_ch_wid;
        qreal height = GlobalValue::gra_ch_hei;
        qreal radius = GlobalValue::gra_ch_rad;
        item = new BSyncytia(BGraphicsItem::ItemType::Chamfer, width, height, text);
        item->setRadius(radius);
    } break;
    default: break;
    }

    connect(item, &BSyncytia::removeMyself, [&](BGraphicsItem *item){ removeItemDFromList(item); });
    connect(item, &BSyncytia::startCreatePolygon, [&](QString text){
        m_list.clear();
        is_creating_BPolygon = true;
        current_polygon = text;
    });
    connect(this, SIGNAL(updatePoint(QString, QList<QPointF>, bool)), item, SLOT(updatePolygon(QString, QList<QPointF>, bool)));

    item->setEnable(true);
    this->addItem(item);
    this->m_graphicsItemList.push_back(item);
}

QList<BGraphicsItem *> BQGraphicsScene::getGraphicsItemList()
{
    return m_graphicsItemList;
}

QGraphicsPixmapItem& BQGraphicsScene::getUnlivePixmapItem()
{
    return m_unlivePixmap;
}

void BQGraphicsScene::updateImage(std::vector<cv::Mat> &list)
{
    m_matList.clear();
    for ( auto &temp : list ) { m_matList.push_back(temp); }

    QImage image = GlobalFun::convertMatToQImage(list.at(0));
    m_unlivePixmap.setPixmap(QPixmap::fromImage(image));
    m_unlivePixmap.setPos((-1)*image.width()/2, (-1)*image.height()/2);
    m_simpleText.setText(QString("1/%1").arg(list.size()));
}

void BQGraphicsScene::saveItemToConfig(QString fileName)
{
    GlobalFun::removeFile(fileName);
    QSettings item(fileName, QSettings::IniFormat);

    item.beginWriteArray("itemList");
    for ( int i = 0; i < m_graphicsItemList.size(); ++i ) {
        item.setArrayIndex(i);

        if ( GlobalValue::com_tp == 1 )
        {
            BGraphicsItem::ItemType type = m_graphicsItemList.at(i)->getType();
            item.setValue("type", (int)type);
            item.setValue("isDottedLine", m_graphicsItemList.at(i)->getIfDottedLine());
            item.setValue("rotation", m_graphicsItemList.at(i)->getRotation());

            switch (type)
            {
            case BGraphicsItem::ItemType::Chamfer:
            case BGraphicsItem::ItemType::Concentric_Circle: {
                item.setValue("radius", m_graphicsItemList.at(i)->getRadius());
            }
            case BGraphicsItem::ItemType::Circle:
            case BGraphicsItem::ItemType::Ellipse:
            case BGraphicsItem::ItemType::Rectangle:
            case BGraphicsItem::ItemType::Square:
            case BGraphicsItem::ItemType::Pill: {
                item.setValue("width", m_graphicsItemList.at(i)->getWidth());
                item.setValue("height", m_graphicsItemList.at(i)->getHeight());
                item.setValue("center", m_graphicsItemList.at(i)->getCurrentCenter());
            } break;
            case BGraphicsItem::ItemType::Polygon: {
                QList<QPointF> list = m_graphicsItemList.at(i)->getPointList();
                item.beginWriteArray("polygon");
                for (int j = 0; j < list.size() - 1; j++)
                {
                    item.setArrayIndex(j);
                    item.setValue("edge", m_graphicsItemList.at(i)->mapToScene( list.at(j) ));
                }
                item.endArray();
            } break;
            default: break;
            }
        }
        else
        {
            item.setValue("width", m_graphicsItemList.at(i)->getWidth());
            item.setValue("height", m_graphicsItemList.at(i)->getHeight());
            item.setValue("center", m_graphicsItemList.at(i)->getCurrentCenter());

            item.setValue("type", (int)m_graphicsItemList.at(i)->getAutoType());
            item.setValue("useScale", m_graphicsItemList.at(i)->getIfUseScale());
            item.setValue("autoWidth", m_graphicsItemList.at(i)->getAutoWidth());
            item.setValue("autoHeight", m_graphicsItemList.at(i)->getAutoHeight());
            item.setValue("cirRadius", m_graphicsItemList.at(i)->getAutoCirRadius());
            item.setValue("chaRadius", m_graphicsItemList.at(i)->getAutoChaRadius());
            item.setValue("dis1", m_graphicsItemList.at(i)->getAutoDis1());
            item.setValue("dis2", m_graphicsItemList.at(i)->getAutoDis2());
            item.setValue("scale", m_graphicsItemList.at(i)->getAutoScale());
        }
    }
    item.endArray();
}

void BQGraphicsScene::loadItemToScene(QString fileName)
{
    clearItem();
    QSettings item(fileName, QSettings::IniFormat);

    int size = item.beginReadArray("itemList");
    for (int i = 0; i < size; ++i) {
        item.setArrayIndex(i);
        createBGraphicsItem(BGraphicsItem::ItemType::Square, "");

        if ( GlobalValue::com_tp == 1 )
        {
            BGraphicsItem::ItemType type = (BGraphicsItem::ItemType)item.value("type").toInt();
            m_graphicsItemList.at(i)->changeType(type);

            bool isDottedLine = item.value("isDottedLine").toBool();
            m_graphicsItemList.at(i)->updatePenType(isDottedLine);

            qreal rotation = item.value("rotation").toDouble();
            m_graphicsItemList.at(i)->setMyRotation(rotation);

            switch (type)
            {
            case BGraphicsItem::ItemType::Chamfer:
            case BGraphicsItem::ItemType::Concentric_Circle: {
                qreal radius = item.value("radius").toDouble();
                m_graphicsItemList.at(i)->setRadius(radius);
            }
            case BGraphicsItem::ItemType::Circle:
            case BGraphicsItem::ItemType::Ellipse:
            case BGraphicsItem::ItemType::Rectangle:
            case BGraphicsItem::ItemType::Square:
            case BGraphicsItem::ItemType::Pill: {
                qreal width = item.value("width").toDouble();
                m_graphicsItemList.at(i)->setWidth(width);

                qreal height = item.value("height").toDouble();
                m_graphicsItemList.at(i)->setHeight(height);

                QPointF center = item.value("center").toPointF();
                m_graphicsItemList.at(i)->setPos(center);
            } break;
            case BGraphicsItem::ItemType::Polygon: {
                int count = item.beginReadArray("polygon");
                QList<QPointF> list;
                for (int j = 0; j < count; ++j)
                {
                    item.setArrayIndex(j);
                    QPointF edge = m_graphicsItemList.at(i)->mapFromScene( item.value("edge").toPointF() );
                    list.append(edge);
                    emit updatePoint(current_polygon, list, false);
                }
                item.endArray();

                emit updatePoint(current_polygon, list, true);
                is_creating_BPolygon = false;
                is_ignore_contextMenu = false;
            } break;
            default: break;
            }
        }
        else
        {
            qreal width = item.value("width").toDouble();
            m_graphicsItemList.at(i)->setWidth(width);

            qreal height = item.value("height").toDouble();
            m_graphicsItemList.at(i)->setHeight(height);

            QPointF center = item.value("center").toPointF();
            m_graphicsItemList.at(i)->setPos(center);

            BGraphicsItem::AutoType type = (BGraphicsItem::AutoType)item.value("type").toInt();
            m_graphicsItemList.at(i)->setAutoType(type);

            bool useScale = item.value("useScale").toBool();
            m_graphicsItemList.at(i)->setIsUseScale(useScale);

            qreal autoWidth = item.value("autoWidth").toDouble();
            m_graphicsItemList.at(i)->setAutoWidth(autoWidth);

            qreal autoHeight = item.value("autoHeight").toDouble();
            m_graphicsItemList.at(i)->setAutoHeight(autoHeight);

            qreal cirRadius = item.value("cirRadius").toDouble();
            m_graphicsItemList.at(i)->setAutoCirRadius(cirRadius);

            qreal chaRadius = item.value("chaRadius").toDouble();
            m_graphicsItemList.at(i)->setAutoChaRadius(chaRadius);

            qreal dis1 = item.value("dis1").toDouble();
            m_graphicsItemList.at(i)->setAutoDis1(dis1);

            qreal dis2 = item.value("dis2").toDouble();
            m_graphicsItemList.at(i)->setAutoDis2(dis2);

            qreal scale = item.value("scale").toDouble();
            m_graphicsItemList.at(i)->setAutoScale(scale);
        }

        m_graphicsItemList.at(i)->updateScreen();
        m_graphicsItemList.at(i)->updateItemEdge();
    }
    item.endArray();
}

//------------------------------------------------------------------------------

void BQGraphicsScene::connectCirclePushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){ createBGraphicsItem(BGraphicsItem::ItemType::Circle, ""); });
}

void BQGraphicsScene::connectEllipsePushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){ createBGraphicsItem(BGraphicsItem::ItemType::Ellipse, ""); });
}

void BQGraphicsScene::connectConCirclePushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){ createBGraphicsItem(BGraphicsItem::ItemType::Concentric_Circle, ""); });
}

void BQGraphicsScene::connectRectanglePushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){ createBGraphicsItem(BGraphicsItem::ItemType::Rectangle, ""); });
}

void BQGraphicsScene::connectSquarePushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){ createBGraphicsItem(BGraphicsItem::ItemType::Square, ""); });
}

void BQGraphicsScene::connectPolygonPushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){
        createBGraphicsItem(BGraphicsItem::ItemType::Polygon, "");
        changeBtnState(false, false);
    });
}

void BQGraphicsScene::connectPillPushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){ createBGraphicsItem(BGraphicsItem::ItemType::Pill, ""); });
}

void BQGraphicsScene::connectChamferPushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){ createBGraphicsItem(BGraphicsItem::ItemType::Chamfer, ""); });
}

void BQGraphicsScene::connectClearPushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){ clearItem(); });
}

void BQGraphicsScene::connectSavePushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){
        if ( m_graphicsItemList.size() == 0 ) {
            GlobalFun::showMessageBox(3, "No graphics to save !");
        } else {
            QString path = GlobalFun::getCurrentTime(1);
            if ( GlobalValue::file_save_path != "null" ) { path = GlobalValue::file_save_path + "/" + path; }

            QString file = QFileDialog::getSaveFileName(nullptr, GlobalString::action_save, path, "Mask files (*.msk)");
            saveItemToConfig(file);

            GlobalFun::showMessageBox(2, "Save MSK file successfully !");
        }
    });
}

void BQGraphicsScene::connectLoadPushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){
        QString path = GlobalFun::getCurrentPath();
        if ( GlobalValue::file_load_path != "null" ) { path = GlobalValue::file_load_path; }

        QString file = QFileDialog::getOpenFileName(nullptr, GlobalString::action_load, path, "Mask files (*.msk)");
        loadItemToScene(file);
    });
}

void BQGraphicsScene::connectChangePushButton(QPushButton *btn)
{
    m_btnList.append(btn);
    connect(btn, &QPushButton::clicked, [&](){
        for ( auto &temp : m_graphicsItemList )
        {
            delete temp;
        }
        m_graphicsItemList.clear();

        if ( GlobalValue::com_tp == 1 ) {
            GlobalValue::com_tp = 2;
            createBGraphicsItem(BGraphicsItem::ItemType::Square, "");
            changeBtnState(false, true);
        } else {
            GlobalValue::com_tp = 1;
            changeBtnState(true, false);
        }

        emit changeMode();
    });
}

void BQGraphicsScene::initStatus()
{
    changeBtnState(GlobalValue::com_tp == 1, GlobalValue::com_tp != 1);
}

void BQGraphicsScene::changeBtnState(bool enable, bool exceptLast)
{
    for ( int i = 0; i < m_btnList.size(); ++i )
    {
        if ( i == m_btnList.size() - 1 && exceptLast ) {
            // pass
        } else {
            m_btnList.at(i)->setEnabled(enable);
        }
    }
}

void BQGraphicsScene::removeItemDFromList(BGraphicsItem *item)
{
    for ( int i = 0; i < m_graphicsItemList.size(); ++i )
    {
        if ( item == m_graphicsItemList.at(i) ) {
            delete m_graphicsItemList.at(i);
            m_graphicsItemList.removeAt(i);
        }
    }
}

void BQGraphicsScene::clearItem()
{
    for ( auto &temp : m_graphicsItemList )
    {
        delete temp;
    }
    m_graphicsItemList.clear();
}

//------------------------------------------------------------------------------

void BQGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (is_creating_BPolygon) {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        switch ( event->buttons() )
        {
        case Qt::LeftButton: {
            m_list.push_back(p);
            emit updatePoint(current_polygon, m_list, false);
        } break;
        case Qt::RightButton: {
            if (m_list.size() >= 3) {
                emit updatePoint(current_polygon, m_list, true);
                is_creating_BPolygon = false;
                is_ignore_contextMenu = true;
                m_list.clear();
                changeBtnState(true, false);
            }
        } break;
        default: break;
        }
    } else {
        QGraphicsScene::mousePressEvent(event);
    }
}

void BQGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    int index = m_simpleText.text().left(1).toInt();
    int size = (int)m_matList.size();

    if ( event->delta() > 0 && index < size ) {
        index++;
        QImage image = GlobalFun::convertMatToQImage(m_matList.at(index-1));
        m_unlivePixmap.setPixmap(QPixmap::fromImage(image));
        m_unlivePixmap.setPos((-1)*image.width()/2, (-1)*image.height()/2);
        m_simpleText.setText(QString("%1/%2").arg(index).arg(m_matList.size()));
    } else if ( event->delta() < 0 && index > 1 ) {
        index--;
        QImage image = GlobalFun::convertMatToQImage(m_matList.at(index-1));
        m_unlivePixmap.setPixmap(QPixmap::fromImage(image));
        m_unlivePixmap.setPos((-1)*image.width()/2, (-1)*image.height()/2);
        m_simpleText.setText(QString("%1/%2").arg(index).arg(m_matList.size()));
    }

    QGraphicsScene::wheelEvent(event);
}
