#include <QtGui/QApplication>
#include "mainwindow.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <gst/video/video.h>
#include <gst/gstdebugutils.h>


#define WINDOW_GLADE "window.glade"
#define INT_PROPERTY_GLADE "int_property.glade"
#define ENUM_PROPERTY_GLADE "enum_property.glade"
#define BOOL_PROPERTY_GLADE "boolean_property.glade"

#define PROPERTY_TO_VBOX                                                \
  properties[i].dynamic ? GTK_BOX (dynamic_vbox) : GTK_BOX (static_vbox)

#define GET_WIDGET(object, type, name)                          \
  type (gtk_builder_get_object ((object)->builder, name))

#define GET_PROP_WIDGET(type, name) GET_WIDGET (&(properties[i]), type, name)


#include <unistd.h>
//#include <QGlib/Error>
//#include <QGlib/Connect>
//#include <QGst/Init>
//#include <QGst/ElementFactory>
//#include <QGst/ChildProxy>
//#include <QGst/PropertyProbe>
//#include <QGst/Pipeline>
//#include <QGst/Pad>
//#include <QGst/Event>
//#include <QGst/Message>
//#include <QGst/Bus>
#include <QDebug>

static guint h264_xid, preview_xid;

//typedef struct
//{

//  GstElement *src;
//  enum
//  { NONE, INT, ENUM, BOOL } type;
//  const gchar *property_name;
//  gboolean readonly;
//  gboolean dynamic;
//} Prop;

class Pipeline
{
public:
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
};

//Prop properties[] = {
//  {NULL, NULL, INT, "initial-bitrate", FALSE, FALSE},
//  {NULL, NULL, INT, "slice-units", FALSE, FALSE},
//  {NULL, NULL, ENUM, "slice-mode", FALSE, FALSE},
//  {NULL, NULL, INT, "iframe-period", FALSE, FALSE},
//  {NULL, NULL, ENUM, "usage-type", FALSE, FALSE},
//  {NULL, NULL, ENUM, "entropy", FALSE, FALSE},
//  {NULL, NULL, BOOL, "enable-sei", FALSE, FALSE},
//  {NULL, NULL, INT, "num-reorder-frames", FALSE, FALSE},
//  {NULL, NULL, BOOL, "preview-flipped", FALSE, FALSE},
//  {NULL, NULL, INT, "leaky-bucket-size", FALSE, FALSE},
//  {NULL, NULL, INT, "num-clock-samples", FALSE, TRUE},
//  {NULL, NULL, ENUM, "rate-control", FALSE, TRUE},
//  {NULL, NULL, BOOL, "fixed-framerate", FALSE, TRUE},
//  {NULL, NULL, INT, "max-mbps", TRUE, TRUE},
//  {NULL, NULL, INT, "level-idc", FALSE, TRUE},
//  {NULL, NULL, INT, "peak-bitrate", FALSE, TRUE},
//  {NULL, NULL, INT, "average-bitrate", FALSE, TRUE},
//  {NULL, NULL, INT, "min-iframe-qp", FALSE, TRUE},
//  {NULL, NULL, INT, "max-iframe-qp", FALSE, TRUE},
//  {NULL, NULL, INT, "min-pframe-qp", FALSE, TRUE},
//  {NULL, NULL, INT, "max-pframe-qp", FALSE, TRUE},
//  {NULL, NULL, INT, "min-bframe-qp", FALSE, TRUE},
//  {NULL, NULL, INT, "max-bframe-qp", FALSE, TRUE},
//  {NULL, NULL, INT, "ltr-buffer-size", FALSE, TRUE},
//  {NULL, NULL, INT, "ltr-encoder-control", FALSE, TRUE},
//};


/* Functions below print the Capabilities in a human-friendly format */
static gboolean print_field (GQuark field, const GValue * value, gpointer pfx) {
  gchar *str = gst_value_serialize (value);

  g_print ("%s  %15s: %s\n", (gchar *) pfx, g_quark_to_string (field), str);
  g_free (str);
  return TRUE;
}

