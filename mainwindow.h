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

private:
    Ui::MainWindow *ui;
    UVCH264Cam* cam;
    EthRelay* ethRelay;
    QTimer* timer;
    QString baseDir;
    QProcess* process;
};

#endif // MAINWINDOW_H
