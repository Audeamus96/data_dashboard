#include "gmeterwidget.h"
#include <QtMath>

const uint32_t defaultTailSize = 25;

GmeterWidget::GmeterWidget(QWidget *parent) : QWidget(parent), _tailSize(defaultTailSize){}

void GmeterWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // draw background
    painter.save();
    drawGraphBackground(&painter);
    painter.restore();

    // Draw the tail
    qreal fadeStep = 1.0 / MAX_TAIL_LENGTH;
    qreal currentOpacity = 1.0;
    for (const QPointF &position : qAsConst(tailPositions)) {
        painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
        painter.setOpacity(currentOpacity);
        //painter.drawEllipse(position, 3, 3); // Adjust size as needed
        qreal dotDiameter = 3;
        QPointF center = logicalToPhysical(position.x(), position.y());
        // If using the QRectF-based drawEllipse
        painter.drawEllipse(center - QPointF(dotDiameter / 2, dotDiameter / 2), dotDiameter, dotDiameter);
        currentOpacity -= fadeStep; // Fade the tail
    }

    // Reset opacity before drawing the main dot
    painter.setOpacity(1.0);

    // Draw the main dot (reuse the last position in the tail)
    if (!tailPositions.isEmpty()) {
        painter.setBrush(Qt::red);
//        painter.drawEllipse(currentPosition, 5, 5); // Adjust size as needed
        qreal dotDiameter = 5;
        QPointF center = logicalToPhysical(currentPosition.x(), currentPosition.y());
        // If using the QRectF-based drawEllipse
        painter.drawEllipse(center - QPointF(dotDiameter / 2, dotDiameter / 2), dotDiameter, dotDiameter);
    }
}

void GmeterWidget::setPosition(qreal x, qreal y)
{
    currentPosition = QPointF(x,y);
    updateTail(); // Update the tail positions before repainting
    update(); // Trigger repaint
}

void GmeterWidget::updateTail()
{
    tailPositions.prepend(currentPosition);

    // Ensure the list doesn't grow indefinitely
    if (tailPositions.size() > _tailSize) {
        tailPositions.removeLast();
    }
}

void GmeterWidget::setTailSize(uint32_t tailSize){
    _tailSize = tailSize > MAX_TAIL_LENGTH ? MAX_TAIL_LENGTH : tailSize;

    if(_tailSize < tailPositions.size()){
        for(int i=0; i > (tailPositions.size() - _tailSize); i++){
            tailPositions.removeLast();
        }
    }
}

QPointF GmeterWidget::logicalToPhysical(qreal logicalX, qreal logicalY){
    qreal physicalWidth = width();
    qreal physicalHeight = height();

    qreal scaleX = physicalWidth / (logicalMax - logicalMin);
    qreal scaleY = physicalHeight / (logicalMax - logicalMin);

    qreal physicalX = qRound((logicalX - logicalMin) * scaleX);
    qreal physicalY = qRound(physicalHeight - (logicalY - logicalMin) * scaleY); // Invert Y axis

    return QPointF(physicalX, physicalY);
}

void GmeterWidget::drawGraphBackground(QPainter *painter){
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setPen(Qt::lightGray);

    // Draw horizontal and vertical grid lines based on the logical range
    for (qreal logicalX = logicalMin; logicalX <= logicalMax; logicalX += 0.5) { // Adjust step as needed
        QPointF start = logicalToPhysical(logicalX, logicalMin);
        QPointF end = logicalToPhysical(logicalX, logicalMax);
        start.setX(start.x() - 0.5);
        end.setY(end.y()-0.5);
        if(logicalX == 0){
            painter->setPen(Qt::black);
            painter->drawLine(start, end);
            painter->setPen(Qt::lightGray);
        }else{
            painter->drawLine(start, end);
        }
    }

    for (qreal logicalY = logicalMin; logicalY <= logicalMax; logicalY += 0.5) {
        QPointF start = logicalToPhysical(logicalMin, logicalY);
        QPointF end = logicalToPhysical(logicalMax, logicalY);
        start.setX(start.x() - 0.5);
        end.setY(end.y()-0.5);
        if(logicalY == 0){
            painter->setPen(Qt::black);
            painter->drawLine(start, end);
            painter->setPen(Qt::lightGray);
        }else{
            painter->drawLine(start, end);
        }
    }

    // Optionally, draw labels for the grid lines
    painter->setPen(Qt::black);
    for (qreal logicalX = logicalMin+.5; logicalX <= logicalMax; logicalX += 1.0) { // Adjust label interval as needed
        QPointF physicalPos = logicalToPhysical(logicalX, logicalMin);
        painter->drawText(physicalPos.x(), height() - 5, QString::number(logicalX));
    }
    for (qreal logicalY = logicalMin+.5; logicalY <= logicalMax; logicalY += 1.0) {
        QPointF physicalPos = logicalToPhysical(logicalMin, logicalY);
        painter->drawText(5, physicalPos.y(), QString::number(logicalY));
    }
}
