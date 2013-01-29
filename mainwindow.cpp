#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdlib.h> // Declare "system()"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    this->cam = NULL;
    this->msecToUpdate = 60000; //10min for mem testing


    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateFile()));

    this->ethRelay = new EthRelay();
    this->ethRelay->connect();

    this->process = new QProcess();


    ui->setupUi(this);

    this->baseDir = ui->lineEdit->text(); //"/mnt/raid/box0/";
    this->device = ui->lineEdit_6->text();

    this->relayNight = 0;
    this->relayDay = 1;

    this->isMaster = true;

}

MainWindow::~MainWindow()
{
    delete ui;
    this->cam->disconnect();
    delete this->process;
}

void MainWindow::updateLight()
{
    switchLightOn();
    QTime time = QTime::currentTime();
    if(time.hour() < 10 || time.hour() > 22){
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
        this->cam = new UVCH264Cam(this);
        this->cam->changeBaseDir(this->baseDir);
        this->cam->changeDevice(this->device);
        this->cam->start();
        QDateTime dt = QDateTime::currentDateTime();
        timer->start(this->msecToUpdate - (dt.time().second()*1000 + dt.time().msec()));
        connect(this->cam, SIGNAL(finished()), this->cam, SLOT(deleteLater()));
        connect(this->cam, SIGNAL(finished()), this, SLOT(deleteCam()));
        connect(this->cam, SIGNAL(changedLocation(QString)), this, SLOT(checkProgress(QString)));
        this->location = this->cam->getLocation();
//        connect(this->cam, SIGNAL(terminated()), this, SLOT(deleteCam()));
        qDebug() << "MainWindow::newCamera()" << QDateTime::currentMSecsSinceEpoch();
    }
}

void MainWindow::restartCamera()
{
    qDebug() << "MainWindow::restartCamera() " << QDateTime::currentMSecsSinceEpoch();
    this->isCameraRestart = true;
    this->cam->disconnect();
//    this->cam = NULL;
//    deleteCam();
}

void MainWindow::on_pushButton_2_clicked()
{
    this->location = this->cam->changeLocationToCurrentTime(this->baseDir);
}

void MainWindow::updateFile()
{
    timer->start(this->msecToUpdate);

//    if(QTime::currentTime().minute() == 0){
//        QFile log(this->baseDir + "/log.txt");
//        if(log.open(QFile::ReadWrite| QFile::Text | QFile::Append)){
//            QTextStream ts(&log);
//            ts << QDateTime::currentDateTime().toString(Qt::ISODate) << " restrart camera. reason: scheduled\n";
//            qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "restrart camera. reason: scheduled";
//            log.close();
//        }else{
//            qDebug() << log.errorString();
//        }
//        restartCamera();
//    }else{
        this->location = this->cam->changeLocationToCurrentTime();
//    }
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

    QFile log(this->baseDir + "/log.txt");
    if(log.open(QFile::ReadWrite| QFile::Text | QFile::Append)){
        QTextStream ts(&log);
        //      -------------- program start 1997-07-16T19:20:30+01:00 --------------
        ts << " -------------- program start " << QDateTime::currentDateTime().toString(Qt::ISODate) << " -------------- \n";
        ts << " --------------------------------------------------------------------- \n";
        log.close();
    }else{
        qDebug() << log.errorString();
    }

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
    this->isCameraRestart = false;
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
    this->ethRelay->switchOff(this->relayNight);
    this->ethRelay->switchOn(this->relayDay);
    if(this->cam != NULL){
//        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Saturation\' 0");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Focus, auto\' false");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Exposure, Auto Priority\' false");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'White Balance Temperature, Auto\' false");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'White Balance Temperature\' 6500");
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
    this->ethRelay->switchOff(relayDay);
    this->ethRelay->switchOn(relayNight);
    if (this->cam != NULL){
//        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Saturation\' 0");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Focus, auto\' false");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Exposure, Auto\' false");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'White Balance Temperature, Auto\' false");
        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'White Balance Temperature\' 6500");

        sendSystemCmd("uvcdynctrl -d " + this->ui->lineEdit_6->text() + " -s \'Focus (absolute)\' 130");// + ui->lineEdit_2->text();
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

void MainWindow::checkProgress(QString oldLocation)
{
    qDebug() << "MainWindow::checkProgress";
    QFileInfo oldFile(oldLocation);        //(this->location);
    if (oldFile.size() == 0){
        QFile log(this->baseDir + "/log.txt");
        if(log.open(QFile::ReadWrite | QFile::Text | QFile::Append)){
            QTextStream ts(&log);
            ts << QDateTime::currentDateTime().toString(Qt::ISODate) << " restrart camera. reason: faulty pipeline\n";
            qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "restrart camera. reason: faulty pipeline";
            log.close();
        }else{
            qDebug() << log.errorString();
        }
        restartCamera();
    }
}

void MainWindow::on_checkBox_4_toggled(bool checked)
{
    if (checked){
        this->ethRelay->switchOn(1);
    }else{
        this->ethRelay->switchOff(1);
    }
}

void MainWindow::setBaseDir(QString path)
{
    this->baseDir = path;
    qDebug() << "MainWindow::setBaseDir " << path;
}

void MainWindow::setDevice(QString device)
{
    this->device = device;
    qDebug() << "MainWindow::setDevice " << device;
}

void MainWindow::setIsMaster(bool isMaster)
{
    this->isMaster = isMaster;
    qDebug() << "MainWindow::setIsMaster " << isMaster;
}

void MainWindow::setRelays(int relayDay, int relayNight)
{
    this->relayDay = relayDay;
    this->relayNight = relayNight;
}

void MainWindow::setRelayDay(int relayDay)
{
    this->relayDay = relayDay;
    qDebug() << "MainWindow::setRelayDay " << relayDay;
}

void MainWindow::setRelayNight(int relayNight)
{
    this->relayNight = relayNight;
    qDebug() << "MainWindow::setRelayNight " << relayNight;
}

void MainWindow::setDirectStart(bool directStart)
{
    this->directStart = directStart;
    qDebug() << "MainWindow::setDirectStart " << directStart;
    if(directStart){
        this->on_pushButton_clicked();
    }
}
