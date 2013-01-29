#include "uvch264cam.h"
#include <iostream>
#include <gst/video/video.h>

UVCH264Cam::UVCH264Cam(QObject *parent = 0) : QThread(parent)
{
//    Pipeline pipeline = Pipeline();
   this->keyFrameNumber = 0;

    this->mainPipeline = NULL;
    this->src = NULL;
    this->queue_src = NULL;
    this->identity1 = NULL;
    this->identity2 = NULL;
    this->identity3 = NULL;
    this->file_sink1 = NULL;
    this->file_sink2 = NULL;
    this->file_sink3 = NULL;
    this->mp4mux1 = NULL;
    this->mp4mux2 = NULL;
    this->mp4mux3 = NULL;
    this->queue_0 = NULL;
    this->queue_1 = NULL;
    this->queue_2 = NULL;
    this->queue_3 = NULL;
    this->valve_0 = NULL;
    this->ph264_1 = NULL;
    this->ph264_2 = NULL;
    this->ph264_3 = NULL;
    this->t = NULL;
    this->vid_capsfilter = NULL;
    this->vf_capsfilter = NULL;
    this->preview_sink = NULL;
    this->queue_preview = NULL;

    this->bus = NULL;
    this->busQueue_0 = NULL;
    this->msg = NULL;

    this->basedir = "";
    this->device = "";
    this->location = "";
    this->oldLocation = "";

    this->handler_id = NULL;
    this->inSwapingBuffers = false;
    this->negoCaps = NULL;
}

UVCH264Cam::~UVCH264Cam()
{
    if (this->mainPipeline){
        /* stop capturing the data */
        gst_element_send_event (this->mainPipeline, gst_event_new_eos());
        gst_element_set_state (this->mainPipeline, GST_STATE_NULL);
        gst_element_send_event (this->catchPipeline, gst_event_new_eos());
        gst_element_set_state (this->catchPipeline, GST_STATE_NULL);

        gst_element_set_state (this->binRec1, GST_STATE_NULL);
        gst_element_set_state (this->binRec2, GST_STATE_NULL);

        gst_object_unref (bus);

        gst_object_unref (this->mainPipeline);
        gst_object_unref (this->catchPipeline);
        gst_object_unref (this->binRec1);
        gst_object_unref (this->binRec2);

        qDebug() << "disconnect!!!!";

        this->terminate();
    }
}




