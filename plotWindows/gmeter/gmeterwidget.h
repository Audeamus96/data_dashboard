#ifndef GMETERWIDGET_H
#define GMETERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QList>
#include <QPointF>

#define MAX_TAIL_LENGTH 100

class GmeterWidget : public QWidget
{
    Q_OBJECT
public:
    GmeterWidget(QWidget *parent = nullptr);
    void setPosition(qreal x, qreal y);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPointF currentPosition;
    QList<QPointF> tailPositions;

    void updateTail();

};

#endif // GMETERWIDGET_H
