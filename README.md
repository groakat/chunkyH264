chunkyH264
==========
Example of gstreamer 0.10 setup that uses uvch264_src to save am on-camera-encoded H264 stream to disk.
It consists of two pipelines which are swapped every minute to chunk the stream in small parts. This allows to record infinite video sequences in a manageable format. 

For further reference about the design, visit my blog post:

http://groakat.wordpress.com/2012/12/05/gstreamer-stream-h264-webcam-data-to-series-of-files/

WARNING
At the moment there is a very naughty memory leak appearing as soon as the pipelines swapped the first time. I am trying to find out where it is. Any help would be appreciated. It seems as I am not removing the oldBinRec properly from the mainpipeline, as I just recently got the following warnings when I closed the program:

(uvhCapture:7798): GStreamer-CRITICAL **: 
Trying to dispose object "rec1 bin", but it still has a parent "vidsrc pipeline".
You need to let the parent manage the object instead of unreffing the object directly.


(uvhCapture:7798): GStreamer-CRITICAL **: 
Trying to dispose object "rec2 bin", but it still has a parent "vidsrc pipeline".
You need to let the parent manage the object instead of unreffing the object directly.


(uvhCapture:7798): GStreamer-CRITICAL **: 
Trying to dispose object "rec1 bin", but it still has a parent "vidsrc pipeline".
You need to let the parent manage the object instead of unreffing the object directly.