void UVCH264Cam::run()
{
    QString filename = updateCurrentFilename();
    this->changingLocation = false;
    this->inSwapingBuffers = false;


    GMainContext * context;
    context = g_main_context_new();
    gst_init(0,NULL);

    qDebug() << filename;

    this->src = gst_element_factory_make("uvch264_src", "src");
    this->queue_preview = gst_element_factory_make( "queue", "queue_preview");
    this->preview_sink = gst_element_factory_make( "xvimagesink", "previewsink");
    this->file_sink1 = gst_element_factory_make( "filesink", "file_sink1");
    this->file_sink2 = gst_element_factory_make( "filesink", "file_sink2");
    this->queue_0 = gst_element_factory_make( "queue", "queue_0");
    this->queue_1 = gst_element_factory_make( "queue", "queue_1");
    this->queue_2 = gst_element_factory_make( "queue", "queue_2");
    this->mp4mux1 = gst_element_factory_make ( "mp4mux", "mux_1");
    this->mp4mux2 = gst_element_factory_make( "mp4mux", "mux_2");
    this->ph264_1 = gst_element_factory_make ("h264parse", "ph264_1");
    this->ph264_2 = gst_element_factory_make( "h264parse", "ph264_2");
    this->identity1 = gst_element_factory_make("identity", "identity_1");
    this->identity2 = gst_element_factory_make( "identity", "identity_2");
    this->audio1 = gst_element_factory_make( "audiotestsrc", "audio1");
    this->audio2 = gst_element_factory_make( "audiotestsrc", "audio2");
    this->aenc1 = gst_element_factory_make( "faac", "encodebin1");
    this->aenc2 = gst_element_factory_make( "faac", "encodebin2");

    this->fakesrc = gst_element_factory_make( "fakesrc", "fakesrc");
    this->fakesink = gst_element_factory_make( "fakesink", "fakesink");
    this->queue_catch = gst_element_factory_make( "queue", "queue_catch");
    this->mainFakesink = gst_element_factory_make( "fakesink", "mainFakesink");

    this->t = gst_element_factory_make( "tee", "teee");

    if (!src || !queue_preview || !preview_sink || !file_sink1 || !file_sink2 || !queue_0 ||
            !queue_1 || !queue_2 || !mp4mux1|| !mp4mux2 || !ph264_1 || !queue_catch ||
            !ph264_2 || !identity1 || !identity2 || !fakesrc || !fakesink || !fakesink ||
            !audio1 || !audio2 || !aenc1 || !aenc2) {

        g_printerr ("Not all elements could be created.\n");
        qDebug() << "this->src" << this->src;
        qDebug() << "this->queue_preview" << this->queue_preview;
        qDebug() << "this->preview_sink" << this->preview_sink;
        qDebug() << "this->file_sink1" << this->file_sink1;
        qDebug() << "this->file_sink2" << this->file_sink2;
        qDebug() << "this->queue_0 "<< this->queue_0;
        qDebug() << "this->queue_1" << this->queue_1;
        qDebug() << "this->queue_2" << this->queue_2;
        qDebug() << "this->mp4mux1" << this->mp4mux1;
        qDebug() << "this->mp4mux2" << this->mp4mux2;
        qDebug() << "this->ph264_1" << this->ph264_1;
        qDebug() << "this->ph264_2" << this->ph264_2;
        qDebug() << "this->identity1" << this->identity1;
        qDebug() << "this->identity2" << this->identity2;
        qDebug() << "this->fakesrc" << this->fakesrc;
        qDebug() << "this->queue_catch" << this->queue_catch;
        qDebug() << "this->mainFakesink" << this->mainFakesink;
        qDebug() << "this->audio1" << this->audio1;
        qDebug() << "this->audio2" << this->audio2;
        qDebug() << "this->aenc1" << this->aenc1;
        qDebug() << "this->aenc2" << this->aenc2;
        return;
      }
    /* define video source caps */
    GstCaps *previewCaps;
    previewCaps = gst_caps_new_simple ("video/x-raw-yuv",
         "width", G_TYPE_INT, 320,
         "height", G_TYPE_INT, 240,
         "framerate", GST_TYPE_FRACTION, 30, 1, NULL);

    h264Caps = gst_caps_new_simple ("video/x-h264",
         "width", G_TYPE_INT, 1920,
         "height", G_TYPE_INT, 1080,
         "profile",  G_TYPE_STRING, "high",
         "framerate", GST_TYPE_FRACTION, 30, 1, NULL);

    /* do element settings */
    g_object_set(this->src, "device", this->device.toStdString().c_str(), "auto-start", true, "initial-bitrate", 12000000, \
                 "iframe-period", 1000, "usage-type", 1, NULL);
    g_object_set(this->src, "async-handling", true, "message-forward", true, NULL);
    g_object_set(this->fakesrc, "is-live", true, "sync", true, NULL);

    g_object_set(this->file_sink1, "sync", FALSE, "async", FALSE, "location", this->location.toStdString().c_str(), NULL);
    g_object_set(this->file_sink2, "sync", FALSE, "async", FALSE, "location", this->location.toStdString().c_str(), NULL);
    QString recoverPath = filename + ".moov";
    g_object_set(this->mp4mux1, "moov-recovery-file", recoverPath.toStdString().c_str(),  "fragment-duration", 1000, NULL);
    g_object_set(this->mp4mux2, "moov-recovery-file", (recoverPath + "test").toStdString().c_str(),  "fragment-duration", 1000, NULL);

    g_object_set(this->audio1, "wave", 4, NULL);
    g_object_set(this->audio2, "wave", 4, NULL);

    /* set up bins and pipelines */
    this->mainPipeline = gst_pipeline_new("vidsrc pipeline");
    this->catchPipeline = gst_pipeline_new("catch pipeline");
    this->binRec1 = gst_bin_new("rec1 bin");
    this->binRec2 = gst_bin_new("rec2 bin");

    gst_bin_add_many(GST_BIN(this->mainPipeline), this->src, this->queue_preview, this->preview_sink, this->queue_0, NULL);
    gst_bin_add_many(GST_BIN(this->catchPipeline), this->fakesrc, this->queue_catch/*, this->fakesink*/, NULL);
    gst_bin_add_many(GST_BIN(this->binRec1), this->queue_1, this->ph264_1, this->mp4mux1, this->identity1, this->file_sink1, this->aenc1, this->audio1, NULL);
    gst_bin_add_many(GST_BIN(this->binRec2), this->queue_2, this->ph264_2, this->mp4mux2, this->identity2, this->file_sink2, this->aenc2, this->audio2, NULL);


    /* create pads for src and link them to connecting queues*/
    qDebug() << "both source source pads connected well:";
    qDebug() << (gst_element_link_pads_filtered(this->src, "vfsrc", this->queue_preview, "sink", previewCaps) != GST_PAD_LINK_OK);
    qDebug() << (gst_element_link_pads_filtered(this->src, "vidsrc", this->queue_0, "sink", h264Caps) != GST_PAD_LINK_OK);

    /* link always pads */
    gst_element_link(this->queue_preview, this->preview_sink);
    gst_element_link_many(this->fakesrc, this->queue_catch/*, this->fakesink*/, NULL);

    gst_element_link_many(this->queue_1, this->ph264_1, this->mp4mux1, this->identity1, this->file_sink1, NULL);
    gst_element_link_many(this->queue_2, this->ph264_2, this->mp4mux2, this->identity2, this->file_sink2, NULL);

    /* add ghost pads to rec bins */
    GstPad* pad = gst_element_get_static_pad (this->queue_1, "sink");
    gst_element_add_pad (this->binRec1, gst_ghost_pad_new ("sink", pad));
    gst_object_unref (GST_OBJECT (pad));

    pad = gst_element_get_static_pad (this->queue_2, "sink");
    gst_element_add_pad (this->binRec2, gst_ghost_pad_new ("sink", pad));
    gst_object_unref (GST_OBJECT (pad));

    /* connect the first recording bin to main pipeline */
    gst_bin_add(GST_BIN(this->mainPipeline), this->binRec1);
    gst_element_link(this->queue_0, this->binRec1);
    this->activeBuffer = 1;

    /* connect passive recording bin to catch pipeline */
    gst_bin_add(GST_BIN(this->catchPipeline), this->binRec2);
    gst_element_link(this->queue_catch, this->binRec2);

    msg = NULL;

    /* Wait until error or EOS */
    bus = gst_element_get_bus (this->mainPipeline);
    gst_bus_add_watch (bus, (GstBusFunc) gstPipelineHandler, this);

    bus = gst_element_get_bus (this->catchPipeline);
    gst_bus_add_watch (bus, (GstBusFunc) gstPipelineHandler, this);
//    gst_bus_set_sync_handler (bus, (GstBusSyncHandler) gstPipelineHandler, this);

    this->capInspectCounter = 0;
    pad = gst_element_get_pad(this->ph264_1, "sink");
    this->save_id = gst_pad_add_buffer_probe (pad, GCallback(saveMeta), this);

    /* run pipeline */
    int ret = gst_element_set_state (this->mainPipeline, GST_STATE_PLAYING);

    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (this->mainPipeline);
        gst_object_unref (this->src);
        gst_object_unref (this->file_sink2);
        gst_object_unref (this->queue_0);
        gst_object_unref (this->mp4mux1);
      }

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(this->mainPipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

    exec();
}