static void print_caps (const GstCaps * caps, const gchar * pfx) {
  guint i;

  g_return_if_fail (caps != NULL);

  if (gst_caps_is_any (caps)) {
    g_print ("%sANY\n", pfx);
    return;
  }
  if (gst_caps_is_empty (caps)) {
    g_print ("%sEMPTY\n", pfx);
    return;
  }

  for (i = 0; i < gst_caps_get_size (caps); i++) {
    GstStructure *structure = gst_caps_get_structure (caps, i);

    g_print ("%s%s\n", pfx, gst_structure_get_name (structure));
    gst_structure_foreach (structure, print_field, (gpointer) pfx);
  }
}

/* Prints information about a Pad Template, including its Capabilities */
static void print_pad_templates_information (GstElementFactory * factory) {
  const GList *pads;
  GstStaticPadTemplate *padtemplate;

  g_print ("Pad Templates for %s:\n", gst_element_factory_get_longname (factory));
  if (!factory->numpadtemplates) {
    g_print ("  none\n");
    return;
  }

  pads = factory->staticpadtemplates;
  while (pads) {
    padtemplate = (GstStaticPadTemplate *) (pads->data);
    pads = g_list_next (pads);

    if (padtemplate->direction == GST_PAD_SRC)
      g_print ("  SRC template: '%s'\n", padtemplate->name_template);
    else if (padtemplate->direction == GST_PAD_SINK)
      g_print ("  SINK template: '%s'\n", padtemplate->name_template);
    else
      g_print ("  UNKNOWN!!! template: '%s'\n", padtemplate->name_template);

    if (padtemplate->presence == GST_PAD_ALWAYS)
      g_print ("    Availability: Always\n");
    else if (padtemplate->presence == GST_PAD_SOMETIMES)
      g_print ("    Availability: Sometimes\n");
    else if (padtemplate->presence == GST_PAD_REQUEST) {
      g_print ("    Availability: On request\n");
    } else
      g_print ("    Availability: UNKNOWN!!!\n");

    if (padtemplate->static_caps.string) {
      g_print ("    Capabilities:\n");
      print_caps (gst_static_caps_get (&padtemplate->static_caps), "      ");
    }

    g_print ("\n");
  }
}

