#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include<iostream>
#include<sstream> 
//The necessary libraries to be used throughout the project

const int minArea = 200; //Defines the minimun area needed for a detected contour to be read as valid data.

const int imageWidth = 20;
const int imageHeight = 30; //Values to be used to resize a region of interest

using namespace std;

using namespace cv; //Pleb names spaces Not using HEX namespace yet needs to be combined


class ContourData{
	public:
		vector<Point> ptContour;
		Rect boundingRect;
		float area;
		

		bool validContour(){
			if (area < minArea){
			       	return false;
			}
			return true;
		}//Checks the validity of the read contour by counting the region area.
	static bool sortContours(const ContourData& cwdLeft, const ContourData& cwdRight){
		return(cwdLeft.boundingRect.x < cwdRight.boundingRect.x); //Sorts all read contours by there respective positions. Remnants of a dead code as 
		//A proper filter method hasn't been applied yet.
	}
};//Class to store necessary contour data to be compared with the XML files later on.

int main(){
	vector<ContourData> allData;
	vector<ContourData> validData;//Declares to variables that fall under the class
	cout<<"Start of the program\n";
	Mat classification;//Defines a matrix 
	FileStorage fsClassifications("group_ints.xml", FileStorage::READ);
	//Reads in the data from group_ints
	if (fsClassifications.isOpened() == false){
		cout<<"Error, can't locate groups_ints.xml\n";
		return (0);
	}//If empty display error message and exit

	fsClassifications["groups"] >> classification;
	fsClassifications.release();//Stores the values of group_ints.xml into classification

	Mat trainingImage;
	FileStorage fsImages("image.xml", FileStorage::READ);//Reads in image.xml

	if (fsImages.isOpened() == false){
		cout<<"Error, can't locate image.xml\n";
		return (0);//Same error message Ahmed :/
	}

	fsImages["images"] >> trainingImage;//Stores the values into the matrix trainingImage
	fsImages.release();

	cv::Ptr<cv::ml::KNearest> kNearest(cv::ml::KNearest::create());//Establishes the function to train a KNN model

	kNearest->cv::ml::StatModel::train(trainingImage, cv::ml::ROW_SAMPLE, classification); //Trains a model based on the values of classification & trainingImage.
	
	Mat matTestImage = imread("test_image.bmp"); //Reads in a test_image

	if (matTestImage.empty()){
		cout<<"Error, test_image.png not found";
		return (0);//If empty return error
	
	}
	Mat ThreshCopy;
	Mat Blurred;	
	Mat Grayscale;
	Mat Thresh;//Establishes more bloody matrices.
	
	cvtColor(matTestImage, Grayscale, COLOR_RGB2GRAY);//Converts the read image to grey scale
	
	GaussianBlur(Grayscale,
        	Blurred,
        	Size(5, 5),
        	0);

                                  
	adaptiveThreshold(Blurred,
        	Thresh,
        	255,
        	ADAPTIVE_THRESH_GAUSSIAN_C,       
        	THRESH_BINARY_INV,               
	 	11,
		2);//Applies filters of the original image and then produces a new one. Ditto for the next filter.

	ThreshCopy = Thresh.clone();//Creates a copy of the image as findcontours overwrites the original image.
	
	
	vector<vector<Point>> ptContours;
	vector<Vec4i> v4iHierarchy;//Creates vectors to store information from the thresh copy see below.
	
	findContours(ThreshCopy,
			ptContours,
			v4iHierarchy,
			RETR_EXTERNAL,
			CHAIN_APPROX_SIMPLE);//Stores the location of the contours as a simplified four end point region and stores the needed data in the hierarchy and ptContours
	
	
	for (int i = 0; i < ptContours.size(); i++){
		ContourData contourData;
		contourData.ptContour = ptContours[i];
		contourData.boundingRect = boundingRect(contourData.ptContour);
		contourData.area = contourArea(contourData.ptContour);
		allData.push_back(contourData);//For each contour check of it's size is valid and then create a bounded rectangle around the region. 
	}

	for (int i=0; i < allData.size(); i++){
		if (allData[i].validContour()){
			validData.push_back(allData[i]);//Checks if each pushed back value was valid corresponding to the class' function and if it works push it back into the the validData area.
		}
	}

	sort(validData.begin(), validData.end(), ContourData::sortContours);//Sorts out the contours.

	string ReadCharacters;//A string to store the read in results

	for (int i = 0; i < validData.size(); i++){//For each vaild data value
		rectangle(matTestImage,
				validData[i].boundingRect,
				Scalar(0,255,0),
				2);
		Mat matROI = Thresh(validData[i].boundingRect);
		Mat ROIResized;
		resize(matROI, ROIResized, Size(imageWidth, imageHeight));//Create a region of interest around the highlighted contours

		Mat ROIFloat;
		ROIResized.convertTo(ROIFloat, CV_32FC1);//Convert the ROI to a float
		imshow("MatRoiResized",ROIResized);
		Mat matCurrentChar(0,0, CV_32F);

		kNearest->findNearest(ROIFloat.reshape(1,1), 3, matCurrentChar);//Use the KNN to try and compare the read in image to the gene
			
		float fltCurrentChar  = (float)matCurrentChar.at<float>(0,0);//Stores the result as a char

		ReadCharacters = ReadCharacters + char(int(fltCurrentChar));//Appends the array with the resut
	}
	cout <<"\n"<<ReadCharacters<<"\n";//Prints out the result
	imshow("Initial_image", matTestImage);//Displays the image

	waitKey(0);
	return(0);

}

