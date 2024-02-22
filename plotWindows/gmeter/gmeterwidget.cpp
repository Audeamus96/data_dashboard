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
        painter.drawEllipse(currentPosition, 5, 5); // Adjust size as needed
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
    if (tailPositions.size() > MAX_TAIL_LENGTH) {
        tailPositions.removeLast();
    }
}
