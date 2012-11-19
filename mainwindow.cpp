#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdlib.h> // Declare "system()"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    this->cam = new UVCH264Cam();

    this->baseDir = "/home/peter/vid/";
    this->cam->changeBaseDir(this->baseDir);
    this->cam->changeDevice("/dev/video1");

    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateFile()));
    timer->start(60000);

    this->ethRelay = new EthRelay();
    this->ethRelay->connect();

    this->process = new QProcess();

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateLight()
{
    QTime time = QTime::currentTime();
    if(time.hour() < 10 || time.hour() > 22){
        // night
        this->ethRelay->switchOff(1);
        this->ethRelay->switchOn(0);
    }else{
        //day
        this->ethRelay->switchOff(0);
        this->ethRelay->switchOn(1);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    this->cam->changeLocationToCurrentTime(this->baseDir);
}

void MainWindow::updateFile()
{
    QString oldLocation = this->cam->getLocation();

    timer->start(60000);
    this->cam->changeLocationToCurrentTime();
    updateLight();

    // add header
    QFile oldVideo(oldLocation);
    if (oldVideo.open(QFile::ReadOnly)) {
        QByteArray header = QByteArray::fromHex("0000000167640028AC7680780227E5C0512000000300200000078C080002022E000808B7BDF0BC2211A80000000168EE38B0000000016588");
        header += oldVideo.readAll();

        oldVideo.close();
        QFile newVideo(oldLocation + "_new.mp4");
        if(newVideo.open(QFile::WriteOnly)){
            QDataStream complete(&newVideo);
            complete << header;
            newVideo.close();
        }
    }
}

void MainWindow::on_pushButton_clicked()
{

    this->cam->start();
//    this->cam->startCamera("/dev/video1", this->baseDir);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    qDebug() << "yeay";
    if (checked){
//        process->start
        std::system("uvcdynctrl -d /dev/video1 -s \'Focus, auto\' true");
    }else{
        std::system("uvcdynctrl -d /dev/video1 -s \'Focus, auto\' false");
    }
}

void MainWindow::on_lineEdit_2_editingFinished()
{
    QString cmd ="uvcdynctrl -d /dev/video1 -s \'Focus (absolute)\' " + ui->lineEdit_2->text();
    system(cmd.toStdString().c_str());
    ui->horizontalSlider->setSliderPosition(ui->lineEdit_2->text().toInt());
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    QString cmd = "uvcdynctrl -d /dev/video1 -s \'Focus (absolute)\' " + QString::number(value);
    system(cmd.toStdString().c_str());
    ui->lineEdit_2->setText( QString::number(value));
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    QString cmd = "uvcdynctrl -d /dev/video1 -s \'Gain\' " + QString::number(value);
    system(cmd.toStdString().c_str());
    ui->lineEdit_3->setText( QString::number(value));
}

void MainWindow::on_checkBox_2_toggled(bool checked)
{
    if (checked){
        std::system("uvcdynctrl -d /dev/video1 -s \'Exposure, Auto\' true");
    }else{
        std::system("uvcdynctrl -d /dev/video1 -s \'Exposure, Auto\' false");
    }

}

void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    QString cmd = "uvcdynctrl -d /dev/video1 -s \'Exposure (Absolute)' " + QString::number(value);
    system(cmd.toStdString().c_str());
    ui->lineEdit_4->setText( QString::number(value));
}
