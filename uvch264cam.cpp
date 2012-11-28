#include "uvch264cam.h"
#include <iostream>
#include <gst/video/video.h>

UVCH264Cam::UVCH264Cam(QObject *parent = 0) : QThread(parent)
{
//    Pipeline pipeline = Pipeline();
   this->keyFrameNumber = 0;

    this->bin = NULL;
    this->src = NULL;
    this->queue_src = NULL;
    this->identity = NULL;
    this->file_sink0 = NULL;
    this->file_sink1 = NULL;
    this->mp4mux0 = NULL;
    this->mp4mux1 = NULL;
    this->queue_0 = NULL;
    this->queue_1 = NULL;
    this->valve_0 = NULL;
    this->ph264 = NULL;
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
}

UVCH264Cam::~UVCH264Cam()
{
    if (this->bin){
        /* stop capturing the data */
        gst_element_send_event (this->bin, gst_event_new_eos());
        gst_element_set_state (this->bin, GST_STATE_NULL);

        /* Free resources */
//        if (msg != NULL)
//             gst_message_unref (msg);

        gst_object_unref (bus);

        gst_object_unref (this->bin);

        gst_object_unref (this->src);
        gst_object_unref (this->file_sink0);
        gst_object_unref (this->queue_0);
        gst_object_unref (this->mp4mux0);
//        gst_object_unref (this->valve_0);

        qDebug() << "disconnect!!!!";

        this->terminate();
    }
}




void UVCH264Cam::run()
{
    QString filename = updateCurrentFilename();
    this->changingLocation = false;


    GMainContext * context;
    context = g_main_context_new();
    gst_init(0,NULL);

    qDebug() << filename;

    //    const QString launcher = "uvch264_src device=" + this->device + " name=src auto-start=true initial-bitrate=6000000 rate-control=cbr src.vfsrc ! queue ! video/x-raw-yuv,width=320,height=240,framerate=30/1 ! xvimagesink src.vidsrc ! video/x-h264,width=1920,height=1080,framerate=30/1 ! tee name=t ! valve name=value_0 ! queue name=queue_0 ! h264parse name=ph264 ! mp4mux name=mux ! filesink name=file_sink0 location=" + filename + " t. queue ! filesink location=/tmp/vid.mp4";
    const QString launcher = "uvch264_src device=" + this->device + " name=src auto-start=true initial-bitrate=6000000 rate-control=cbr src.vfsrc ! queue ! video/x-raw-yuv,width=320,height=240,framerate=30/1 ! xvimagesink src.vidsrc ! video/x-h264,width=1920,height=1080,framerate=30/1 ! queue name=queue_0 ! queue name=queue_1 ! h264parse name=ph264 ! mp4mux name=mux ! idenity name=identity ! filesink location=" + filename + " name=file_sink0";
    this->bin = gst_parse_launch (launcher.toStdString().c_str(), NULL);
//    this->bin = gst_parse_launch("uvch264_src device=" + device + "name=src auto-start=true initial-bitrate=6000000 rate-control=cbr src.vfsrc ! queue ! video/x-raw-yuv,width=320,height=240,framerate=30/1 ! xvimagesink src.vidsrc ! queue ! video/x-h264,width=1920,height=1080,framerate=30/1 ! filesink name=file_sink0 location=" + filename;)


    this->file_sink0 = gst_bin_get_by_name (GST_BIN (this->bin), "file_sink0");
    this->src = gst_bin_get_by_name (GST_BIN (this->bin), "src");
    this->queue_0 = gst_bin_get_by_name (GST_BIN (this->bin), "queue_0");
    this->queue_1 = gst_bin_get_by_name (GST_BIN (this->bin), "queue_1");
    this->mp4mux0 = gst_bin_get_by_name (GST_BIN (this->bin), "mux");
    this->valve_0 = gst_bin_get_by_name (GST_BIN (this->bin), "valve_0");
    this->ph264 = gst_bin_get_by_name (GST_BIN (this->bin), "ph264");
    this->identity = gst_bin_get_by_name (GST_BIN (this->bin), "identity");
//    this->t = gst_bin_get_by_name (GST_BIN (this->bin), "t");



    //    g_object_set(this->queue_0, "max-size-bytes", 4294967295, NULL);
    g_object_set(this->file_sink0, "async", false, NULL);
    QString recoverPath = filename + ".moov";
    g_object_set(this->mp4mux0, "moov-recovery-file", recoverPath.toStdString().c_str(), NULL);

    msg = NULL;
    /* Wait until error or EOS */
    bus = gst_element_get_bus (this->bin);
    gst_bus_set_sync_handler (bus, (GstBusSyncHandler) gstPipelineHandler, this);

    int ret = gst_element_set_state (this->bin, GST_STATE_PLAYING);

    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (this->bin);
        gst_object_unref (this->src);
//        gst_object_unref (this->identity);
        gst_object_unref (this->file_sink0);
        gst_object_unref (this->queue_0);
        gst_object_unref (this->mp4mux0);
//        gst_object_unref (this->valve_0);
//        gst_object_unref (this->file_sink1);
//        gst_object_unref (this->t);
//        gst_object_unref (this->vid_capsfilter);
//        gst_object_unref (this->vf_capsfilter);
//        return -1;
      }
