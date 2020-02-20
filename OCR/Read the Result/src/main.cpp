#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include<iostream>
#include<sstream>

const int minArea = 100;

const int imageWidth = 20;
const int imageHeight = 30;

using namespace std;

using namespace cv;


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
		}
	static bool sortContours(const ContourData& cwdLeft, const ContourData& cwdRight){
		return(cwdLeft.boundingRect.x < cwdRight.boundingRect.x);
	}
};

int main(){
	vector<ContourData> allData;
	vector<ContourData> validData;
	cout<<"Start of the program\n";
	Mat classification;
	FileStorage fsClassifications("group_ints.xml", FileStorage::READ);

	if (fsClassifications.isOpened() == false){
		cout<<"Error, can't locate groups_ints.xml\n";
		return (0);
	}

	fsClassifications["groups"] >> classification;
	fsClassifications.release();

	cout <<"The matrix of classifications is" <<classification<<"\n";

	Mat trainingImage;
	FileStorage fsImages("image.xml", FileStorage::READ);

	if (fsImages.isOpened() == false){
		cout<<"Error, can't locate image.xml\n";
		return (0);
	}

	fsImages["images"] >> trainingImage;
	fsImages.release();

	cv::Ptr<cv::ml::KNearest> kNearest(cv::ml::KNearest::create());

	kNearest->cv::ml::StatModel::train(trainingImage, cv::ml::ROW_SAMPLE, classification);

	//Note at this point the training and generated data is complete. We would now upload the filtered image and test it. However for the sake of learning I tried some of my own img processing based off
	//the last example
	
	Mat matTestImage = imread("test_image.bmp");

	if (matTestImage.empty()){
		cout<<"Error, test_image.png not found";
		return (0);
	
	}
	Mat ThreshCopy;
	Mat Blurred;	
	Mat Grayscale;
	Mat Thresh;
	
	cvtColor(matTestImage, Grayscale, COLOR_RGB2GRAY);
	
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
		2);

	ThreshCopy = Thresh.clone();
	
	
	vector<vector<Point>> ptContours;
	vector<Vec4i> v4iHierarchy;
	
	findContours(ThreshCopy,
			ptContours,
			v4iHierarchy,
			RETR_EXTERNAL,
			CHAIN_APPROX_SIMPLE);
	
	
	for (int i = 0; i < ptContours.size(); i++){
		ContourData contourData;
		contourData.ptContour = ptContours[i];
		contourData.boundingRect = boundingRect(contourData.ptContour);
		contourData.area = contourArea(contourData.ptContour);
		allData.push_back(contourData);
	}

	for (int i=0; i < allData.size(); i++){
		if (allData[i].validContour()){
			validData.push_back(allData[i]);
		}
	}

	sort(validData.begin(), validData.end(), ContourData::sortContours);

	string ReadCharacters;

	for (int i = 0; i < validData.size(); i++){
		rectangle(matTestImage,
				validData[i].boundingRect,
				Scalar(0,255,0),
				2);
		Mat matROI = ThreshCopy(validData[i].boundingRect);
		Mat ROIResized;
		resize(matROI, ROIResized, Size(imageWidth, imageHeight));

		Mat ROIFloat;
		ROIResized.convertTo(ROIFloat, CV_32FC1);
		
		Mat matCurrentChar(0,0, CV_32F);

		kNearest->findNearest(ROIFloat.reshape(1,1), 5, matCurrentChar);
		
		float fltCurrentChar  = (float)matCurrentChar.at<float>(0,0);

		ReadCharacters = ReadCharacters + char(int(fltCurrentChar));
	}
	cout <<"\n"<<ReadCharacters<<"\n";
	imshow("Initial_image", matTestImage);

	waitKey(0);
	return(0);

}

