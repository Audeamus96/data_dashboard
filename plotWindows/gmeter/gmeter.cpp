#include "gmeter.h"
//#include "helperObjects/dataMultiplexer/datamultiplexer.h"
#include <mainwindow.h>


Gmeter::Gmeter(QString objName)
{
    _gmeterDrawingWidget = new GmeterWidget(this);
    _gmeterDrawingWidget->setMinimumSize(QSize(200,200));
    this->setObjectName(objName);
   windMainLayout = new QVBoxLayout(_gmeterDrawingWidget);
    this->setWidget(_gmeterDrawingWidget);

    _refresher = new QTimer(this);
    connect(_refresher, &QTimer::timeout, _gmeterDrawingWidget,[this]() {
        // Update the angle for the moving dot
        // This is a simple example; you might want to base this on actual data
        qreal currentAngle = _gmeterDrawingWidget->getAngle();
        _gmeterDrawingWidget->setAngle(currentAngle + 0.01); // Increment the angle and update the widget
    });

    _refresher->start(5);

//    _contWind = new QWidget();
//    this->setObjectName(objName);
//    windMainLayout = new QVBoxLayout(_contWind);
//    this->setWidget(_contWind);

//    _refresher = new QTimer(this);

//    _ConstructUI();

}

Gmeter::~Gmeter(){
    emit logLine("Gmeter: Destroying the plot");
}

void Gmeter::_ConstructUI(){

    // generate some data:
//    QVector<double> x(101), y(101); // initialize with entries 0..100
//    for (int i=0; i<101; ++i)
//    {
//        x[i] = i/50.0 - 1; // x goes from -1 to 1
//        y[i] = x[i]*x[i]; // let's plot a quadratic function
//    }

//    _plot = new QCustomPlot();
//    _plot->addGraph();
//    _plot->graph(0)->setData(x,y);
//    _plot->xAxis->setLabel("x");
//    _plot->xAxis->setLabel("y");
//    _plot->xAxis->setRange(-1,1);
//    _plot->yAxis->setRange(0,1);

//    QCPItemTracer *tracer = new QCPItemTracer(_plot);


//    _plot->replot();
//    windMainLayout->addWidget(_plot);

}