int UVCH264Cam::changeLocation(QString filename)
{
    qDebug() << "UVCH264Cam::changeLocsaveMetaation";
    int ret = gst_element_set_state (this->file_sink2, GST_STATE_NULL);
    g_object_set(this->file_sink2, "location", filename.data(), /*"append", true,*/ NULL);
    ret = gst_element_set_state (this->file_sink2, GST_STATE_PLAYING);

    return ret;
}

int UVCH264Cam::changeBaseDir(QString baseDir)
{
    this->basedir = baseDir;
}

int UVCH264Cam::changeDevice(QString device)
{
    qDebug() << "UVCH264Cam::changeDevice";
    if (this->mainPipeline){
        /* stop capturing the data */
        gst_element_set_state (this->mainPipeline, GST_STATE_NULL);

        gst_object_unref (bus);

        gst_object_unref (this->mainPipeline);

        this->exit();

        this->device = device;

        this->start();
    }else{
        this->device = device;
    }
}

QString UVCH264Cam::getLocation()
{
    qDebug() << "UVCH264Cam::getLocation";
    return this->location;
}

int UVCH264Cam::switchReview(bool toogle)
{
    qDebug() << "UVCH264Cam::changeLocation";
    if(this->mainPipeline){
        if (toogle){
            gst_element_unlink(this->queue_0, this->file_sink2);
        }else{
            gst_element_link(this->queue_0, this->file_sink2);
        }
    }
}

int UVCH264Cam::disconnect()
{
    qDebug() << "UVCH264Cam::disconnect()";
    this->changingLocation = false;

    if (this->mainPipeline){
        /* stop capturing the data */
        gst_element_send_event (this->mainPipeline, gst_event_new_eos());
        gst_element_set_state (this->mainPipeline, GST_STATE_NULL);
    }
    this->quit();
}

QString UVCH264Cam::changeLocationToCurrentTime(QString baseDir)
{
    qDebug() << "UVCH264Cam::changeLocation";
    this->changeBaseDir(baseDir);

    return changeLocationToCurrentTime();
}

QString UVCH264Cam::changeLocationToCurrentTime()
{
    qDebug() << "UVCH264Cam::changeLocationToCurrentTime";
    updateCurrentFilename();

    this->changingLocation = true;

    int ret = 0;

    GstPad* pad = gst_element_get_pad(this->queue_0, "src");
    this->inSwapingBuffers = true;
//    if (!this->handler_id)
        this->handler_id = gst_pad_add_buffer_probe (pad, GCallback(checkForKeyframes), this);

    GstEvent* event = gst_video_event_new_upstream_force_key_unit(GST_CLOCK_TIME_NONE, true, this->keyFrameNumber++);
    gst_element_send_event (this->src, event);

    qDebug() << "UVCH264Cam::changeLocationToCurrentTime: went through";

    if (!ret){
        return this->location;
    }else{
        return NULL;
    }
}

QString UVCH264Cam::updateCurrentFilename()
{
    qDebug() << "UVCH264Cam::updateCurrentFilename";

    this->oldLocation = this->location;

    this->location = this->basedir + QDateTime::currentDateTime().toString("yyyyMMdd/hh");

    QDir dir(this->location);
    if(!dir.exists()){
        dir.mkpath(".");
    }

    this->location += "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd.hh-mm-ss") + ".mp4";

    return this->location;

}

