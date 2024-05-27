# SimpleAtik v1.0

## SimpleAtik is the original CLI script to trigger and control Atik cameras for Faraday Rotation Imaging Diagnostic at MAGPIE - Jack Hare, Dec 2015

## This is a deprecated software - replaced by pyAtik v1.0

### Running the software
To run the software (SimpelAtik.exe), you will need:

Drivers for a Labjack U3-HV, available from Labjack 
Drivers for Atik 383L+ (Other Atik cameras might work)
Libraries for Artemis CCD: ArtemisHsc.dll 
The previous two currently come bundled as SetupArtemisUnviersal, available from Atik cameras. 
You only need to install drivers and Software Developer kit, not Dawn or anything elseop

Install the Labjack and Atik software before plugging in the labjack or camera. Only plug in one camera and one labjack/

Place SimpleAtik.exe inside a folder. Double click to start. It will load and connect to the labjack and Atik camera.
-The serial number of the camera will appear in the window.
-The code will prompt you to enter a temperature in Celsius for the camera. 1'C works well.
-The code will prompt you to enter an exposure time in seconds. We usually use 1 second.
-The code will say 'Waiting for trigger'. The temperature shown is continuously updated. 
-The camera will take some time to equilibriate at the temperature you have selected.
-When terminal FI04 on the labjack is grounded (eg. by the SLR trigger box), the camera will begin an exposure.
-The download time for a full frame image is around 10 seconds, so be patient!
-The file is automatically saved to the same directory as SimpleAtik.exe with a filename that consists of a timestamp
followed by 'cN' where N is the first digit of the camera's serial number. In our case, our cameras have serial numbers 
beginning with 1 and 2, which is convienient. If new Atiks are bought, the serial numbers will be different, and this might
not be as useful.
-The code will then wait for a trigger again. This loop repeats until the program is closed by clicking the x in the top right.

If you need to change the target temperature or exposure time, exit the program and re run it.
Delete unwanted photos at the end of a shot day and empty the recycle bin frequently.

### Building the software

This software was built using Visual Studio 2013 Community edition and OpenCV 3.0. 
Note that the Artemis DLL is 32 bit, so you need to use the 32 bit version of OpenCV and target a 32 bit build in VS13
You'll need to install OpenCV (I did so using binaries) and edit the path as instructed: [here](http://docs.opencv.org/3.0-beta/doc/tutorials/introduction/windows_install/windows_install.html)
You can get the labjack headers [here](https://labjack.com/support/software/examples/ud/dev-c) or use the ones provided in the Solution file.

Header files (all in same directory as main.cpp):
ArtemisHSCAPI.h Atik camera
LJUD_DynamicLinking.h labjack
StdAfx.h needed for Atik

Source files (all in same directory as main.cpp):
ArtemisHSCAPI.cpp Atik camera
main.cpp body of code
StdAfx.cpp needed for Atik

### Configuration properties:

General:
Character set: Multi-byte character set

C/C++>General>Additional Include Directories:
$(ProgramFiles)\LabJack\Drivers;$(OPENCV_DIR)\..\..\include;%(AdditionalIncludeDirectories)

Linker>General>Additional Library Directories:
$(OPENCV_DIR)\staticlib;$(ProgramFiles)\LabJack\Drivers
(NB: Must have added OPENCV_DIR to the path as per Open CV installation instructions)

Linker>Input>Additional Dependencies:
opencv_features2d300d.lib
opencv_flann300d.lib
opencv_hal300d.lib
opencv_highgui300d.lib
opencv_imgcodecs300d.lib
opencv_imgproc300d.lib
opencv_ml300d.lib
opencv_objdetect300d.lib
opencv_photo300d.lib
opencv_shape300d.lib
opencv_stitching300d.lib
opencv_superres300d.lib
opencv_ts300d.lib
opencv_video300d.lib
opencv_videostab300d.lib
libtiffd.lib
libpngd.lib
libjpegd.lib
libjasperd.lib
IlmImfd.lib
libwebpd.lib
ippicvmt.lib
zlibd.lib
comctl32.lib
vfw32.lib
