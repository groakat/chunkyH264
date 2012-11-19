#include "mainwindow.h"
#include "ui_mainwindow.h"

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
