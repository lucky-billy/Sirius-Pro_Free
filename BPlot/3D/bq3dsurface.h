#ifndef BQ3DSURFACE_H
#define BQ3DSURFACE_H

#include <QObject>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
using namespace QtDataVisualization;

class BQMouseEvent : public QMouseEvent
{
public:
    BQMouseEvent(Type type, const QPointF &localPos, Qt::MouseButton button,
                 Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);

    void changeButtonType(int type);
};

class BQ3DSurface : public Q3DSurface
{
    Q_OBJECT

public:
    BQ3DSurface(const QSurfaceFormat *format = nullptr, QWindow *parent = nullptr);

signals:
    void mousePressed();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
};

#endif // BQ3DSURFACE_H