/*  update locations of filesink and mp4mux (for moov-recovery-file)
 *
 */
void UVCH264Cam::updateFilesinkLocation(GstElement* filesink, GstElement* mp4mux)
{
    qDebug() << "UVCH264Cam::updateFilesink";
    int ret = 0;

    if (this->mainPipeline != NULL){
        ret = gst_element_set_state (filesink, GST_STATE_NULL);
        ret = gst_element_set_state (mp4mux, GST_STATE_NULL);

        std::string path = this->location.toStdString();
        qDebug() << "location.data() " << path.c_str() ;
        g_object_set(filesink, "location", path.c_str(),NULL);
        QString recoverPath = this->location + ".moov";
        g_object_set(mp4mux, "moov-recovery-file", recoverPath.toStdString().c_str(), NULL);

        ret = gst_element_set_state (mp4mux, GST_STATE_PLAYING);
        ret = gst_element_set_state (filesink, GST_STATE_PLAYING);
    }
}

void UVCH264Cam::swapBuffers(GstPad* pad, gboolean blocked, gpointer user_data)
{
    UVCH264Cam* cam = (UVCH264Cam*)user_data;
    if (blocked){
        qDebug() << " UVCH264Cam::swapBuffers(): activeBuffer==" << cam->activeBuffer;
        GstElement* oldBinRec;      /* currently active bin */
        GstElement* newBinRec;      /* bin that will be installed to continue saving */
        GstElement* newMp4mux;      /* mp4mux of newBinRec */
        GstElement* newParser;      /* parser of newBinRec */
        GstElement* newFilesink;    /* filesing of newBinRec */
        GstElement* queue;          /* main camera queue (left side of connection) */
        GstElement* binQueue;       /* queue of oldBinRec (right side of connection) */

        if(cam->activeBuffer == 1){
            oldBinRec = cam->binRec1;
            newBinRec = cam->binRec2;
            newMp4mux = cam->mp4mux2;
            newParser = cam->ph264_2;
            newFilesink = cam->file_sink2;
            queue = cam->queue_0;
            binQueue = cam->queue_2;
            cam->activeBuffer = 2;
        }else if(cam->activeBuffer == 2){
            oldBinRec = cam->binRec2;
            newBinRec = cam->binRec1;
            newMp4mux = cam->mp4mux1;
            newParser = cam->ph264_1;
            newFilesink = cam->file_sink1;
            queue = cam->queue_0;
            binQueue = cam->queue_1;
            cam->activeBuffer = 1;
        }else{
            qWarning() << "UVCH264Cam::swapBuffers: activeBuffer out of range";
        }

        qDebug() << "/* prepare new file sink and mp4mux */";
        cam->updateFilesinkLocation(newFilesink, newMp4mux);
        gst_element_set_state(cam->catchPipeline, GST_STATE_PLAYING);

        qDebug() << "/* unlink new binRec, prepare to replace old binRec */";
        gst_element_unlink(cam->queue_catch, newBinRec);

        gst_bin_remove(GST_BIN(cam->catchPipeline), newBinRec);
        gst_element_set_state(newBinRec, GST_STATE_NULL);

        qDebug() << "/* pass actual rec bin to catch pipeline and unblock flow*/";
        gst_element_unlink(cam->queue_0, oldBinRec);
        gst_bin_remove(GST_BIN(cam->mainPipeline), oldBinRec);
        gst_bin_add(GST_BIN(cam->catchPipeline), oldBinRec);
        gst_element_link(cam->queue_catch, oldBinRec);

        qDebug() << "/* preparing catch pipeline (set playing, load with EOS) */";
        GstEvent* event = gst_event_new_eos();
        gst_element_send_event(cam->queue_catch, event);
        gst_event_unref(event);

        qDebug() << "/* link new bin */";
        gst_bin_add(GST_BIN(cam->mainPipeline), newBinRec);
        gst_element_set_state(newBinRec, GST_STATE_PLAYING);

        qDebug () << "/* attach new rec bin to camera src */";
        gst_element_link(queue, newBinRec);

        qDebug() << "====================================================================================================";
        qDebug() << "====================================================================================================";
        qDebug() << "====================================================================================================";
        qDebug() << "====================================================================================================";
        qDebug() << "====================================================================================================";
        qDebug() << "====================================================================================================";
        qDebug() << "====================================================================================================";

        /* prope buffers arriving in new binRec for H264 header */
        GstPad* queuePad = gst_element_get_static_pad(binQueue, "sink");
        cam->queueCounter = 0;
        cam->queueCheckID = gst_pad_add_buffer_probe(queuePad, GCallback(dropFirstBuffer), cam);
        gst_object_unref(GST_OBJECT(queuePad));

        qDebug() << "end of switch";
        gst_pad_set_blocked_async(pad, false, swapBuffers, cam);
    }else{

        /* TEMPORARY TRIAL TO DELETE OLD BINREC -- NOT SUCCESSFUL FOR SOME REASON */
        gst_element_set_state(cam->catchPipeline, GST_STATE_NULL);

        if(cam->activeBuffer == 2){
            gst_bin_remove(GST_BIN(cam->catchPipeline), cam->binRec1);
            gst_object_unref(GST_OBJECT (cam->binRec1));
        }else{
            gst_bin_remove(GST_BIN(cam->catchPipeline), cam->binRec2);
            gst_object_unref(GST_OBJECT (cam->binRec2));

        }

    }
}


