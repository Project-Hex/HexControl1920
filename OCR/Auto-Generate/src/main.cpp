#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>
#include<iostream>
#include<vector>

const int min_area = 100;
const int image_width = 20;
const int image_height = 30;

using namespace cv;
using namespace std;

int main(){
	Mat imgRGB;
	Mat imgGray;
	Mat imgBlurring;
	Mat imgBW;
	Mat imgBWCopy;

	Mat GroupInts;
	Mat FlattenedFloats;


	int Orientation = 1;
	
	
	vector<vector<Point>> ptContours; 
	vector<Vec4i> v4iHierarchy; 
	vector <String> fileList;

	string characterArray("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	string orientation("0123");
	string Directory("/home/jgbroz/Documents/Project-Hex/OCR/Auto-Generate/A/0/*.bmp");

	for (int i = 0; i <= 25; i++){
		Directory.replace(53, 1, characterArray, i, 1);
		cout<<Directory<<"\n";
		for (int j = 0; j <= 3; j++){
			Directory.replace(55, 1, orientation, j, 1); 
			glob(Directory, fileList, false);
			for (size_t k = 0; k < fileList.size(); ++k){
				cout<<"Displaying the file list:" <<fileList[k]<<"\n";
				imgRGB = imread(fileList[k]);
				imshow("Test Result", imgRGB);
				cout<< characterArray[i];

				if (imgRGB.empty()){
					cout<<"Image not in directory\n";
					return(0);
				}

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
						CHAIN_APPROX_SIMPLE);

				for (int i = 0; i < ptContours.size(); i++){
					if (contourArea(ptContours[i]) > min_area){
						Rect rectangleChar = boundingRect(ptContours[i]);
						rectangle(imgRGB, rectangleChar, Scalar(255,0,0),2);
						Mat matROI = imgBW(rectangleChar);
						Mat matROIResized;
						resize(matROI, matROIResized, Size(image_width, image_height));

						imshow("matROI",matROI);
						imshow("matROIResized", matROIResized);
						imshow("imgRGB", imgRGB);
						
						waitKey(0);
						
						cout<<"\n"<<v4iHierarchy[i]<<"\n";							
						cout<<"\n"<<ptContours[i]<<"\n";
						cout<<"valid input appending the xml files.\n";

						GroupInts.push_back(int(characterArray[i]));

						Mat matImageFloat;

						matROIResized.convertTo(matImageFloat, CV_32FC1);

						Mat FlatFloats = matImageFloat.reshape(1,1);

						FlattenedFloats.push_back(FlatFloats);
						}
					}
				}
			}	
		}
	
	cout<<"Training Completed\n";

	FileStorage fsClassifications("group_ints.xml", FileStorage::WRITE);
	if (fsClassifications.isOpened() == false){
		cout<<"Unable to open group_ints\n";
		return(0);
	}

	fsClassifications<<"groups"<<GroupInts;
	fsClassifications.release();

	FileStorage fsTrainingImage("image.xml",FileStorage::WRITE);
	if (fsTrainingImage.isOpened() == false){
		cout<<"Unable to open image\n";
		return(0);
	}
	fsTrainingImage<<"images"<<FlattenedFloats;
	fsTrainingImage.release();
	return(0);
}
		
		
			


