#include "uvch264cam.h"
#include <iostream>
#include <gst/video/video.h>

UVCH264Cam::UVCH264Cam(QObject *parent = 0) : QThread(parent)
{
//    Pipeline pipeline = Pipeline();
    this->bin = NULL;
   this->keyFrameNumber = 0;
    this->location = "";
    this->basedir = "";
    msg == NULL;
}

UVCH264Cam::~UVCH264Cam()
{
    if (this->bin){
        /* stop capturing the data */
        gst_element_set_state (this->bin, GST_STATE_NULL);

        /* Free resources */
//        if (msg != NULL)
//             gst_message_unref (msg);

        gst_object_unref (bus);

        gst_object_unref (this->bin);

        gst_object_unref (this->src);
        gst_object_unref (this->file_sink0);
        gst_object_unref (this->queue_0);

        qDebug() << "disconnect!!!!";

        this->terminate();
    }
}




void UVCH264Cam::run()
{
    QString filename = updateCurrentFilename();


    gst_init(0,NULL);

    const QString launcher = "uvch264_src device=" + this->device + " name=src auto-start=true initial-bitrate=6000000 rate-control=cbr src.vfsrc ! queue ! video/x-raw-yuv,width=320,height=240,framerate=30/1 ! xvimagesink src.vidsrc ! video/x-h264,width=1920,height=1080,framerate=30/1 ! queue name=queue_0 ! h264parse ! mp4mux !  filesink name=file_sink0 location=" + filename;
    this->bin = gst_parse_launch (launcher.toLatin1(), NULL);
//    this->bin = gst_parse_launch("uvch264_src device=" + device + "name=src auto-start=true initial-bitrate=6000000 rate-control=cbr src.vfsrc ! queue ! video/x-raw-yuv,width=320,height=240,framerate=30/1 ! xvimagesink src.vidsrc ! queue ! video/x-h264,width=1920,height=1080,framerate=30/1 ! filesink name=file_sink0 location=" + filename;)


    this->file_sink0 = gst_bin_get_by_name (GST_BIN (this->bin), "file_sink0");
    this->src = gst_bin_get_by_name (GST_BIN (this->bin), "src");
    this->queue_0 = gst_bin_get_by_name (GST_BIN (this->bin), "queue_0");

    //    g_object_set(this->queue_0, "max-size-bytes", 4294967295, NULL);
        g_object_set(this->file_sink0, /*"async", false,*/ NULL);

    int ret = gst_element_set_state (this->bin, GST_STATE_PLAYING);

    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (this->bin);
        gst_object_unref (this->src);
//        gst_object_unref (this->identity);
        gst_object_unref (this->file_sink0);
        gst_object_unref (this->queue_0);
//        gst_object_unref (this->file_sink1);
//        gst_object_unref (this->t);
//        gst_object_unref (this->vid_capsfilter);
//        gst_object_unref (this->vf_capsfilter);
//        return -1;
      }

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(this->bin), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

    msg = NULL;
    /* Wait until error or EOS */
    bus = gst_element_get_bus (this->bin);
//    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    gst_bus_set_sync_handler (bus, (GstBusSyncHandler) gstPipelineHandler, this->bin);


    qDebug() << "UVCH264Cam::run() finished "<< QDateTime::currentMSecsSinceEpoch();
    exec();

//    /* Free resources */
//    if (msg != NULL)
//      gst_message_unref (msg);
//    gst_object_unref (bus);

//    /* stop capturing the data */
//    gst_element_set_state (this->bin, GST_STATE_NULL);

//    gst_object_unref (this->bin);
////    return 0;
}

int UVCH264Cam::changeLocation(QString filename)
{
    // write standard header of h264 files
//    QFile newVid(filename);

//    if (newVid.open(QFile::WriteOnly)) {
//        QDataStream txtStream(&newVid);
//        QByteArray header = QByteArray::fromHex("0000000167640028AC7680780227E5C0512000000300200000078C080002022E000808B7BDF0BC2211A80000000168EE38B0000000016588");

//        txtStream << header;
//        newVid.flush();
//        newVid.close();
//    }
    int ret = gst_element_set_state (this->file_sink0, GST_STATE_NULL);
    g_object_set(this->file_sink0, "location", filename.data(), /*"append", true,*/ NULL);
    ret = gst_element_set_state (this->file_sink0, GST_STATE_PLAYING);

    return ret;
}

int UVCH264Cam::changeBaseDir(QString baseDir)
{
    this->basedir = baseDir;
}