int UVCH264Cam::checkForKeyframes(GstPad *pad, GstBuffer *buffer, gpointer user_data)
{
    UVCH264Cam* cam = (UVCH264Cam*) user_data;

    if(!GST_BUFFER_FLAG_IS_SET(buffer, GST_BUFFER_FLAG_DELTA_UNIT)) {
        // keyframe
        qDebug() << "keyframe detected";
        gst_pad_remove_data_probe(pad, cam->handler_id);

        if(cam->inSwapingBuffers){
            /* copy h264 header of camera to new keyframe */
            GstBuffer* newBuffer = gst_buffer_try_new_and_alloc(cam->byteHeader.size());
            memcpy(GST_BUFFER_DATA(newBuffer), cam->byteHeader.data_ptr(), cam->byteHeader.size());
            buffer = gst_buffer_merge(newBuffer, buffer);
            gst_buffer_unref(newBuffer);

            /* do changes asyncronous, the documentation disapproves state changes
            /* in probes and guarantees that the async block will happen
            /* before the data arrives at the pad */
            cam->inSwapingBuffers = false;
            gst_pad_set_blocked_async(pad, true, swapBuffers, cam);

        }
    }else{
        qDebug() << "skip non-keyframe";
    }

    return GST_PAD_PROBE_PASS;
}

/*
 *  copy and save H264 meta data for insertiion after pipeline swap
 */

int UVCH264Cam::saveMeta(GstPad *pad, GstBuffer *buffer, gpointer user_data)
{
    qDebug() << "UVCH264Cam::saveMeta";

    UVCH264Cam* cam = (UVCH264Cam*)user_data;
    gst_pad_remove_data_probe(pad, cam->save_id);

    /* copy first 60 bytes of camera stream that are used to negotiate meta
    /* data between 264parse and mp4mux */
    if(cam->byteHeader.isEmpty()){
        cam->byteHeader = QByteArray((char*)GST_BUFFER_DATA(buffer), 56);
        qDebug() << "copied byteheader: " << cam->byteHeader.toHex();
    }

    return GST_PAD_PROBE_PASS;
}

/*
 *  drops buffers that do not have a proper H264 header
 *  (used after pipeline switch)
 */

int UVCH264Cam::dropFirstBuffer(GstPad *pad, GstBuffer *buffer, gpointer user_data)
{
    qDebug() << "UVCH264Cam::dropFirstBuffer: entering function";

    UVCH264Cam* cam = (UVCH264Cam*) user_data;

    /* this sleep seems to make the memory leak smaller for some reason */
    QThread::msleep(10);

    QByteArray cmpArray((char*)GST_BUFFER_DATA(buffer), cam->byteHeader.size());


    if(cmpArray == cam->byteHeader){
        qDebug() << "both headers are the same!!";
        gst_pad_remove_buffer_probe(pad, cam->queueCheckID);
        qDebug() << "pass buffer";
        return GST_PAD_PROBE_PASS;

    }else{
        qDebug() << "cmpArray: " << cmpArray.toHex();
        qDebug() << "byteheader: " << cam->byteHeader.toHex();
        qDebug() << "drop buffer";
        return GST_PAD_PROBE_DROP;
    }
}

/*
 * handler to parse error and other messages inside the pipeline
 * useful for debugging
 */
