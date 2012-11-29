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
//#include <gst/video/video.h>
//#include <gstcollectpads2.h>

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

public:
    /* video force key unit event creation and parsing */

    GstEvent * gst_video_event_new_downstream_force_key_unit (GstClockTime timestamp,
                                                              GstClockTime streamtime,
                                                              GstClockTime runningtime,
                                                              gboolean all_headers,
                                                              guint count);

    gboolean gst_video_event_parse_downstream_force_key_unit (GstEvent * event,
                                                              GstClockTime * timestamp,
                                                              GstClockTime * streamtime,
                                                              GstClockTime * runningtime,
                                                              gboolean * all_headers,
                                                              guint * count);

    GstEvent * gst_video_event_new_upstream_force_key_unit (GstClockTime running_time,
                                                            gboolean all_headers,
                                                            guint count);

    gboolean gst_video_event_parse_upstream_force_key_unit (GstEvent * event,
                                                            GstClockTime * running_time,
                                                            gboolean * all_headers,
                                                            guint * count);

    gboolean gst_video_event_is_force_key_unit(GstEvent *event);



public slots:
    QString changeLocationToCurrentTime(QString baseDir);
    QString changeLocationToCurrentTime();
    int switchReview(bool toogle);
    int disconnect();

signals:
    void changedLocation(QString oldLocation);


private:
    QString updateCurrentFilename();
    void updateFilesink(GstElement *filesink, GstElement *mp4mux);

    static GstBusSyncReply gstPipelineHandler(GstBus * bus, GstMessage * msg, UVCH264Cam* cam);
    static bool gstQueueHandler(GstBus * bus, GstMessage * msg, UVCH264Cam* cam);
    static void handle_block_location(GstPad* pad, gboolean blocked, gpointer user_data);

    GstElement *mainPipeline;
    GstElement *binRec1;
    GstElement *binRec2;
    GstElement *binRec3;
    GstElement *catchPipeline;
    GstElement *src;
    GstElement *queue_src;
    GstElement *identity1;
    GstElement *identity2;
    GstElement *identity3;
    GstElement *file_sink1;
    GstElement *file_sink2;
    GstElement *file_sink3;
    GstElement *mp4mux1;
    GstElement *mp4mux2;
    GstElement *mp4mux3;
    GstElement *queue_0;
    GstElement *queue_1;
    GstElement *queue_2;
    GstElement *queue_3;
    GstElement *queue_catch;
    GstElement *valve_0;
    GstElement *ph264_1;
    GstElement *ph264_2;
    GstElement *ph264_3;
    GstElement *os;
    GstElement *t;
    GstElement *vid_capsfilter;
    GstElement *vf_capsfilter;
    GstElement *preview_sink;
    GstElement *queue_preview;
    GstElement *fakesrc;
    GstElement *fakesink;

    GstCaps *h264Caps;

    GstPad* vidsrcPad;
    GstPad* vfsrcPad;

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
