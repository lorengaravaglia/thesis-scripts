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
#include <ctime>
#include <iomanip>

using namespace cv;
using namespace std;

int correctPredictions = 0;
int totalPredictions = 0;

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

static void show_image(Mat& img)
{
    // Show the result:
    imshow("face_recognizer", img); //used to be original
    // And display it:
    char key = (char) waitKey(20);
}

int main(int argc, const char *argv[]) {

	// Min and max face sizes used to speed up face detection.
	int min_face_size=150;
	int max_face_size=300;

    // Image height and width for the resized image used in prediction.
    // These values match the dimensions of the training images to improve
    // recognition accuracy.
    int im_width =	75;	//images[0].cols;
    int im_height =	75;	//images[0].rows;

    // Get the path to your CSV:
    string fn_haar = "C:\\OpenCV new\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml";
    string fn_csv = "G:\\Masters_Thesis_Files\\Honda1.csv"; 
    
    // These vectors hold the images and corresponding labels:
    vector<Mat> images;
    vector<int> labels;

    // Read in the data (fails if no valid input filename is given, but you'll get an error message):
    try 
	{
        read_csv(fn_csv, images, labels);
    } 
	catch (cv::Exception& e) 
	{
        cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
        exit(1);
    }

    // Create a FaceRecognizer and train it on the given images:
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
	
    cout<<"training"<<endl;
    model->train(images, labels);
	    
	// That's it for learning the Face Recognition model. You now
    // need to create the classifier for the task of Face Detection.
    // We are going to use the haar cascade you have specified earlier.
    
    CascadeClassifier haar_cascade;
    haar_cascade.load(fn_haar);
   
	// Get a handle to the Video device:
    VideoCapture cap("test.sdp");

    // Check if we can use this device at all:
    if(!cap.isOpened()) 
	{
        cerr << "Capture cannot be opened." << endl;
        return -1;
    }

    // Holds the current frame from the Video device:
    Mat frame;
    
	for(;;) 
	{
        cap >> frame;
        // Clone the current frame:
        Mat original = frame.clone();

        // Convert the current frame to grayscale:
        Mat gray;
        cvtColor(original, gray, CV_BGR2GRAY);

        // Find the faces in the frame:
        vector< Rect_<int> > faces;
        
        // Detect the faces in the image.
        haar_cascade.detectMultiScale(gray, faces, 1.2, 2, 0|CV_HAAR_SCALE_IMAGE, Size(min_face_size, min_face_size),Size(max_face_size, max_face_size) );
		
        // At this point you have the position of the faces in
        // faces. Now we'll get the faces, make a prediction and
        // annotate it in the video. Cool or what?
        for(int i = 0; i < faces.size(); i++) 
		{
            // Process face by face:
            Rect face_i = faces[i];
            // Crop the face from the image. So simple with OpenCV C++:

            Mat face = gray(face_i);

            // Resizing the face is necessary for Eigenfaces and Fisherfaces. You can easily
            // verify this, by reading through the face recognition tutorial coming with OpenCV.
            // Resizing IS NOT NEEDED for Local Binary Patterns Histograms, so preparing the
            // input data really depends on the algorithm used.
            //
            // I strongly encourage you to play around with the algorithms. See which work best
            // in your scenario, LBPH should always be a contender for robust face recognition.
            //
            // Since I am showing the Fisherfaces algorithm here, I also show how to resize the
            // face you have just found:
            Mat face_resized;

            cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);

            // Now perform the prediction, see how easy that is:
            int prediction = model->predict(face_resized);  // was face_resized
            if(prediction == 0)
            {
                correctPredictions++;
            }
            totalPredictions++;

            cout<<"Prediction = "<<prediction<<endl;
            cout<<"Percentage correct = "<<((double)correctPredictions/(double)totalPredictions)*100.0<<"%"<<endl;

            // And finally write all we've found out to the original image!
            // First of all draw a green rectangle around the detected face:
            //rectangle(original, face_i, CV_RGB(0, 255,0), 1);
            // Create the text we will annotate the box with:
            //string box_text = format("Prediction = %d", prediction);
            // Calculate the position for annotated text (make sure we don't
            // put illegal values in there):
            //int pos_x = std::max(face_i.tl().x - 10, 0);
            //int pos_y = std::max(face_i.tl().y - 10, 0);
            // And now put it into the image:
            //putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0), 2);  //thickness was 2.0, changed to int -> 2
        }
		show_image(gray);
    }
    return 0;
}