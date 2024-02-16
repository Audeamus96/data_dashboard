#include "gmeterwidget.h"
#include <QtMath>

GmeterWidget::GmeterWidget(QWidget *parent) : QWidget(parent){}

void GmeterWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw the circular path
    int margin = 10;
    QRectF circleRect(margin, margin, width() - 2 * margin, height() - 2 * margin);
    painter.drawEllipse(circleRect);

    // Draw the tail
    qreal fadeStep = 1.0 / MAX_TAIL_LENGTH;
    qreal currentOpacity = 1.0;
    for (const QPointF &position : qAsConst(tailPositions)) {
        painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
        painter.setOpacity(currentOpacity);
        painter.drawEllipse(position, 3, 3); // Adjust size as needed
        currentOpacity -= fadeStep; // Fade the tail
    }

    // Reset opacity before drawing the main dot
    painter.setOpacity(1.0);

    // Draw the main dot (reuse the last position in the tail)
    if (!tailPositions.isEmpty()) {
        painter.setBrush(Qt::red);
        painter.drawEllipse(tailPositions.first(), 5, 5); // Adjust size as needed
    }
//    // Calculate the dot's position
//    qreal centerX = width() / 2.0;
//    qreal centerY = height() / 2.0;
//    qreal radius = (width() - 2 * margin) / 2.0;
//    qreal dotX = centerX + radius * qCos(angle);
//    qreal dotY = centerY + radius * qSin(angle);

//    // Draw the moving dot
//    painter.setBrush(Qt::red);
//    int dotSize = 10;
//    painter.drawEllipse(QPointF(dotX, dotY), dotSize / 2.0, dotSize / 2.0);
}

void GmeterWidget::setAngle(qreal newAngle)
{
    if (angle != newAngle) {
        angle = newAngle;
        updateTail(); // Update tail before repainting
        update(); // Trigger a repaint whenever the angle changes
    }
}

void GmeterWidget::updateTail()
{
    // Calculate the new dot position based on the current angle
    qreal centerX = width() / 2.0;
    qreal centerY = height() / 2.0;
    qreal radius = (width() - 20) / 2.0; // Adjust for margin
    qreal dotX = centerX + radius * qCos(angle);
    qreal dotY = centerY + radius * qSin(angle);
    QPointF newDotPosition(dotX, dotY);

    // Add the new position to the list of tail positions
    tailPositions.prepend(newDotPosition);

    // Ensure the list doesn't grow indefinitely
    if (tailPositions.size() > MAX_TAIL_LENGTH) {
        tailPositions.removeLast();
    }
}
