#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ethrelay.h>
#include <uvch264cam.h>
#include <QDebug>
#include <QTimer>
#include <QDateTime>

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

private:
    Ui::MainWindow *ui;
    UVCH264Cam* cam;
    EthRelay* ethRelay;
    QTimer* timer;
    QString baseDir;
};

#endif // MAINWINDOW_H
