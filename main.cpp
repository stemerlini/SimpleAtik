// ------------------------------------------------------------------------------
// UPDATED VERSION OF SIMPLEATIK 2.0 - Original version wrote by Jack Hare
// Author: Stefano Merlini
// Date: 01/03/2021
// ------------------------------------------------------------------------------

#include "C:\Program Files (x86)\LabJack\Drivers\LabJackUD.h" //64-bit Windows
#include<iostream>
//camera header file
#include "ArtemisHSCAPI.h"
//necessary for camera for some reason
#include "stdafx.h"
//allows us to do string compare
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <stdio.h>
#include <windows.h>
#include <ctime>

#include <iomanip> //allows precision

// include the Thorlab APT library 
#include "Thorlabs.MotionControl.IntegratedStepperMotors.h"
#include <stdlib.h>
#include <conio.h>

// ----------------------------------------------------------------------------------------

using namespace cv;
using namespace std;

void list_available_devices();

void ErrorHandler(LJ_ERROR lngErrorcode, long lngLineNumber, long lngIteration);

string to_string_2c(int i);

string generate_filename(char* serial);

// ----------------------------------------------------------------------------------------


int main(int argc, char** argv)
{
	//load the labjack
	LJ_ERROR lngErrorcode;
	LJ_HANDLE lngHandle = 0;

	//Open ThorLab Found Polirizer

	int serialNo = 55000001;


	//Open the first found LabJack.
	lngErrorcode = OpenLabJack(LJ_dtU3, LJ_ctUSB, "1", 1, &lngHandle);
	ErrorHandler(lngErrorcode, __LINE__, 0);
	lngErrorcode = ePut(lngHandle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0); //reset to factory defaults, FI04 configured as DIN
	ErrorHandler(lngErrorcode, __LINE__, 0);

	if (!ArtemisLoadDLL("ArtemisHSC.dll"))
	{
		cout << "Couldn't Load DLL." << endl;
	}


	cout << "Welcome to Atik MAGPIE. Jack Hare, Dec 2015" << endl;
	ArtemisHandle hCam = ArtemisConnect(0);


	char* serial=new char[10];
	ArtemisDeviceSerial(0, serial);
	cout << "Serial:" << serial << endl;

	int temperature;
	cout << "Set camera temperature ('C): " ;
	cin >> temperature;

	ArtemisSetCooling(hCam, temperature * 100);

	float exposure_s = 1;
	cout << "Set exposure time (s): ";
	cin >> exposure_s;

	while (1)
	{
		long ValueDIPort = 1;//LJ triggers on logical LOW by grounding FI04 to GND
		cout <<endl<< "Waiting for Trigger. Temperature: ";

		// before entering update loop
		HANDLE han = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
		GetConsoleScreenBufferInfo(han, &bufferInfo);

		// update loop
		while (ValueDIPort != 0)
		{
			lngErrorcode = eDI(lngHandle, 4, &ValueDIPort);
			ErrorHandler(lngErrorcode, __LINE__, 0);
			ArtemisTemperatureSensorInfo(hCam, 1, &temperature);

			SetConsoleCursorPosition(han, bufferInfo.dwCursorPosition); //move the cursor to overwrite the temperature
			cout << to_string(temperature / 100) << " 'C   ";//extra spaces cope with -1 etc printing C's far to the right...
		}

		cout << endl << "Start exposure:... ";
		ArtemisStartExposure(hCam, exposure_s); //take the exposure!

		while (ArtemisImageReady(hCam) == 0.0){
			Sleep(10); //we need to wait for the camera to be ready, else we'll get nothing when we download the image
		}

		int w, h, x, y, binx, biny; //we need the image size to reshape the array
		ArtemisGetImageData(hCam, &x, &y, &w, &h, &binx, &biny);
		cout << "Exposure Ready. Width: " << w << "; " << "Height: " << h << endl;
		unsigned short* output_buf = (unsigned short*)ArtemisImageBuffer(hCam);

		Mat image; //create a opencv Matrix object to hold the data
		image = Mat(h, w, CV_16U, output_buf); //CV_16U is a unsigned short (16-bit) OpenCV data type

		if (!image.data)                              // Check for invalid input
		{
			cout << "Could not open or find the image" << endl;
			return -1;
		}

		string filename = generate_filename(serial); //generate a nice filename based on the current time and camera serial
		CvScalar img_sum = sum(image);
		double total = img_sum.val[0];

		cout << fixed;
		cout << setprecision(0);
		cout << "Writing Image: " << filename << ", Image sum: " << total<< "...";
		imwrite(filename, image); //write a 16 bit PNG
		cout << " Image Written!" << endl;
	}

	return 0;
}


//This is a simple function that returns a list of available Thorlab devices

void list_available_devices()
{
	// function to return the list of devices connected to the PC
	int devices = 0


}

//This is our simple error handling function that is called after every UD
//function call.  This function displays the errorcode and string description
//of the error.  It also has a line number input that can be used with the
//macro __LINE__ to display the line number in source code that called the
//error handler.  It also has an iteration input is useful when processing
//results in a loop (getfirst/getnext).
void ErrorHandler(LJ_ERROR lngErrorcode, long lngLineNumber, long lngIteration)
{
	char err[255];

	if (lngErrorcode != LJE_NOERROR)
	{
		ErrorToString(lngErrorcode, err);
		printf("Error number = %d\n", lngErrorcode);
		printf("Error string = %s\n", err);
		printf("Source line number = %d\n", lngLineNumber);
		printf("Iteration = %d\n\n", lngIteration);
		if (lngErrorcode > LJE_MIN_GROUP_ERROR)
		{
			//Quit if this is a group error.
			getchar();
			exit(0);
		}
	}
}


//Makes an integer into a two character string. Works for our cases.
string to_string_2c(int i)
{
	if (i < 10){
		return "0" + to_string(i);
	}
	else{
		return to_string(i);
	}
}


string generate_filename(char* serial){
	time_t currentTime;
	struct tm *localTime;

	time(&currentTime);                   // Get the current time
	localTime = localtime(&currentTime);  // Convert the current time to the local time

	int Day = localTime->tm_mday;
	int Month = localTime->tm_mon + 1;
	int Year = localTime->tm_year - 100; //year is years since 1900, but we will never be <year 2000 these days...
	int Hour = localTime->tm_hour;
	int Min = localTime->tm_min;
	int Sec = localTime->tm_sec;

	string filename = "s" + to_string_2c(Month) + to_string_2c(Day) + "_" + to_string_2c(Year) + "_" + to_string_2c(Hour) + to_string_2c(Min) + to_string_2c(Sec) + "_c"+serial[0]+ ".png";
	return filename;
}

