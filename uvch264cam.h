#ifndef UVCH264CAM_H
#define UVCH264CAM_H
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QThread>

#include <gst/gst.h>
#include <gst/video/video.h>

//#include <gst/interfaces/xoverlay.h>
//#include <gst/video/video.h>
//#include <gst/gstdebugutils.h>

class UVCH264Cam : public QThread
{
public:
    UVCH264Cam();
    ~UVCH264Cam();

    void run();

    int changeLocation(QString filename);

    int changeBaseDir(QString baseDir);

    int changeDevice(QString device);

    QString getLocation();



public slots:
    int changeLocationToCurrentTime(QString baseDir);
    int changeLocationToCurrentTime();
    int switchReview(bool toogle);
    int disconnect();


private:
    QString updateCurrentFilename();

    static GstBusSyncReply gstPipelineHandler(GstBus * bus, GstMessage * msg, GstPipeline * bin);

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
    GstElement *t;
    GstElement *vid_capsfilter;
    GstElement *vf_capsfilter;
    GstElement *preview_sink;
    GstElement *queue_preview;

    GstBus *bus;
    GstMessage *msg;

    QString basedir;
    QString device;
    QString location;

    unsigned int keyFrameNumber;
};

#endif // UVCH264CAM_H