/* Shows the CURRENT capabilities of the requested pad in the given element */
static void print_pad_capabilities (GstElement *element, gchar *pad_name) {
  GstPad *pad = NULL;
  GstCaps *caps = NULL;

  /* Retrieve pad */
  pad = gst_element_get_static_pad (element, pad_name);
  if (!pad) {
    g_printerr ("Could not retrieve pad '%s'\n", pad_name);
    return;
  }

  /* Retrieve negotiated caps (or acceptable caps if negotiation is not finished yet) */
  caps = gst_pad_get_negotiated_caps (pad);
  if (!caps)
    caps = gst_pad_get_caps_reffed (pad);

  /* Print and free */
  g_print ("Caps for the %s pad:\n", pad_name);
  print_caps (caps, "      ");
  gst_caps_unref (caps);
  gst_object_unref (pad);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    /////////// GSTREAMER PART /////////////

    GstBus *bus;
    GstMessage *msg;

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    /////////////// END test /////////////////////

    Pipeline pipeline = Pipeline();

//    pipeline.bin = gst_parse_launch ("uvch264_src device=/dev/video1 auto-start=true name=src initial-bitrate=6000000 average-bitrate=6000000 rate-control=cbr"
//                                     "src.vidsrc ! queue ! "
//                                     "capsfilter name=vid_cf ! identity name=identity ! tee name=t ! "
//                                     "queue name=queue_0 ! mp4mux ! filesink name=file_sink0 location=/home/peter/tmp/test0.mp4 "
//                                     "t. ! queue name=queue_1 ! mp4mux ! filesink name=file_sink1 location=/home/peter/tmp/test1.mp4 "
//                                     "src.vfsrc ! queue name=queue_preview ! capsfilter name=vf_cf ! "
//                                     "xvimagesink name=preview_sink async=false", NULL);


//    pipeline.src = gst_bin_get_by_name (GST_BIN (pipeline.bin), "src");
//    pipeline.identity = gst_bin_get_by_name (GST_BIN (pipeline.bin), "identity");
//    pipeline.vid_capsfilter = gst_bin_get_by_name (GST_BIN (pipeline.bin), "vid_cf");
//    pipeline.vf_capsfilter = gst_bin_get_by_name (GST_BIN (pipeline.bin), "vf_cf");
//    pipeline.t = gst_bin_get_by_name (GST_BIN (pipeline.bin), "t");
//    pipeline.file_sink0 = gst_bin_get_by_name (GST_BIN (pipeline.bin), "file_sink0");
//    pipeline.file_sink1 = gst_bin_get_by_name (GST_BIN (pipeline.bin), "file_sink1");
//    pipeline.queue_0 = gst_bin_get_by_name (GST_BIN (pipeline.bin), "queue_0");
//    pipeline.queue_1 = gst_bin_get_by_name (GST_BIN (pipeline.bin), "queue_1");
//    pipeline.queue_preview = gst_bin_get_by_name (GST_BIN (pipeline.bin), "queue_preview");
//    pipeline.preview_sink = gst_bin_get_by_name (GST_BIN (pipeline.bin), "preview_sink");

    pipeline.src = gst_element_factory_make("uvch264_src", "src");
    pipeline.queue_src = gst_element_factory_make("queue", "queue_src");
    pipeline.identity = gst_element_factory_make("identity", "identity");
    pipeline.vid_capsfilter = gst_element_factory_make ("capsfilter", "vid_cf");
    pipeline.vf_capsfilter = gst_element_factory_make ("capsfilter", "vf_cf");
    pipeline.t = gst_element_factory_make ("tee", "t");
    pipeline.file_sink0 = gst_element_factory_make ("filesink", "file_sink0");
    pipeline.file_sink1 = gst_element_factory_make ("filesink", "file_sink1");
    pipeline.queue_0 = gst_element_factory_make ("queue", "queue_0");
    pipeline.queue_1 = gst_element_factory_make ("queue", "queue_1");
    pipeline.queue_preview = gst_element_factory_make ("queue", "queue_preview");
    pipeline.preview_sink = gst_element_factory_make ("xvimagesink", "preview_sink");
    pipeline.mp4mux0 = gst_element_factory_make("mp4mux", "mp4mux0"); //ffmux_mp4
    pipeline.mp4mux1 = gst_element_factory_make("mp4mux", "mp4mux1");

    /* Create the empty pipeline */
    pipeline.bin = gst_pipeline_new ("test-pipeline");


    if (    !pipeline.bin || !pipeline.src || !pipeline.queue_src || !pipeline.identity ||
            !pipeline.file_sink0 || !pipeline.file_sink1 || !pipeline.queue_0 ||
            !pipeline.queue_1 || !pipeline.t || !pipeline.vid_capsfilter || !pipeline.vf_capsfilter ||
            !pipeline.preview_sink || !pipeline.queue_preview || !pipeline.mp4mux0 || !pipeline.mp4mux1 ) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
    }

    /* Configure elements */

    // source element
    g_object_set (pipeline.src, "device","/dev/video1", "auto-start", true, "initial-bitrate","6000000", "average-bitrate","6000000", /*"rate-control","cbr",*/ NULL);

    // mp4mux elements
//    g_object_set (pipeline.mp4mux0, "streamable", true, "dts-method",  2, "presentation-time", false, "trak-timescale", 1, NULL);
//    g_object_set (pipeline.mp4mux1, "streamable", true, "dts-method",  2, "presentation-time", false, "trak-timescale", 1, NULL);