//    gst_bus_add_watch (bus, (GstBusFunc) gstPipelineHandler, this);

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(this->bin), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

//    this->busQueue_0 = gst_element_get_bus (this->file_sink0);
//    gst_bus_set_sync_handler (this->busQueue_0 , (GstBusSyncHandler) gstQueueHandler, this);


    qDebug() << "UVCH264Cam::run() finished "<< QDateTime::currentMSecsSinceEpoch();

    exec();
}

int UVCH264Cam::changeLocation(QString filename)
{
    qDebug() << "UVCH264Cam::changeLocation";
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
    qDebug() << "UVCH264Cam::changeDevice";
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
    qDebug() << "UVCH264Cam::getLocation";
    return this->location;
}

int UVCH264Cam::switchReview(bool toogle)
{
    qDebug() << "UVCH264Cam::changeLocation";
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
    qDebug() << "UVCH264Cam::disconnect()";
    this->changingLocation = false;

    if (this->bin){
        /* stop capturing the data */
        gst_element_send_event (this->bin, gst_event_new_eos());
        gst_element_set_state (this->bin, GST_STATE_NULL);
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

//    updateFilesink();
    if (this->bin != NULL){
//        ret = gst_element_set_state (this->mp4mux0, GST_STATE_NULL);
//        g_object_set(this->valve_0, "drop", true, NULL);
        qDebug() << "UVCH264Cam::changeLocationToCurrentTime(): send EOS";
        GstPad* pad = gst_element_get_pad(this->queue_0 , "src");
        gst_pad_set_blocked_async(pad, TRUE, handle_block_location, this);
        gst_object_unref(pad);
//        gst_element_send_event (this->mp4mux0, gst_event_new_eos());

//        gst_element_set_state(this->file_sink0, GST_STATE_NULL);
    }

//    emit this->changedLocation(oldLocation);

    if (!ret){
        return this->location;
    }else{
        return NULL;
    }
//    return ret;
}

QString UVCH264Cam::updateCurrentFilename()
{
    qDebug() << "UVCH264Cam::updateCurrentFilename";

    this->oldLocation = this->location;

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

void UVCH264Cam::updateFilesink()
{
    qDebug() << "UVCH264Cam::updateFilesink";
    int ret = 0;

    if (this->bin != NULL){
        GstClockTime gct = 10;
//        GstEvent* event =  gst_video_event_new_upstream_force_key_unit(gct, true, this->keyFrameNumber++);
//        qDebug() << this->keyFrameNumber;
//        gst_element_send_event (this->bin, event);

        ret = gst_element_set_state (this->file_sink0, GST_STATE_NULL);
//        QChar* path = location.data();
        std::string path = location.toStdString();
        qDebug() << "location.data() " << path.c_str() ;
        g_object_set(this->file_sink0, "location", path.c_str(),NULL);
        gint bufferStatus = 100;
        ret = gst_element_set_state (this->file_sink0, GST_STATE_PLAYING);
        g_object_get(this->queue_0, "current-level-bytes", &bufferStatus, NULL);
        qDebug() << bufferStatus <<  (bufferStatus / 4294967295.0f);
    }


}

GstBusSyncReply UVCH264Cam::gstPipelineHandler(GstBus *bus, GstMessage *msg, UVCH264Cam * cam)
{
//    qDebug() << "UVCH264Cam::gstPipelineHandler";
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
            if (GST_MESSAGE_SRC (msg) == GST_OBJECT (cam->bin)) {
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
//       gst_message_unref (msg);
     }
     return GST_BUS_DROP;
}

GstBusSyncReply UVCH264Cam::gstQueueHandler(GstBus *bus, GstMessage *msg, UVCH264Cam *cam)
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
            if((GST_MESSAGE_SRC (msg) == GST_OBJECT (cam->file_sink0))){
                if (cam->changingLocation){
                    cam->updateFilesink();
                    cam->changingLocation = false;
                    int ret = gst_element_set_state (cam->queue_0, GST_STATE_PLAYING);
                    g_object_set(cam->valve_0, "drop", false, NULL);

                 }
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

void UVCH264Cam::handle_block_location(GstPad *pad, gboolean blocked, gpointer user_data)
{
    GstEvent   *event;

    UVCH264Cam* cam = (UVCH264Cam*) user_data;

    if (blocked) {
        qDebug() << "UVCH264Cam::handle_block_location - true branch";
      GstPad     *peer = gst_pad_get_peer (pad);

      gint datarate;
      g_object_get(cam->identity, "datarate", &datarate, NULL);
      qDebug() << "UVCH264Cam::handle_block_location: datarate1= " << datarate;

      gst_pad_unlink (pad, peer);

//      gst_pad_set_blocked(pad, true);

//      GstElement * element = gst_pad_get_parent_element(peer);
//      gchar* elementName = gst_element_get_name(element);
//      qDebug() << elementName;
//      gst_object_unref(element);

      /* send EOS */
      event = gst_event_new_eos();
      gst_pad_push_event(peer, event);

      /* send flush  start */
      event = gst_event_new_flush_start();
      gst_pad_push_event(peer, event);

//      g_object_get(cam->identity, "datarate", &datarate, NULL);
//      qDebug() << "UVCH264Cam::handle_block_location: datarate after EOS= " << datarate;

//      while(datarate > 0){
//          g_object_get(cam->identity, "datarate", &datarate, NULL);
//          qDebug() << "UVCH264Cam::handle_block_location: LOOP datarate= " << datarate;
//          QThread::msleep(1);
//      }

      cam->updateFilesink();

      QString recoverPath = cam->getLocation() + ".moov";
      gst_element_set_state(cam->mp4mux0, GST_STATE_NULL);
      g_object_set(cam->mp4mux0, "moov-recovery-file", recoverPath.toStdString().c_str(), NULL);
      gst_element_set_state(cam->mp4mux0, GST_STATE_PLAYING);


      /* send flush  stop*/
      event = gst_event_new_flush_stop();
      gst_pad_push_event(peer, event);


      gst_element_set_state(cam->queue_1, GST_STATE_PLAYING);

      gst_pad_link(pad, peer);


      gst_pad_set_blocked_async (pad, FALSE, handle_block_location, cam);

      gst_object_unref (peer);
    }else{
//        gst_element_set_state(cam->queue_1, GST_STATE_PLAYING);
//        gst_element_set_state(cam->mp4mux0, GST_STATE_PLAYING);

        qDebug() << "UVCH264Cam::handle_block_location - false branch";

//        GstPad     *peer = gst_pad_get_peer (pad);
//        gst_pad_link (pad, peer);
//        gst_object_unref (peer);
    }

}
