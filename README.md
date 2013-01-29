chunkyH264
==========
Example of gstreamer 0.10 setup that uses uvch264_src to save am on-camera-encoded H264 stream to disk.
It consists of two pipelines which are swapped every minute to chunk the stream in small parts. This allows to record infinite video sequences in a manageable format. 

For further reference about the design, visit my blog post:

http://groakat.wordpress.com/2012/12/05/gstreamer-stream-h264-webcam-data-to-series-of-files/

Commandline interface
---------------------
If you want to execute the program from the command line, there is a very basic interface available. However, it will start the gui anyway.
<pre>
--baseDir       path to directory the videos will be stored. The program creates a folder for each
                day and subfolders for each hour
--device        your video device
--directStart   if true the program will start capturing instantly, otherwise it will just set the
                options and show the gui waiting for further interactions (the GUI is not updated yet
                with input arguments)

example:
./uvhCapture --baseDir=/home/peter/vid --device=/dev/video1 --directStart=true
</pre>


Setup of .pro file
------------------
As I am linking to custom build gstreamer 0.10 libraries, you will have to adjust the include path in the .pro file

Relay stuff
-----------
There is some relay class that manages a hardware relay via network. Do not worry about that. The software will run anyway.


WARNING
-------
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
