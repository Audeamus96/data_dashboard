#ifndef GMETERWIDGET_H
#define GMETERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QList>
#include <QPointF>

#define MAX_TAIL_LENGTH 10000

class GmeterWidget : public QWidget
{
    Q_OBJECT
public:
    GmeterWidget(QWidget *parent = nullptr);
    void setPosition(qreal x, qreal y);
    void setTailSize(uint32_t size);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPointF currentPosition;
    QList<QPointF> tailPositions;
    uint32_t _tailSize;

    void updateTail();
    QPointF logicalToPhysical(qreal logicalX, qreal logicalY);
    void drawGraphBackground(QPainter *painter);

    // Constants for logical range of the graph
    const qreal logicalMin = -3.5;
    const qreal logicalMax = 3.5;

};

#endif // GMETERWIDGET_H
