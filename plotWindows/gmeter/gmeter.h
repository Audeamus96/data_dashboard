#ifndef GMETER_H
#define GMETER_H
#include <QVBoxLayout>
#include <QMdiSubWindow>

#include <QPainter>

#include <helperObjects/graphHeaderWidget/graphheaderwidget.h>
#include "plotWindows/line/qcustomplot.h"
#include "gmeterwidget.h"

class Gmeter : public QMdiSubWindow
{
    Q_OBJECT
public:
    Gmeter(QString objName="");
    ~Gmeter();

    void UpdateInputChannels(uint8_t *inChannels);
    void ReceiveData(double *data, uint8_t n);

signals:
    void logLine(const QString &line);

private:
    void _ConstructUI();

    GmeterWidget *_gmeterDrawingWidget;

    QWidget *_contWind;
    QVBoxLayout *windMainLayout;

    QTimer *_refresher;

};

#endif // GMETER_H
