// minimal.cpp: Display the landmarks of a face in an image.
//              This demonstrates stasm_search_single.



#include <stdio.h>
#include <stdlib.h>
#include <list>

// OpenCV
#include <opencv2//core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/video/tracking.hpp>
#include "opencv/highgui.h"
#include "stasm_lib.h"
#include "FaceRecognition.h"
#include "HairExtraction.h"
#include "SkinSynthesis.h"
#include "HairEditing.h"

using namespace cv;

void createMosaic(std::string dir, std::string ext, int nRows, int nCols, Scalar textColor)
{
	Scalar backgroundColor = Scalar(255, 255, 255);
	Mat mosaic(2 * nRows, 9 * nCols, CV_8UC3, backgroundColor);

	for (int fileInd = 1; fileInd <= 18; fileInd++)
	{
		string pathStringModel = dir + std::to_string(fileInd) + ext;

			const char * path = pathStringModel.c_str();

			Mat img(imread(path, CV_LOAD_IMAGE_COLOR));
			if (!img.data)
			{	
				img = Mat(nRows, nCols, CV_8UC3, backgroundColor);
			}

			int xStart, yStart;

			if (fileInd <= 9)
			{
				yStart = 0;
				xStart = (fileInd - 1)*nCols;
			

			}
			else
			{
				yStart = nRows;
				xStart = (fileInd - 10)*nCols;
			}
			

			Rect rect(xStart, yStart, nCols, nRows);

			Mat roi(mosaic(rect));

			putText(img, std::to_string(fileInd), cvPoint(0, 35), FONT_HERSHEY_PLAIN, 3, textColor, 3);
			img.copyTo(roi);

		/*	cv::imshow("img", img);
			cv::waitKey();*/
	}

	double scale = 0.5;
	Mat resized;
	Mat scalingMatrix = (Mat_<double>(2, 3) << scale, 0, 0, 0, scale, 0);

	int newHeight = mosaic.rows*scale;
	int newWidth = mosaic.cols*scale;

	warpAffine(mosaic, resized, scalingMatrix, Size(newWidth, newHeight), 1, 0, backgroundColor);

	/*cv::imshow("mosaic", resized);
	cv::waitKey();*/

	cv::imwrite(dir + "mosaic.bmp", resized);
	
}

Mat generateResultImage(Mat imgTarget, Mat imgModel, Mat hairSwap)
{

	int nCols = imgTarget.cols;
	int nRows = imgTarget.rows;

	Mat resultImage(nRows, 3 * nCols, CV_8UC3);

	Rect rectModel(0, 0, nCols, nRows);

	Mat roiModel(resultImage(rectModel));

	putText(imgModel, "Model", cvPoint(0, 30), FONT_HERSHEY_PLAIN, 2, Scalar(255,0,0), 3);
	imgModel.copyTo(roiModel);


	Rect rectTarget(nCols, 0, nCols, nRows);

	Mat roiTarget(resultImage(rectTarget));

	putText(imgTarget, "Target", cvPoint(0, 30), FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 0), 3);
	imgTarget.copyTo(roiTarget);

	Rect rectSwap(2*nCols, 0, nCols, nRows);

	Mat roiSwap(resultImage(rectSwap));

	putText(hairSwap, "Result", cvPoint(0, 30), FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 0), 3);
	hairSwap.copyTo(roiSwap);

	return resultImage;

}

std::string removeExtension(const std::string& filename) {
	size_t lastdot = filename.find_last_of(".");
	if (lastdot == std::string::npos) return filename;
	return filename.substr(0, lastdot);
}

