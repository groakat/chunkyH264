#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdlib.h> // Declare "system()"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    this->baseDir = "/mnt/raid/box0/";

    this->cam = NULL;


    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateFile()));

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
    switchLightOn();
    QTime time = QTime::currentTime();
    if(time.hour() < 10 || time.hour() > 44){
        // night
        setNight();
    }else{
        //day
        setDay();
    }
}

void MainWindow::newCamera()
{
    if(!this->cam){
        this->cam = new UVCH264Cam();
        this->cam->changeBaseDir(this->ui->lineEdit->text());
        this->cam->changeDevice(this->ui->lineEdit_6->text());
        this->cam->start();
        timer->start(60000);
        connect(this->cam, SIGNAL(finished()), this->cam, SLOT(deleteLater()));
        qDebug() << "new camera";
    }
}

void MainWindow::restartCamera()
{
    this->cam->disconnect();
    this->isCameraRestart = true;
//    this->cam = NULL;
    deleteCam();
}

void MainWindow::on_pushButton_2_clicked()
{
    this->cam->changeLocationToCurrentTime(this->baseDir);
}

void MainWindow::updateFile()
{
    QString oldLocation = this->cam->getLocation();

    timer->start(60000);

    if(QTime::currentTime().minute() == 47){
        restartCamera();
    }else{
        this->cam->changeLocationToCurrentTime();
    }
    updateLight();

//    // add header
//    QFile oldVideo(oldLocation);
//    if (oldVideo.open(QFile::ReadOnly)) {
//        QByteArray header = QByteArray::fromHex("0000000167640028AC7680780227E5C0512000000300200000078C080002022E000808B7BDF0BC2211A80000000168EE38B0000000016588");
//        header += oldVideo.readAll();

//        oldVideo.close();
//        QFile newVideo(oldLocation + "_new.mp4");
//        if(newVideo.open(QFile::WriteOnly)){
//            QDataStream complete(&newVideo);
//            complete << header;
//            newVideo.close();
//        }
//    }
}

void MainWindow::on_pushButton_clicked()
{
//    this->cam->startCamera("/dev/video1", this->baseDir);
//    updateLight();
    newCamera();
    updateLight();
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    qDebug() << "yeay";
    if (checked){
//        process->start
        QString cmd = "uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Focus, auto\' true";
        std::system(cmd.toStdString().c_str());
    }else{
        QString cmd = "uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Focus, auto\' false";
        std::system(cmd.toStdString().c_str());
    }
}

void MainWindow::on_lineEdit_2_editingFinished()
{
    QString cmd ="uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Focus (absolute)\' " + ui->lineEdit_2->text();
    system(cmd.toStdString().c_str());
    ui->horizontalSlider->setSliderPosition(ui->lineEdit_2->text().toInt());
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    QString cmd = "uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Focus (absolute)\' " + QString::number(value);
    system(cmd.toStdString().c_str());
    ui->lineEdit_2->setText( QString::number(value));
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    QString cmd = "uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Gain\' " + QString::number(value);
    system(cmd.toStdString().c_str());
    ui->lineEdit_3->setText( QString::number(value));
}

void MainWindow::on_checkBox_2_toggled(bool checked)
{
    if (checked){
        QString cmd = "uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Exposure, Auto\' true";
        std::system(cmd.toStdString().c_str());
    }else{
        QString cmd = "uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Exposure, Auto\' false";
        std::system(cmd.toStdString().c_str());
    }

}

void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    QString cmd = "uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Exposure (Absolute)' " + QString::number(value);
    system(cmd.toStdString().c_str());
    ui->lineEdit_4->setText( QString::number(value));
}

void MainWindow::on_lineEdit_6_editingFinished()
{
    if(this->cam != NULL){
        this->cam->changeDevice(this->ui->lineEdit_6->text());
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    this->cam->disconnect();
//    this->cam->wait();
//    if(this->cam->isFinished()){
//        delete &this->cam;
    this->cam = NULL;
//    }
}

void MainWindow::on_checkBox_3_toggled(bool checked)
{

}

void MainWindow::on_pushButton_6_clicked()
{
    restartCamera();
}

void MainWindow::on_pushButton_3_clicked(bool checked)
{
    setDay();
}

void MainWindow::on_pushButton_4_clicked(bool checked)
{
    setNight();
}

void MainWindow::deleteCam()
{
//    delete this->cam;
    qDebug() << "deleteCam()";
    this->cam = NULL;
    if (this->isCameraRestart){
        this->isCameraRestart = false;
        newCamera();
    }
}

void MainWindow::setDay()
{
    qDebug() << "setDay()";;
    this->ethRelay->switchOff(0);
    this->ethRelay->switchOn(1);
    if(this->cam != NULL){
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Saturation\' 0");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Focus, auto\' false");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Exposure, Auto Priority\' false");
//        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Exposure, Auto\' false");

        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Focus (absolute)\' 100 ");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Exposure (Absolute)\' 98");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Gain\' 0");
//        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Saturation\' 0 " + \
//                      "\'Focus, auto\' false " + "\'Exposure, Auto\' false " + \
//                      "\'Focus (absolute)\' 100 " + "\'Exposure (Absolute)\' 98 " + \
//                      "\'Gain\' 0");
    }
}

void MainWindow::setNight()
{
    qDebug() << "setNight";
    this->ethRelay->switchOn(0);
    this->ethRelay->switchOn(1);
    if (this->cam != NULL){
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Saturation\' 0");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Focus, auto\' false");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Exposure, Auto\' false");

        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Focus (absolute)\' 135");// + ui->lineEdit_2->text();
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Exposure (Absolute)\' 98"); // + QString::number(value);
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Gain\' 0");
    }
}

void MainWindow::switchLightOn()
{
    this->ui->checkBox_4->setCheckState(Qt::Checked);
}

void MainWindow::switchLightOff()
{
    this->ui->checkBox_4->setCheckState(Qt::Unchecked);
}

void MainWindow::sendSystemCmd(QString cmd)
{
    std::system(cmd.toStdString().c_str());
}

void MainWindow::on_checkBox_4_toggled(bool checked)
{
    if (checked){
        this->ethRelay->switchOn(1);
    }else{
        this->ethRelay->switchOff(1);
    }
}
