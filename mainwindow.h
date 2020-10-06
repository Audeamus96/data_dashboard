#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "serialAdapter/serialadapter.h"
#include "helperObjects/channel/channel.h"
#include "helperObjects/mathComponent/mathchannelcomponent.h"
#include <QLabel>
#include <vector>

#include <QTimer>
#include <QCheckBox>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public Q_SLOTS:
    void LogLine(const QString &line);
    void toggleConnection();
    void refreshUI();

public slots:
    void UpdateAvailMathCh();

private slots:
    void on_frameStartCh_editingFinished();

    void on_frameChSeparator_editingFinished();

    void on_frameEndSep_editingFinished();

    void on_channelNumber_valueChanged(int arg1);

    void on_addMathComp_clicked();

    void on_add3D_clicked();

    void on_delete_updateMathComp(uint8_t id);

private:
    void LoadSettings();
    void clearLayout(QLayout* layout, bool deleteWidgets = true);

    Ui::MainWindow *ui;
    SerialAdapter *dataAdapter;

    QSettings   *settings;
    QTimer      *mainTimer;

    //  Dynamically created/destroyed UI elements
    std::vector<Channel*>ch;
    std::vector<MathChannelComponent*>mathComp;

    std::vector<QCheckBox*>mathChEnabled;
    std::vector<QLineEdit*>mathChName;
};

#endif // MAINWINDOW_H
