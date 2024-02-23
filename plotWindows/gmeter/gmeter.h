#ifndef GMETER_H
#define GMETER_H
#include <QVBoxLayout>
#include <QMdiSubWindow>

#include <helperObjects/graphHeaderWidget/graphheaderwidget.h>
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

public slots:
    void UpdateTailSize(const QString &_size);

private:
    void _ConstructUI();

    uint8_t _nInputs;
    QVector<uint8_t>_inputChannels;
    uint8_t _maxInChannel;
    uint32_t _tailSize;

    QVBoxLayout *windMainLayout;
    QWidget *_contWind;
    graphHeaderWidget *_header;
    GmeterWidget *_gmeterWidget;

    QTimer *_refresher;

};

#endif // GMETER_H
