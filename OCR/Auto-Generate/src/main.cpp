#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>
#include<iostream>
#include<vector> //Inclusion of the necessary libraries

const int min_area = 200;
const int image_width = 20;
const int image_height = 30; //Defined constants 

using namespace cv;
using namespace std; //Using namespace cv/std will be merged with Zalan's Hex later on

int main(){
	Mat imgRGB;
	Mat imgGray;
	Mat imgBlurring;
	Mat imgBW;
	Mat imgBWCopy;

	Mat GroupInts;
	Mat FlattenedFloats;//Definition of the needed Matrices
	
	
	vector<vector<Point>> ptContours; 
	vector<Vec4i> v4iHierarchy; 
	vector <String> fileList;//Definition of vectors. Could be moved out of main. Just made them as I went.

	string characterArray("ABCDEFGHIJKLMNOPQRSTUVWXYZ");//The alphabet needed to assign values to read in images
	string orientation("0123");//Based on my peers file storage I had to accomadate for their file names.
	string Directory("/home/jgbroz/Documents/Project-Hex/OCR/Auto-Generate/A/0/*.bmp");//The directory of the saved images. MUST BE ALTERED IF RAN ON A DIFFERENT SYSTEM.

	for (int i = 0; i <= 25; i++){//For i less than twenty0five )size of alphabet)
		Directory.replace(53, 1, characterArray, i, 1);//Replace value fifty-three in the directory with a character corresponding to i from the alphabet written above.
		cout<<Directory<<"\n";//Prints out the current directory to terminal
		for (int j = 0; j <= 3; j++){//Cycles through each of the orientations (zero-three)
			Directory.replace(55, 1, orientation, j, 1); //Changes the directory further based on orientation 
			glob(Directory, fileList, false);//Reads in a list of all the files in the directory and stores it in an array
			for (size_t k = 0; k < fileList.size(); ++k){//Runs a for loop equal to the amount of files in the directory
				imgRGB = imread(fileList[k]);//Reads in the first image

				if (imgRGB.empty()){
					cout<<"Image not in directory\n";
					return(0);
				}//Returns error code if the image isn't found.

				cvtColor(imgRGB,imgGray,COLOR_RGB2GRAY);

				GaussianBlur(imgGray,
						imgBlurring,
						Size(5,5),
						0);

				adaptiveThreshold(imgBlurring,
						imgBW,
						255,
						ADAPTIVE_THRESH_GAUSSIAN_C, 
						THRESH_BINARY_INV,
						11,
						2);

				imgBWCopy = imgBW.clone();
				
				imshow("imgBW",imgBW);
				imshow("Blurring",imgBlurring);

				findContours(imgBWCopy,
						ptContours,
						v4iHierarchy,
						RETR_EXTERNAL,
						CHAIN_APPROX_SIMPLE);//Applies filters to the image to make it more readable.

				for (int l = 0; l < ptContours.size(); l++){//For the number of contour sets read in repeat the process. Ideally should be one but left room for multiple characters found.
					if (contourArea(ptContours[l]) > min_area){//Check if the contour exceeds the defined minimun region.
						Rect rectangleChar = boundingRect(ptContours[l]);//Place a rectangle on the contours end points.
						rectangle(imgRGB, rectangleChar, Scalar(255,0,0),2);//Draw the rectangle over the image RGB not necessary as user feedback is no longer limited
						//Add an imshow for RGB and a waitKey(0) to see how the images are being interpretted. 

						Mat matROI = imgBW(rectangleChar);
						Mat matROIResized;//Create a region of interest based on the highlighted region
						resize(matROI, matROIResized, Size(image_width, image_height));//Resize the ROI based on the constants to be uniformly stored.

						
						imshow("matROIResized", matROIResized);//Shows the final result to be stored. Again add a waitKey(0) at the end to really see it.
						
						cout<<"valid input appending the xml files."<<characterArray[i]<<"\n";//Display what character is currently being appended to the xml file
						
						GroupInts.push_back(int(characterArray[i]));//Push the current value into a vector array 

						Mat matImageFloat;

						matROIResized.convertTo(matImageFloat, CV_32FC1);//Convert the Resized image to float values using OpenCV 32FC1. Needed for the KNN inputs

						Mat FlatFloats = matImageFloat.reshape(1,1);//Must be converted further to flat floats

						FlattenedFloats.push_back(FlatFloats);//Append the vector array with the new values of the xml file 
						}
					}
				}
			}	
		}
	
	cout<<"Training Completed\n";//State the training is complete

	FileStorage fsClassifications("group_ints.xml", FileStorage::WRITE);//Writes the new values to an xml file for the corresponding characters
	if (fsClassifications.isOpened() == false){
		cout<<"Unable to open group_ints\n";
		return(0);
	}

	fsClassifications<<"groups"<<GroupInts;
	fsClassifications.release();//Writes them in and releases the files

	FileStorage fsTrainingImage("image.xml",FileStorage::WRITE);//Same process
	if (fsTrainingImage.isOpened() == false){
		cout<<"Unable to open image\n";
		return(0);
	}
	fsTrainingImage<<"images"<<FlattenedFloats;//Write and release the xml file broken down to a pixel by pixel basis with an area of about 600. From my memory may be incorrect area.
	fsTrainingImage.release();
	return(0);
}
		
		
			


