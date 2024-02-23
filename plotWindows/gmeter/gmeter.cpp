#include "gmeter.h"
//#include "helperObjects/dataMultiplexer/datamultiplexer.h"
#include <mainwindow.h>

// todo: limit of 3.5g's

Gmeter::Gmeter(QString objName)
{
    _gmeterDrawingWidget = new GmeterWidget(this);
    _gmeterDrawingWidget->setMinimumSize(QSize(200,200));
    this->setObjectName(objName);
    windMainLayout = new QVBoxLayout(_gmeterDrawingWidget);
    this->setWidget(_gmeterDrawingWidget);

    _refresher = new QTimer(this);

    static qreal x = 0;
    static qreal y = 0;

    connect(_refresher, &QTimer::timeout, _gmeterDrawingWidget,[this]() {
        // Update the angle for the moving dot
        // This is a simple example; you might want to base this on actual data
        _gmeterDrawingWidget->setPosition(x, y); // Increment the angle and update the widget
        x+=0.1;
        y+=0.1;
    });

    _refresher->start(500);

}

Gmeter::~Gmeter(){
    emit logLine("Gmeter: Destroying the plot");

}

void Gmeter::_ConstructUI(){

}
