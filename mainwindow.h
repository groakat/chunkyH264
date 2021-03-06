#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ethrelay.h>
#include <uvch264cam.h>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void updateLight();

    void newCamera();

    void restartCamera();


private slots:
    void on_pushButton_2_clicked();
    void updateFile();

    void on_pushButton_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_checkBox_toggled(bool checked);

    void on_lineEdit_2_editingFinished();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_checkBox_2_toggled(bool checked);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_lineEdit_6_editingFinished();

    void on_pushButton_5_clicked();

    void on_checkBox_3_toggled(bool checked);

    void on_pushButton_6_clicked();

    void on_pushButton_3_clicked(bool checked);

    void on_pushButton_4_clicked(bool checked);

    void on_checkBox_4_toggled(bool checked);

public slots:
    void deleteCam();

    void setDay();
    void setNight();

    void switchLightOn();
    void switchLightOff();



    void setBaseDir(QString path);

    void setDevice(QString device);

    void setIsMaster(bool isMaster);

    void setRelays(int relayDay, int relayNight);

    void setRelayDay(int relayDay);

    void setRelayNight(int relayNight);

    void setDirectStart(bool directStart);

    void sendSystemCmd(QString cmd);

    void checkProgress(QString oldLocation);

private:
    Ui::MainWindow *ui;
    UVCH264Cam* cam;
    EthRelay* ethRelay;
    QTimer* timer;
    QProcess* process;
    bool isCameraRestart;
    QString location;
    int msecToUpdate;

    // basic input parameters
    QString baseDir;
    QString device;
    bool isMaster;
    int relayDay;
    int relayNight;
    bool directStart;
};

#endif // MAINWINDOW_H