//    // set caps
    GstCaps *h264_caps = NULL;
    GstCaps *raw_caps = NULL;

    h264_caps = gst_caps_from_string ("video/x-h264,width=1920,height=1080,framerate=(fraction)30/1");
    raw_caps = gst_caps_from_string ("video/x-raw-yuv,width=320,height=240,framerate=(fraction)30/1");

    g_object_set (pipeline.vid_capsfilter, "caps", h264_caps, NULL);
    g_object_set (pipeline.vf_capsfilter, "caps", raw_caps, NULL);

    // sinks
    g_object_set (pipeline.preview_sink, "async", false, NULL);
    g_object_set (pipeline.file_sink0, "location", "/home/peter/tmp/test0.h264", NULL);
    g_object_set (pipeline.file_sink1, "location", "/home/peter/tmp/test1.h264", NULL);


    /* Associate all elements with the pipeline */
    gst_bin_add_many (GST_BIN (pipeline.bin) , pipeline.src , pipeline.queue_src , pipeline.identity ,
                pipeline.file_sink0 , pipeline.file_sink1 , pipeline.queue_0 ,
                pipeline.queue_1 , pipeline.t , pipeline.vid_capsfilter , pipeline.vf_capsfilter ,
                pipeline.preview_sink , pipeline.queue_preview, pipeline.mp4mux0, pipeline.mp4mux1, NULL);


    /* Manually link the source which has two ambiguous pads */
    GstPad *raw_sink_pad, *h264_sink_pad;
    GstPad *queue_src_pad, *queue_preview_pad;

    raw_sink_pad = gst_element_get_static_pad(pipeline.src, "vfsrc");
    queue_preview_pad = gst_element_get_static_pad (pipeline.queue_preview, "sink");

    h264_sink_pad = gst_element_get_static_pad(pipeline.src, "vidsrc");
    queue_src_pad = gst_element_get_static_pad (pipeline.queue_src, "sink");


    if (    gst_pad_link (raw_sink_pad, queue_preview_pad) != GST_PAD_LINK_OK ||
            gst_pad_link (h264_sink_pad, queue_src_pad) != GST_PAD_LINK_OK) {
        g_printerr ("Src could not be linked.\n");
        gst_object_unref (pipeline.bin);
        return -1;
    }

    /* Manually link the Tee, which has "Request" pads */
    GstPadTemplate *tee_src_pad_template;
    GstPad *tee_0_pad, *tee_1_pad;
    GstPad *queue_0_tee_pad, *queue_1_tee_pad;

    tee_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (pipeline.t), "src%d");

    tee_0_pad = gst_element_request_pad (pipeline.t, tee_src_pad_template, NULL, NULL);
    g_print ("Obtained request pad %s for 0 branch.\n", gst_pad_get_name (tee_0_pad));
    queue_0_tee_pad = gst_element_get_static_pad (pipeline.queue_0, "sink");

    tee_1_pad = gst_element_request_pad (pipeline.t, tee_src_pad_template, NULL, NULL);
    g_print ("Obtained request pad %s for 1 branch.\n", gst_pad_get_name (tee_1_pad));
    queue_1_tee_pad = gst_element_get_static_pad (pipeline.queue_1, "sink");

    if (    gst_pad_link (tee_0_pad, queue_0_tee_pad) != GST_PAD_LINK_OK ||
            gst_pad_link (tee_1_pad, queue_1_tee_pad) != GST_PAD_LINK_OK) {
        g_printerr ("Tee could not be linked.\n");
        gst_object_unref (pipeline.bin);
        return -1;
    }
    gst_object_unref (queue_0_tee_pad);
    gst_object_unref (queue_1_tee_pad);



    /* Link all elements that can be automatically linked because they have "Always" pads */
    if (gst_element_link_many (pipeline.queue_src, pipeline.vid_capsfilter, pipeline.t, NULL) != TRUE ||
        gst_element_link_many (pipeline.queue_0, pipeline.file_sink0, NULL) != TRUE ||
            gst_element_link_many (pipeline.queue_1, pipeline.file_sink1, NULL) != TRUE ||
            gst_element_link_many (pipeline.queue_preview, pipeline.vf_capsfilter, pipeline.preview_sink, NULL) != TRUE) {
      g_printerr ("Elements could not be linked.\n");
      gst_object_unref (pipeline.bin);
      return -1;
    }


    /* Manually link mp4mux, which has "Request" pads */
//    GstPadTemplate *mp4mux0_src_pad_template, *mp4mux1_src_pad_template;
//    GstPad *mp4mux_0_pad, *mp4mux_1_pad;
//    GstPad *queue_0_mp4mux_pad, *queue_1_mp4mux_pad;

//    mp4mux0_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (pipeline.mp4mux0), "video_%d");