GstBusSyncReply UVCH264Cam::gstPipelineHandler(GstBus *bus, GstMessage *msg, UVCH264Cam * cam)
{
    qDebug() << "UVCH264Cam::gstPipelineHandler" << msg;
    /* Parse message */
     if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE (msg)) {

        case    GST_MESSAGE_UNKNOWN:
            qDebug() << "message is of type " << "GST_MESSAGE_UNKNOWN";
            break;
        case GST_MESSAGE_EOS:
            g_print ("End-Of-Stream reached.\n");
 //           if((GST_MESSAGE_SRC (msg) == GST_OBJECT (cam->queue_0))){
 //               if (cam->changingLocation){
 //                   cam->updateFilesink();
 //                   cam->changingLocation = false;
 //                   int ret = gst_element_set_state (cam->queue_0, GST_STATE_PLAYING);
 ////                   g_object_set(cam->valve_0, "drop", false, NULL);

 //                }
 //           }
            break;
        case GST_MESSAGE_ERROR:
            gst_message_parse_error (msg, &err, &debug_info);
            g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
            g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error (&err);
            g_free (debug_info);
            break;
        case GST_MESSAGE_WARNING:
            gst_message_parse_warning(msg, &err, &debug_info);
            g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
            g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
            break;
        case GST_MESSAGE_INFO:
            gst_message_parse_info(msg, &err, &debug_info);
            g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
            g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
            break;
        case GST_MESSAGE_TAG:
//            qDebug() << "message is of type " << "GST_MESSAGE_TAG " << GST_OBJECT_NAME (msg->src);
//            GstTagList *tags = NULL;
//            gst_message_parse_tag (msg, &tags);
//            qDebug() << "Got tags from element %s\n"
//            debug_info = gst_tag_list_to_string(tags);
//            qDebug() << "taglist:" << debug_info;
//            gst_tag_list_unref (tags);
            break;
        case GST_MESSAGE_BUFFERING:
            qDebug() << "message is of type " << "GST_MESSAGE_BUFFERING";
            break;
        case GST_MESSAGE_STATE_CHANGED:
            /* We are only interested in state-changed messages from the pipeline */
            if (GST_MESSAGE_SRC (msg) == GST_OBJECT (cam->mainPipeline)) {
            GstState old_state, new_state, pending_state;
            gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
            g_print ("Pipeline state changed from %s to %s:\n",
            gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
            }
            break;
        case GST_MESSAGE_STATE_DIRTY:
            qDebug() << "message is of type " << "GST_MESSAGE_STATE_DIRTY";
            break;
        case GST_MESSAGE_STEP_DONE:
            qDebug() << "message is of type " << "GST_MESSAGE_STEP_DONE";
            break;
        case GST_MESSAGE_CLOCK_PROVIDE:
            qDebug() << "message is of type " << "GST_MESSAGE_CLOCK_PROVIDE";
            break;
        case GST_MESSAGE_CLOCK_LOST:
            qDebug() << "message is of type " << "GST_MESSAGE_CLOCK_LOST";
            break;
        case GST_MESSAGE_NEW_CLOCK:
            qDebug() << "message is of type " << "GST_MESSAGE_NEW_CLOCK";
            break;
        case GST_MESSAGE_STRUCTURE_CHANGE:
            qDebug() << "message is of type " << "GST_MESSAGE_STRUCTURE_CHANGE";
            break;
        case GST_MESSAGE_STREAM_STATUS:
            GstStreamStatusType *type;
            GstElement* element;
            gst_message_parse_stream_status(msg, type, &element);
//            qDebug() << "message is of type " << "GST_MESSAGE_STREAM_STATUS " << *type;
            break;
        case GST_MESSAGE_APPLICATION:
            qDebug() << "message is of type " << "GST_MESSAGE_APPLICATION";
            break;
        case GST_MESSAGE_ELEMENT:
            qDebug() << "message is of type " << "GST_MESSAGE_ELEMENT" << GST_OBJECT_NAME (msg->src);
//            if (GST_MESSAGE_SRC (msg) == GST_OBJECT(cam->src)){
//                qDebug() << "element is " << "cam->src";
//            }else if (GST_MESSAGE_SRC (msg) == GST_OBJECT(cam->src)){
//                qDebug() << "element is " << "cam->src";
//            }else if (GST_MESSAGE_SRC (msg) == GST_OBJECT(cam->queue_0)){
//                qDebug() << "element is " << "cam->queue_0";
//            }else if (GST_MESSAGE_SRC (msg) == GST_OBJECT(cam->ph264)){
//                qDebug() << "element is " << "cam->ph264";
//            }else if (GST_MESSAGE_SRC (msg) == GST_OBJECT(cam->mp4mux0)){
//                qDebug() << "element is " << "cam->mp4mux0";
//            }else if (GST_MESSAGE_SRC (msg) == GST_OBJECT(cam->file_sink0)){
//                qDebug() << "element is " << "cam->file_sink0";
//            }else if (GST_MESSAGE_SRC (msg) == GST_OBJECT(cam->bin)){
//                qDebug() << "element is " << "cam->bin";
//            }
            break;
        case GST_MESSAGE_SEGMENT_START:
            qDebug() << "message is of type " << "GST_MESSAGE_SEGMENT_START";
            break;
        case GST_MESSAGE_SEGMENT_DONE:
            qDebug() << "message is of type " << "GST_MESSAGE_SEGMENT_DONE";
            break;
        case GST_MESSAGE_DURATION:
            qDebug() << "message is of type " << "GST_MESSAGE_DURATION";
            break;
        case GST_MESSAGE_LATENCY:
            qDebug() << "message is of type " << "GST_MESSAGE_LATENCY";
            break;
        case GST_MESSAGE_ASYNC_START:
            qDebug() << "message is of type " << "GST_MESSAGE_ASYNC_START";
            break;
        case GST_MESSAGE_ASYNC_DONE:
            qDebug() << "message is of type " << "GST_MESSAGE_ASYNC_DONE "  << GST_OBJECT_NAME (msg->src);
            break;
        case GST_MESSAGE_REQUEST_STATE:
            qDebug() << "message is of type " << "GST_MESSAGE_REQUEST_STATE";
            break;
        case GST_MESSAGE_STEP_START:
            qDebug() << "message is of type " << "GST_MESSAGE_STEP_START";
            break;
        case GST_MESSAGE_QOS:
            qDebug() << "message is of type " << "GST_MESSAGE_QOS";
            break;
        case GST_MESSAGE_PROGRESS:
            qDebug() << "message is of type " << "GST_MESSAGE_PROGRESS";
            break;
        case GST_MESSAGE_ANY:
            qDebug() << "message is of type " << "GST_MESSAGE_ANY";
            break;
        default:
           /* We should not reach here because we only asked for ERRORs and EOS */
           g_printerr ("Unexpected message received.\n");
           break;
       }
       gst_message_unref (msg);
     }
     return GST_BUS_DROP;
}

