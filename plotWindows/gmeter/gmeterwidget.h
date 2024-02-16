#ifndef GMETERWIDGET_H
#define GMETERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QList>
#include <QPointF>

#define MAX_TAIL_LENGTH 25

class GmeterWidget : public QWidget
{
    Q_OBJECT
public:
    GmeterWidget(QWidget *parent = nullptr);
    void setAngle(qreal newAngle);
    qreal getAngle() const{ return angle;}

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal angle = 0;
    QList<QPointF> tailPositions;

    void updateTail();

};

#endif // GMETERWIDGET_H