//    mp4mux_0_pad = gst_element_request_pad (pipeline.mp4mux0, mp4mux0_src_pad_template, NULL, gst_caps_from_string("video/x-h264,width=1920,height=1080,framerate=30/1"));
//    g_print ("Obtained request mp4mux pad %s for 0 branch.\n", gst_pad_get_name (mp4mux_0_pad));
//    queue_0_mp4mux_pad = gst_element_get_static_pad (pipeline.queue_0, "src");

//    mp4mux1_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (pipeline.mp4mux1), "video_%d");

//    mp4mux_1_pad = gst_element_request_pad (pipeline.mp4mux1, mp4mux1_src_pad_template, NULL, gst_caps_from_string("width=1920,height=1080,framerate=30/1"));
//    g_print ("Obtained request mp4mux pad %s for 1 branch.\n", gst_pad_get_name (mp4mux_1_pad));
//    queue_1_mp4mux_pad = gst_element_get_static_pad (pipeline.queue_1, "src");

//    if (    gst_pad_link (queue_0_mp4mux_pad, mp4mux_0_pad) != GST_PAD_LINK_OK ||
//            gst_pad_link (queue_1_mp4mux_pad, mp4mux_1_pad) != GST_PAD_LINK_OK) {
//        g_printerr ("mp4mux could not be linked.\n");
//        gst_object_unref (pipeline.bin);
//        return -1;
//    }
//    gst_object_unref (mp4mux_0_pad);
//    gst_object_unref (mp4mux_1_pad);


    /* Start playing the pipeline */
//    int ret = gst_element_set_state (pipeline.bin, GST_STATE_READY);

    /* start capturing the data */
//    g_signal_emit_by_name (G_OBJECT (pipeline.src), "start-capture", NULL);

    /* Start playing the pipeline */
     int ret = gst_element_set_state (pipeline.bin, GST_STATE_PLAYING);

    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline.bin);
        gst_object_unref (pipeline.src);
        gst_object_unref (pipeline.identity);
        gst_object_unref (pipeline.file_sink0);
        gst_object_unref (pipeline.file_sink1);
        gst_object_unref (pipeline.t);
        gst_object_unref (pipeline.vid_capsfilter);
        gst_object_unref (pipeline.vf_capsfilter);
        return -1;
      }



    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline.bin), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

    sleep(10);

    ret = gst_element_set_state (pipeline.file_sink1, GST_STATE_NULL);
//    ret = gst_element_set_state (pipeline.queue_1, GST_STATE_NULL);
    g_object_set(pipeline.file_sink1, "location", "/home/peter/tmp/newVid.h264", NULL);
//    ret = gst_element_set_state (pipeline.queue_1, GST_STATE_PLAYING);
    ret = gst_element_set_state (pipeline.file_sink1, GST_STATE_PLAYING);



    /* Wait until error or EOS */
    bus = gst_element_get_bus (pipeline.bin);
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));


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
           g_printerr ("Unexpected message received.\n");
           break;
       }

       gst_message_unref (msg);
     }

    /* Free resources */
    if (msg != NULL)
      gst_message_unref (msg);
    gst_object_unref (bus);

    /* stop capturing the data */
    g_signal_emit_by_name (G_OBJECT (pipeline.src), "stop-capture", NULL);
    gst_element_set_state (pipeline.bin, GST_STATE_NULL);

    gst_object_unref (pipeline.bin);
//    gst_object_unref (pipeline.src);
//    gst_object_unref (pipeline.identity);
//    gst_object_unref (pipeline.file_sink0);
//    gst_object_unref (pipeline.file_sink1);
//    gst_object_unref (pipeline.t);
//    gst_object_unref (pipeline.vid_capsfilter);
//    gst_object_unref (pipeline.vf_capsfilter);
//    gst_object_unref (pipeline.queue_0);
//    gst_object_unref (pipeline.queue_1);
//    gst_object_unref (pipeline.queue_preview);
//    gst_object_unref (pipeline.preview_sink);
//    gst_object_unref (pipeline.mp4mux0);
//    gst_object_unref (pipeline.mp4mux1);
    return 0;


    ///////////////////////////////////////


    
    return a.exec();
}