int UVCH264Cam::changeDevice(QString device)
{
    if (this->bin){
        /* stop capturing the data */
        gst_element_set_state (this->bin, GST_STATE_NULL);

        /* Free resources */
//        if (msg != NULL)
//         gst_message_unref (msg);


        gst_object_unref (bus);

        gst_object_unref (this->bin);

        this->exit();

        this->device = device;

        this->start();
    }else{
        this->device = device;
    }
}

QString UVCH264Cam::getLocation()
{
    return this->location;
}

int UVCH264Cam::switchReview(bool toogle)
{
    if(this->bin){
        if (toogle){
            gst_element_unlink(this->queue_0, this->file_sink0);
        }else{
            gst_element_link(this->queue_0, this->file_sink0);
        }
    }
}

int UVCH264Cam::disconnect()
{
//    if (this->bin){
//        /* stop capturing the data */
//        gst_element_set_state (this->bin, GST_STATE_NULL);

//        /* Free resources */
////        if (msg != NULL)
////             gst_message_unref (msg);

//        gst_object_unref (bus);

//        gst_object_unref (this->bin);

//        gst_object_unref (this->src);
//        gst_object_unref (this->file_sink0);
//        gst_object_unref (this->queue_0);

//        qDebug() << "disconnect!!!!" << this->bin;

//    }
    this->quit();
}

QString UVCH264Cam::changeLocationToCurrentTime(QString baseDir)
{
    this->changeBaseDir(baseDir);

    return changeLocationToCurrentTime();
}

QString UVCH264Cam::changeLocationToCurrentTime()
{
    QString oldLocation = this->location;
    QString location = updateCurrentFilename();
    qDebug() << location;

    // write standard header of h264 files
    QFile newVid(location);

//    if (newVid.open(QFile::WriteOnly)) {
//        QDataStream txtStream(&newVid);
//        QByteArray header = QByteArray::fromHex("0000000167640028AC7680780227E5C0512000000300200000078C080002022E000808B7BDF0BC2211A80000000168EE38B0000000016588");

//        txtStream << header;
//        newVid.flush();
//        newVid.close();
//    }

    int ret = 0;

    if (this->bin != NULL){
        GstClockTime gct = 10;
        GstEvent* event =  gst_video_event_new_upstream_force_key_unit(gct, true, this->keyFrameNumber++);
        qDebug() << this->keyFrameNumber;
        gst_element_send_event (this->bin, event);

        ret = gst_element_set_state (this->file_sink0, GST_STATE_NULL);
//        QChar* path = location.data();
        std::string path = location.toStdString();
        qDebug() << "location.data() " << path.c_str() ;
        g_object_set(this->file_sink0, "location", path.c_str(), /*"append", true, */NULL);
        gint bufferStatus = 100;
        ret = gst_element_set_state (this->file_sink0, GST_STATE_PLAYING);
        g_object_get(this->queue_0, "current-level-bytes", &bufferStatus, NULL);
        qDebug() << bufferStatus <<  (bufferStatus / 4294967295.0f);
    }

    emit this->changedLocation(oldLocation);

    if (!ret){
        return location;
    }else{
        return NULL;
    }
//    return ret;
}

QString UVCH264Cam::updateCurrentFilename()
{
    QDateTime time = QDateTime::currentDateTime();
    QDate date = QDate::currentDate();

    int day = date.day();
    int month = date.month();
    int year = date.year();

    this->location = this->basedir + "/" + QString::number(year) + QString::number(month) + QString::number(day) + "/" + QString::number(time.time().hour());

    QDir dir(this->location);
    if(!dir.exists()){
        dir.mkpath(".");
    }

    this->location += + "/minute_" + QString::number(time.time().minute()) + ".mp4";

    return this->location;

}

GstBusSyncReply UVCH264Cam::gstPipelineHandler(GstBus *bus, GstMessage *msg, GstPipeline *bin)
{
    /* Parse message */
     if (msg != NULL) {
       GError *err;
       gchar *debug_info;

       switch (GST_MESSAGE_TYPE (msg)) {
         case GST_MESSAGE_ERROR:
           gst_message_parse_error (msg, &err, &debug_info);
           g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
           g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
           g_clear_error (&err);
           g_free (debug_info);
           break;
         case GST_MESSAGE_EOS:
           g_print ("End-Of-Stream reached.\n");
           break;
         case GST_MESSAGE_STATE_CHANGED:
           /* We are only interested in state-changed messages from the pipeline */
           if (GST_MESSAGE_SRC (msg) == GST_OBJECT (bin)) {
             GstState old_state, new_state, pending_state;
             gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
             g_print ("Pipeline state changed from %s to %s:\n",
                 gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
           }
         default:
           /* We should not reach here because we only asked for ERRORs and EOS */
//           g_printerr ("Unexpected message received.\n");
           break;
       }

       gst_message_unref (msg);
     }
     return GST_BUS_DROP;
}
