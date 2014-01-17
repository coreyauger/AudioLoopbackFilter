AudioLoopbackFilter
===================

Directshow Audio Loopback Capture Filter

## Dependencies
Microsoft SDK (Direct Show base classes)  [http://www.microsoft.com/en-ca/download/details.aspx?id=8279](http://www.microsoft.com/en-ca/download/details.aspx?id=8279) 

## Overview
This project provides an efficient working audio capture source filter.  A “what you hear is what you get”.  The code is based on Microsofts [IAudioCaptureClient](http://msdn.microsoft.com/en-us/library/dd370858.aspx) running in “loopback” mode.  A feature that has been added since Windows Vista.  

## Past Work
This code is based on the initial work done by “Maurits” at Microsoft.  His work and all the comments containing some of the issues with the code are located [here](http://blogs.msdn.com/b/matthew_van_eerde/archive/2008/12/16/sample-wasapi-loopback-capture-record-what-you-hear.aspx).


Additionally “Roger Pack” wrote the original directshow filter wrapper for Maurits code.  Roger’s work can be found here: [https://github.com/rdp/virtual-audio-capture-grabber-device](https://github.com/rdp/virtual-audio-capture-grabber-device).  I found that in Windows 8 the filter no longer worked on this project.  I also submit a patch that fixed the audio in windows 8.. but the filter graph would pause if there was silence (no audio).

The main issue with these works, is the inability to generate “silence”.  This problem is magnified in directshow by not allowing your filter graph to process any video unless there is valid audio (ie silence will pause your graph).

## My Solution
I based my filter off the Synth Audio Source, from the directshow example projects.  I have removed most of the source that was not needed for my filter to make it easier to understand.

There are a few major differences in my filter from the one I was using [before](https://github.com/rdp/virtual-audio-capture-grabber-device).

* I generate silence values into the buffer when there is “No” data available from the AudioCaptureClient.  This will allow the filter graph to continue to process video frames/mux even if there is no audio.

* I sync the Reference clock to use the clock time specified by the AudioCaptureClient.  This eliminates the usage of the extra threading and BAD sleep statements in the code.  The result is a significantly lighter CPU footprint.  

## Finally 
My solution has only undergone a very small amount of testing.  Which did include both a Windows 7 and Windows 8 machine.  However, if you have any trouble with the filter please contact me and I will be glad to help.  Thanks.


[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/coreyauger/audioloopbackfilter/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