bool UVCH264Cam::gstQueueHandler(GstBus *bus, GstMessage *msg, UVCH264Cam *cam)
{
    qDebug() << "UVCH264Cam::gstQueueHandler";
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
//            if((GST_MESSAGE_SRC (msg) == GST_OBJECT (cam->file_sink2))){
//                if (cam->changingLocation){
//                    cam->updateFilesink();
//                    cam->changingLocation = false;
//                    int ret = gst_element_set_state (cam->queue_0, GST_STATE_PLAYING);
//                    g_object_set(cam->valve_0, "drop", false, NULL);

//                 }
//            }
        default:
          /* We should not reach here because we only asked for ERRORs and EOS */
//           g_printerr ("Unexpected message received.\n");
          break;
      }

      gst_message_unref (msg);
    }
    return GST_BUS_PASS;

}

#define GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME "GstForceKeyUnit"

/**
 * gst_video_event_new_downstream_force_key_unit:
 * @timestamp: the timestamp of the buffer that starts a new key unit
 * @stream_time: the stream_time of the buffer that starts a new key unit
 * @running_time: the running_time of the buffer that starts a new key unit
 * @all_headers: %TRUE to produce headers when starting a new key unit
 * @count: integer that can be used to number key units
 *
 * Creates a new downstream force key unit event. A downstream force key unit
 * event can be sent down the pipeline to request downstream elements to produce
 * a key unit. A downstream force key unit event must also be sent when handling
 * an upstream force key unit event to notify downstream that the latter has been
 * handled.
 *
 * To parse an event created by gst_video_event_new_downstream_force_key_unit() use
 * gst_video_event_parse_downstream_force_key_unit().
 *
 * Returns: The new GstEvent
 * Since: 0.10.36
 */
GstEvent *
UVCH264Cam::gst_video_event_new_downstream_force_key_unit (GstClockTime timestamp,
    GstClockTime stream_time, GstClockTime running_time, gboolean all_headers,
    guint count)
{
  GstEvent *force_key_unit_event;
  GstStructure *s;

  s = gst_structure_new (GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME,
      "timestamp", G_TYPE_UINT64, timestamp,
      "stream-time", G_TYPE_UINT64, stream_time,
      "running-time", G_TYPE_UINT64, running_time,
      "all-headers", G_TYPE_BOOLEAN, all_headers,
      "count", G_TYPE_UINT, count, NULL);
  force_key_unit_event = gst_event_new_custom (GST_EVENT_CUSTOM_DOWNSTREAM, s);

  return force_key_unit_event;
}

/**
 * gst_video_event_new_upstream_force_key_unit:
 * @running_time: the running_time at which a new key unit should be produced
 * @all_headers: %TRUE to produce headers when starting a new key unit
 * @count: integer that can be used to number key units
 *
 * Creates a new upstream force key unit event. An upstream force key unit event
 * can be sent to request upstream elements to produce a key unit.
 *
 * @running_time can be set to request a new key unit at a specific
 * running_time. If set to GST_CLOCK_TIME_NONE, upstream elements will produce a
 * new key unit as soon as possible.
 *
 * To parse an event created by gst_video_event_new_downstream_force_key_unit() use
 * gst_video_event_parse_downstream_force_key_unit().
 *
 * Returns: The new GstEvent
 * Since: 0.10.36
 */
GstEvent *
UVCH264Cam::gst_video_event_new_upstream_force_key_unit (GstClockTime running_time,
    gboolean all_headers, guint count)
{
  GstEvent *force_key_unit_event;
  GstStructure *s;

  s = gst_structure_new (GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME,
      "running-time", GST_TYPE_CLOCK_TIME, running_time,
      "all-headers", G_TYPE_BOOLEAN, all_headers,
      "count", G_TYPE_UINT, count, NULL);
  force_key_unit_event = gst_event_new_custom (GST_EVENT_CUSTOM_UPSTREAM, s);

  return force_key_unit_event;
}

/**
 * gst_video_event_is_force_key_unit:
 * @event: A #GstEvent to check
 *
 * Checks if an event is a force key unit event. Returns true for both upstream
 * and downstream force key unit events.
 *
 * Returns: %TRUE if the event is a valid force key unit event
 * Since: 0.10.36
 */
