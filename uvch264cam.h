#ifndef UVCH264CAM_H
#define UVCH264CAM_H
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QThread>
#include <QCoreApplication>

#include <gst/gst.h>
#include <gst/video/video.h>

//#include <gst/interfaces/xoverlay.h>
//#include <gst/video/video.h>
//#include <gst/gstdebugutils.h>

class UVCH264Cam : public QThread
{
    Q_OBJECT

public:
    UVCH264Cam(QObject *parent);
    ~UVCH264Cam();

    void run();

    int changeLocation(QString filename);

    int changeBaseDir(QString baseDir);

    int changeDevice(QString device);

    QString getLocation();



public slots:
    QString changeLocationToCurrentTime(QString baseDir);
    QString changeLocationToCurrentTime();
    int switchReview(bool toogle);
    int disconnect();

signals:
    void changedLocation(QString oldLocation);


private:
    QString updateCurrentFilename();
    void updateFilesink();

    static GstBusSyncReply gstPipelineHandler(GstBus * bus, GstMessage * msg, UVCH264Cam* cam);
    static GstBusSyncReply gstQueueHandler(GstBus * bus, GstMessage * msg, UVCH264Cam* cam);
    static void handle_block_location(GstPad* pad, gboolean blocked, gpointer user_data);

    GstElement *bin;
    GstElement *src;
    GstElement *queue_src;
    GstElement *identity;
    GstElement *file_sink0;
    GstElement *file_sink1;
    GstElement *mp4mux0;
    GstElement *mp4mux1;
    GstElement *queue_0;
    GstElement *queue_1;
    GstElement *valve_0;
    GstElement *ph264;
    GstElement *t;
    GstElement *vid_capsfilter;
    GstElement *vf_capsfilter;
    GstElement *preview_sink;
    GstElement *queue_preview;

    GstBus *bus;
    GstBus *busQueue_0;
    GstMessage *msg;

    QString basedir;
    QString device;
    QString location;
    QString oldLocation;

    unsigned int keyFrameNumber;

    bool changingLocation;
};

#endif // UVCH264CAM_H