int main(int argc, char *argv[])
{

	/*string dataDir = "data/";
	string faceDir = "faces/";
	string modelsDir = "hairModels/";
	string segmentationDir = "segmentation/";
	string resultsDir = "results/";*/

	/*createMosaic(dataDir, ".png", 320, 240, Scalar(255,0,0));
	createMosaic(faceDir, ".bmp", 320, 240, Scalar(255,0,0));
	createMosaic(modelsDir, ".bmp", 320, 240, Scalar(255,0,0));*/
	//createMosaic(segmentationDir, ".bmp", 320, 240, Scalar(255,255,255));

	//for (int fileInd = 1; fileInd <= 18; fileInd++)
	//{
	//	string pathStringModel = dataDir + std::to_string(fileInd) + ".png";

	//	const char * pathModel = pathStringModel.c_str();
	//	Mat_<unsigned char> imgGrayModel(imread(pathModel, CV_LOAD_IMAGE_GRAYSCALE));
	//	Mat imgRGBModel(imread(pathModel, CV_LOAD_IMAGE_COLOR));
	//	if (!imgGrayModel.data)
	//	{
	//		printf("Cannot load %s\n", imgGrayModel);
	//		continue;
	//		//exit(1);
	//	}

	//	Face faceModel = detectFace(imgGrayModel, pathModel, dataDir.c_str());

	//	Mat segmentationLabels;
	//	Hair hairModel;

	//	int retCode = extractHair(imgRGBModel, faceModel, &segmentationLabels, &hairModel);

	//	if (retCode == -1)
	//	{
	//		printf("Could not extract hair\n");
	//		continue;
	//	}

	//	cv::imwrite(segmentationDir + std::to_string(fileInd) + "_segmentation.bmp", segmentationLabels);

	//	Mat hairPixels(imgRGBModel.rows, imgRGBModel.cols, CV_8UC3, Scalar(255, 255, 255));
	//	imgRGBModel.copyTo(hairPixels, hairModel.getHairMask());

	//	cv::imwrite(modelsDir + std::to_string(fileInd) + "_hairModel.bmp", hairPixels);

	//	Mat synthesizedFace = synthesizeSkin(imgRGBModel, faceModel, hairModel.getHairMask());

	//	cv::imwrite(faceDir + std::to_string(fileInd) + "_face.bmp", synthesizedFace);
	//}

	string dataDir = "data/";
	string faceDir = "faces/";
	string modelsDir = "hairModels/";
	string segmentationDir = "segmentation/";
	string resultsDir = "results/";

	/*int targetInd = 7;
	int modelInd = 6;

	string pathStringTarget = dataDir + std::to_string(targetInd) + ".png";
	string pathStringModel = dataDir + std::to_string(modelInd) + ".png";*/

	string model;
	string target;

	if (argc == 3)
	{
		model = argv[1];
		target = argv[2];
	}

	else
	{
		target = "7.png";
		model =  "6.png";
	}

	string pathStringModel = dataDir + model;
	string pathStringTarget = dataDir + target;

	const char * pathTarget = pathStringTarget.c_str();
	const char * pathModel = pathStringModel.c_str();
	const char * dataDirC = dataDir.c_str();

	Mat_<unsigned char> imgGrayModel(imread(pathModel, CV_LOAD_IMAGE_GRAYSCALE));
	Mat imgRGBModel(imread(pathModel, CV_LOAD_IMAGE_COLOR));
	if (!imgGrayModel.data)
	{
		printf("Cannot load %s\n", imgGrayModel);
		exit(1);
	}

	printf("Detecting face from model... \n");
	Face faceModel = detectFace(imgGrayModel, pathModel, dataDirC);
	printf("Face detected. \n");

	Mat segmentationLabelsModel;
	Hair hairModel;

	printf("Extracting hair from model... \n");
	int retCode = extractHair(imgRGBModel, faceModel, &segmentationLabelsModel, &hairModel);
	if (retCode == -1)
	{
		printf("Cannot extract hair. \n");
	}
	printf("Hair extracted. \n");

	Mat_<unsigned char> imgGrayTarget(imread(pathTarget, CV_LOAD_IMAGE_GRAYSCALE));
	Mat imgRGBTarget(imread(pathTarget, CV_LOAD_IMAGE_COLOR));
	if (!imgGrayTarget.data)
	{
		printf("Cannot load %s\n", imgGrayTarget);
		exit(1);	}	
	
	printf("Detecting face from target... \n");
	Face faceTarget = detectFace(imgGrayTarget, pathTarget, dataDirC);
	Mat segmentationLabelsTarget;
	Hair hairTarget;
	printf("Face detected. \n");

	printf("Extracting hair from target... \n");
	retCode = extractHair(imgRGBTarget, faceTarget,&segmentationLabelsTarget,&hairTarget);

	if (retCode == -1)
	{
		printf("Cannot extract hair.\n");
	}
	printf("Hair extracted. \n");

	printf("Synthesizing face... \n");
	Mat synthesizedface = synthesizeSkin(imgRGBTarget, faceTarget, hairTarget.getHairMask());
	printf("Face synthesized. \n");

	Mat hairSwap = swapHair(hairModel, faceTarget, imgRGBModel, imgRGBTarget, synthesizedface);

	Mat resultImage = generateResultImage(imgRGBTarget, imgRGBModel, hairSwap);

	cv::imwrite(resultsDir + "Hair" + removeExtension(model) + "xFace" + removeExtension(target) + ".bmp", resultImage);
	
    return 0;
}
