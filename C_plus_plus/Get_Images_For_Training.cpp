/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace cv;
using namespace std;

/* Min and max face sizes used to speed up face detection */
int min_face_size=150;
int max_face_size=300;

/* Height and width of the output image. I chose 150 x 150 because it allows for a good amount of   *
 * detail to remain in the image without causing the face recognition training to take a long time  *
 * to complete.																				        */
int height = 75;
int width = 75;

/* Path to training video to grab frame from without the file type appended. */
/* Changed this based on the person being trained for.						 */
string path[] = {"G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\chia\\chia",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\danny\\danny",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\fuji\\fuji",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\harsh\\harsh",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\hector\\hector",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\hide\\hide",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\james\\james",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\jeff\\jeff",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\joey\\joey",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\leekc\\leekc",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\louis\\louis",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\miho\\miho",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\ming\\ming",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\mushiake\\mushiake",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\rakesh\\rakesh",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\saito\\saito",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\victor\\victor",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\wei\\wei",
				 "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\yokoyama\\yokoyama"};


int main(int argc, const char *argv[]) 
{
	//create loop to go through all people training images are needed for.
	for(unsigned int a = 0; a < sizeof(path)/sizeof(path[0]); a = a + 1 )
	{
		int j = 0;
		
		// Get the path to your CSV:
		std::string fn_haar = "C:\\OpenCV new\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt2.xml";
    
		// These vectors hold the images and corresponding labels:
		CascadeClassifier haar_cascade;
		haar_cascade.load(fn_haar);

	    // Get a handle to the Video device:
		 VideoCapture cap(path[a] + ".avi");

		// Check if we can use this device at all:
		if(!cap.isOpened())
		{
			cerr << "Capture Device ID cannot be opened." << endl;
			return -1;
		}

		// Holds the current frame from the video device:
		Mat frame;

		for(;;) 
		{
			cap >> frame;
			if(frame.empty())
			{
				break;
			}

			// Clone the current frame:
			Mat original = frame.clone();

			// Convert the current frame to grayscale:
			Mat gray;
			cvtColor(original, gray, CV_BGR2GRAY);

	        // Find the faces in the frame:
		    vector< Rect_<int> > faces;

			//Changed the face detection instruction to be more specific, which speeds up the process dramatically.
			haar_cascade.detectMultiScale(gray, faces, 1.2, 2, 0|CV_HAAR_SCALE_IMAGE, Size(min_face_size, min_face_size),Size(max_face_size, max_face_size) );

		    // At this point you have the position of the faces in
			// faces. Now we'll get the faces, make a prediction and
			// annotate it in the video. Cool or what?
			for(int i = 0; i < faces.size(); i++)
			{
				// Process face by face:
				Rect face_i = faces[i];
				
				// Crop the face from the image.
				Mat face = gray(face_i);

       			//create the output file name for the image.
				string output = path[a] + std::to_string(j)+".jpg";
				cout << output<<endl;

				//Resize the image to the specified image height and width.
				Mat face_resized;
				cv::resize(face, face_resized, Size(width, height), 1.0, 1.0, INTER_CUBIC);

				// Show the result:
				imshow("resized_face", face_resized);
				// And display it:
				char key = (char) waitKey(20);
				imwrite(output, face_resized);

				//Increase the image number counter.
				j++;

				// First of all draw a green rectangle around the detected face:
				rectangle(original, face_i, CV_RGB(0, 255,0), 1);
			}

			/* This display just shows the input video stream with the detected face marked with a green box */
			// Show the result:
			imshow("face_recognizer", original);
			// And display it:
			char key = (char) waitKey(20);
			// Exit this loop on escape:
			if(key == 27)
				break;
		}
	}
    return 0;
}