gboolean
UVCH264Cam::gst_video_event_is_force_key_unit (GstEvent * event)
{
  const GstStructure *s;

  g_return_val_if_fail (event != NULL, FALSE);

  if (GST_EVENT_TYPE (event) != GST_EVENT_CUSTOM_DOWNSTREAM &&
      GST_EVENT_TYPE (event) != GST_EVENT_CUSTOM_UPSTREAM)
    return FALSE;               /* Not a force key unit event */

  s = gst_event_get_structure (event);
  if (s == NULL
      || !gst_structure_has_name (s, GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME))
    return FALSE;

  return TRUE;
}

/**
 * gst_video_event_parse_downstream_force_key_unit:
 * @event: A #GstEvent to parse
 * @timestamp: (out): A pointer to the timestamp in the event
 * @stream_time: (out): A pointer to the stream-time in the event
 * @running_time: (out): A pointer to the running-time in the event
 * @all_headers: (out): A pointer to the all_headers flag in the event
 * @count: (out): A pointer to the count field of the event
 *
 * Get timestamp, stream-time, running-time, all-headers and count in the force
 * key unit event. See gst_video_event_new_downstream_force_key_unit() for a
 * full description of the downstream force key unit event.
 *
 * Returns: %TRUE if the event is a valid downstream force key unit event.
 * Since: 0.10.36
 */
gboolean
UVCH264Cam::gst_video_event_parse_downstream_force_key_unit (GstEvent * event,
    GstClockTime * timestamp, GstClockTime * stream_time,
    GstClockTime * running_time, gboolean * all_headers, guint * count)
{
  const GstStructure *s;
  GstClockTime ev_timestamp, ev_stream_time, ev_running_time;
  gboolean ev_all_headers;
  guint ev_count;

  g_return_val_if_fail (event != NULL, FALSE);

  if (GST_EVENT_TYPE (event) != GST_EVENT_CUSTOM_DOWNSTREAM)
    return FALSE;               /* Not a force key unit event */

  s = gst_event_get_structure (event);
  if (s == NULL
      || !gst_structure_has_name (s, GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME))
    return FALSE;

  if (!gst_structure_get_clock_time (s, "timestamp", &ev_timestamp))
    ev_timestamp = GST_CLOCK_TIME_NONE;
  if (!gst_structure_get_clock_time (s, "stream-time", &ev_stream_time))
    ev_stream_time = GST_CLOCK_TIME_NONE;
  if (!gst_structure_get_clock_time (s, "running-time", &ev_running_time))
    ev_running_time = GST_CLOCK_TIME_NONE;
  if (!gst_structure_get_boolean (s, "all-headers", &ev_all_headers))
    ev_all_headers = FALSE;
  if (!gst_structure_get_uint (s, "count", &ev_count))
    ev_count = 0;

  if (timestamp)
    *timestamp = ev_timestamp;

  if (stream_time)
    *stream_time = ev_stream_time;

  if (running_time)
    *running_time = ev_running_time;

  if (all_headers)
    *all_headers = ev_all_headers;

  if (count)
    *count = ev_count;

  return TRUE;
}

/**
 * gst_video_event_parse_upstream_force_key_unit:
 * @event: A #GstEvent to parse
 * @running_time: (out): A pointer to the running_time in the event
 * @all_headers: (out): A pointer to the all_headers flag in the event
 * @count: (out): A pointer to the count field in the event
 *
 * Get running-time, all-headers and count in the force key unit event. See
 * gst_video_event_new_upstream_force_key_unit() for a full description of the
 * upstream force key unit event.
 *
 * Create an upstream force key unit event using  gst_video_event_new_upstream_force_key_unit()
 *
 * Returns: %TRUE if the event is a valid upstream force-key-unit event. %FALSE if not
 * Since: 0.10.36
 */
gboolean
UVCH264Cam::gst_video_event_parse_upstream_force_key_unit (GstEvent * event,
    GstClockTime * running_time, gboolean * all_headers, guint * count)
{
  const GstStructure *s;
  GstClockTime ev_running_time;
  gboolean ev_all_headers;
  guint ev_count;

  g_return_val_if_fail (event != NULL, FALSE);

  if (GST_EVENT_TYPE (event) != GST_EVENT_CUSTOM_UPSTREAM)
    return FALSE;               /* Not a force key unit event */

  s = gst_event_get_structure (event);
  if (s == NULL
      || !gst_structure_has_name (s, GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME))
    return FALSE;

  if (!gst_structure_get_clock_time (s, "running-time", &ev_running_time))
    ev_running_time = GST_CLOCK_TIME_NONE;
  if (!gst_structure_get_boolean (s, "all-headers", &ev_all_headers))
    ev_all_headers = FALSE;
  if (!gst_structure_get_uint (s, "count", &ev_count))
    ev_count = 0;

  if (running_time)
    *running_time = ev_running_time;

  if (all_headers)
    *all_headers = ev_all_headers;

  if (count)
    *count = ev_count;

  return TRUE;
}


