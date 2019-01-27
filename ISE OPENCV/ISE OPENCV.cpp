// ISE OPENCV.cpp : Defines the entry point for the console application.

/*
Setting for OpenCV(build)
	- Propertise > C/C++ (for once) VC++ (for all time) > General > Add addition library 
		add opencv\include + opencv\include\opencv + opencv\include\opencv2

	- Propertise > Linker > General > Add addition library 
		add build\x64\VC14 + VC15\lib

	- Propertise > Linker > Input > Add addition library
		add build\x64\vc15\lib\opencv_world343.lib (d for debug fast compilation)

Link to DLL to execute program successfully(run)
	- Copy file at build\x64\vc15\bin\opencv_world343.dll
		to Containing Folder

	- Build with x64 Release

	- Refer to Uninstall Program Error
*/

#include "stdafx.h"
#include "string.h"
#include "cmath"
#include "iostream"
#include <iomanip>

#include "highgui\highgui.hpp"
#include "core\core.hpp"
#include "imgproc.hpp"

#include "baseapi.h"
#include "allheaders.h"


#define NELEMS(x)  (int)(sizeof(x) / sizeof((x)[0]))

//to refer keyword (datatype: Mat)
using namespace std;
using namespace cv;

double mapvalue(double input, double input_start, double input_end, double output_start, double output_end)
{
	double slope = 1.0 * (output_end - output_start) / (input_end - input_start);
	double output = output_start + slope * (input - input_start);

	return output;
}

Mat EqualizerHist(Mat Grey)
{
	Mat BetterImg = Mat::zeros(Grey.size(), CV_8UC1);;
	int iCount[256] = { 0 };

	int iH = Grey.rows;
	int iR = Grey.cols;

	//Step 1: Count
	for (int y = 0; y < iH; y++)
	{
		for (int x = 0; x < iR; x++)
		{
			iCount[Grey.at<uchar>(y, x)] ++;
		}
	}

	float iAcc = 0;
	float iTotal = (float)iH * iR;
	int iNew[256] = { 0 };

	for (int i = 0; i < 256; i++)
	{
		//Step 2: Probability of each GV
		//Step 3: Accumulate of Probability of GV
		iAcc += (float)iCount[i] / iTotal;

		//Step 4: (NumGV - 1) * AccGV + RoundUp
		iNew[i] = round((256 - 1) * iAcc);
	}

	//cout << to_string(iNew[255]) << endl;

	//Step 5: = > New Value <
	for (int y = 0; y < iH; y++)
	{
		for (int x = 0; x < iR; x++)
		{
			BetterImg.at<uchar>(y, x) = iNew[Grey.at<uchar>(y, x)];
		}
	}

	return BetterImg;
}

int OTSU(Mat Grey)
{
	int iCount[256] = { 0 };

	int iH = Grey.rows;
	int iR = Grey.cols;


	//Count
	for (int y = 0; y < iH; y++)
	{
		for (int x = 0; x < iR; x++)
		{
			iCount[Grey.at<uchar>(y, x)] ++;
		}
	}

	float iTotal = (float)iH * iR;
	
	float iTheta = 0;
	float iThetaList[256] = { 0 };

	float iMiu = 0;
	float iMiuList[256] = { 0 };

	for (int i = 0; i < 256; i++)
	{
		//Probability of each GV
		//Accumulate of Probability of GV
		iTheta += (float)iCount[i] / iTotal;
		iThetaList[i] = iTheta;

		//Milk
		iMiu += (float)i * (float)iCount[i] / iTotal;
		iMiuList[i] = iMiu;
	}

	//cout << to_string(iNew[255]) << endl;

	int index = 0;
	int value = 0;
	//Step 5: = > New Value <
	for (int i = 0; i < 256; i++)
	{
		int temp = iMiuList[255] * iThetaList[i] - iMiuList[i];
		int tempvalue = (temp * temp) / (iThetaList[i] * (1 - iThetaList[i]));


		if (value < tempvalue)
		{
			index = i;
			value = tempvalue;
		}
	}

	return index;
}


//Process pixel with math function
enum equation {	gray,	invert1,	step,	binary,	contrast};
Mat equatImage(Mat Grey, equation type, float a = 50, int b = 100)
{
	Mat processedResult = Mat::zeros(Grey.size(), CV_8UC1);
	int iH = Grey.rows;
	int iR = Grey.cols;
	int n = 0;

	for (int y = n; y < iH - n; y++)
	{
		for (int x = n; x < iR - n; x++)
		{
			//cout << to_string(pixel) << endl;

			float pixel = (float)Grey.at<uchar>(y, x);

			switch (type)
			{
			case gray:
				processedResult.at<uchar>(y, x) = (Grey.at<uchar>(y, x * 3 + 0) + Grey.at<uchar>(y, x * 3 + 1) + Grey.at<uchar>(y, x * 3 + 2)) / 3;
				break;
			case invert1:
				processedResult.at<uchar>(y, x) = 255 - pixel;
				break;
			case step:
				processedResult.at<uchar>(y, x) = (pixel >= a && pixel <= b) ? 255 : 0;
				break;
			case binary:
				processedResult.at<uchar>(y, x) = (pixel >= a) ? 255 : 0;
				break;
			case contrast:

				float temp;

				if (pixel > 122)
					temp = pixel - 122;
				else
					temp = 123 - pixel;

				temp = (temp / 123);

				temp *= ((100 - a) / 100 * 123);

				temp += ((a / 100) * 123);

				temp = (int)(pixel > 122) ? temp + 122 : 123 - temp;

				if (temp < 0)
					cout << temp << endl;
				//means gg

				processedResult.at<uchar>(y, x) = temp;
				break;
			default:
				break;
			} 

		}
	}

	return processedResult;
}


// Custom Masking
enum mask_type {	box_blur,	gaussian_blur,	laplacian_edge,	sobelx_edge,	sobely_edge, sobely2_edge, sharpen,	Gsharpen};
int **Masks(mask_type mask, int value_a = 1) {

	int n = value_a * 2 + 1;
	int **img = 0;

	//setting
	switch (mask)
	{
	case box_blur:
	case gaussian_blur:

		img = new int*[n];

		for (int i = 0; i < n; i++) {
			img[i] = new int[n];
		}

		break;

	case laplacian_edge:
	case sobelx_edge:
	case sobely_edge:
	case sobely2_edge:
	case sharpen:
	case Gsharpen:
		
		n = 3;
		img = new int*[n];

		for (int i = 0; i < n; i++) {
			img[i] = new int[n];
		}

		break;

	default:
		break;
	}


	/*
	Masking
	*/
	switch (mask)
	{
	case box_blur:

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				img[i][j] = 1;
			}
		}

		break;


	case sharpen:

		img[0][0] = 0;
		img[0][1] = -1;
		img[0][2] = 0;
		img[1][0] = -1;
		img[1][1] = 5;
		img[1][2] = -1;
		img[2][0] = 0;
		img[2][1] = -1;
		img[2][2] = 0;

		break;

	case Gsharpen:

		img[0][0] = 1;
		img[0][1] = 2;
		img[0][2] = 1;
		img[1][0] = 2;
		img[1][1] = -28;
		img[1][2] = 2;
		img[2][0] = 1;
		img[2][1] = 2;
		img[2][2] = 1;

		break;

	case laplacian_edge:

		img[0][0] = 0;
		img[0][1] = 1;
		img[0][2] = 0;
		img[1][0] = 1;
		img[1][1] = -4;
		img[1][2] = 1;
		img[2][0] = 0;
		img[2][1] = 1;
		img[2][2] = 0;

		break;

	case sobelx_edge:

		img[0][0] = -1;
		img[0][1] = -1;
		img[0][2] = -1;
		img[1][0] = 0;
		img[1][1] = 0;
		img[1][2] = 0;
		img[2][0] = 1;
		img[2][1] = 1;
		img[2][2] = 1;

		break;

	case sobely_edge:

		img[0][0] = -1;
		img[0][1] = 0;
		img[0][2] = 1;
		img[1][0] = -2;
		img[1][1] = 0;
		img[1][2] = 2;
		img[2][0] = -1;
		img[2][1] = 0;
		img[2][2] = 1;

		break;

	case sobely2_edge:

		img[0][0] = -1;
		img[0][1] = 0;
		img[0][2] = 1;
		img[1][0] = -1;
		img[1][1] = 0;
		img[1][2] = 1;
		img[2][0] = -1;
		img[2][1] = 0;
		img[2][2] = 1;

		break;

	default:
		break;
	}

	return img;
}
Mat Masking(Mat Grey, int **Filter, int n = 1, double FilterDivider = 1)
{
	Mat Result = Mat::zeros(Grey.size(), CV_8UC1);
	int iH = Grey.rows;
	int iR = Grey.cols;
	int F_size = n * 2 + 1;

	/*int **Filter;
	*Filter = new int[F_size];
	for (int i = 0; i < F_size; i++) {
		*Filter[i] = *_Filter[i];
	}*/


	if (F_size % 2 != 1 || F_size % 2 != 1)
		throw "ISE Program: Filter should be stay at odd number.";
	else if (F_size != F_size)
		throw "ISE Program: Filter should be square size";

	for (int y = n; y < iH - n; y++)
	{
		for (int x = n; x < iR - n; x++)
		{
			double sum = 0;

			for (int i = -1 * n; i <= n; i++)
			{
				for (int j = -1 * n; j <= n; j++)
				{
					sum += Grey.at<uchar>(y + i, x + j) * Filter[i + n][j + n];
				}
			}

			//cout << to_string(sum / FilterDivider) << endl;
			int temp = abs((int)(sum / FilterDivider));
			Result.at<uchar>(y, x) = temp > 255 ? 255 : temp;
		}
	}

	return Result;
}



enum morphology_type {	dilation,	eroson};
enum structure {	full,	plus1,	horizontal,	vertical, cross};

Mat morp(Mat Grey, morphology_type type, structure shape, int repeat = 1, int offsetNumber = 1)
{
	Mat Result = Mat::zeros(Grey.size(), CV_8UC1);
	int iH = Grey.rows;
	int iR = Grey.cols;
	int inttype = 255 - (type == dilation ? 0 : 255);

	Mat temp;

	for (int re = 0; re < repeat; re++)
	{
		temp = Result;

		for (int y = offsetNumber; y < iH - offsetNumber; y++)
		{
			for (int x = offsetNumber; x < iR - offsetNumber; x++)
			{
				int flag = 0;

				if (shape == cross)
				{
					for (int i = -offsetNumber; i <= offsetNumber; i++)
					{
						if (Grey.at<uchar>(y + i, x + i) == inttype) { flag = 1; }
						if (Grey.at<uchar>(y - i, x + i) == inttype) { flag = 1; }
					}
				}

				if (shape == horizontal || shape == plus1 || shape == full)
				{
					for (int i = -offsetNumber; i <= offsetNumber; i++)
					{
						if (Grey.at<uchar>(y, x + i) == inttype) { flag = 1; }
					}
				}


				if (shape == vertical || shape == plus1 || shape == full)
				{
					for (int i = -offsetNumber; i <= offsetNumber; i++)
					{
						if (Grey.at<uchar>(y + i, x) == inttype) { flag = 1; }
					}
				}


				if (shape == full)
				{
					for (int i = -offsetNumber; i <= offsetNumber; i++)
					{
						for (int j = -offsetNumber; j <= offsetNumber; j++)
						{
							if (Grey.at<uchar>(y + i, x + j) == inttype) { flag = 1; }
						}
					}
				}


				//cout << to_string(pixel) << endl;
				if (flag == 1)
					temp.at<uchar>(y, x) = inttype;
				else
					temp.at<uchar>(y, x) = 255 - inttype;
			}
		}

		Result = temp;
	}

	return Result;
}


Mat ColorContours(Mat img_gray,vector<vector<Point>> contours1, vector<Vec4i> hierachy1)
{
	Mat result = Mat::zeros(img_gray.size(), CV_8UC3);
	if (!contours1.empty())
	{
		for (int i = 0; i < contours1.size(); i++)
		{
			Scalar colour((rand() & 255), (rand() & 255), (rand() & 255));
			drawContours(result, contours1, i, colour, CV_FILLED, 8, hierachy1);
		}
	}

	return result;
}



int main(int argc, char** argv)
{

	cout << setprecision(2) ;

	printf("%-10s%-10s%-10s%-10s%-10s\n", "i", "ratio", "area", "density","-");

	for (int n = 1; n <= 20; n++)
	{


		////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////// IMAGE PROCESS /////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////

		//Mat img_orig = imread("Dataset\\" + to_string(n) + ".jpg");
		//Mat img_gray = equatImage(img_orig, gray);
		//Mat img_ench = EqualizerHist(img_gray);
		//Mat img_blur = Masking(img_gray, Masks(box_blur,2), 2, 25);
		//Mat img_soby = Masking(img_blur, Masks(sobely_edge));
		//Mat img_dila = equatImage(img_soby, binary, 100);

		//imshow("result", img_dila);
		////waitKey();



		////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////// Morphology //////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////

		//img_dila = morp(img_dila, dilation, plus1, 4);
		//img_dila = morp(img_dila, eroson, vertical, 2);
		//img_dila = morp(img_dila, dilation, horizontal, 4);
		//img_dila = morp(img_dila, dilation, plus1, 4);

		//Mat Blob = img_dila.clone();

		vector<vector<Point>> shape_result = vector<vector<Point>>();
		vector<Vec4i> hierachy_result = vector<Vec4i>();

		vector<vector<Point>> contours1;
		vector<Vec4i> hierachy1;
		//findContours(img_dila, contours1, hierachy1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

		//imshow("result", ColorContours(img_dila, contours1, hierachy1));
		////waitKey();



		////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////// FILTER 1 ////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////

		//Mat plate;
		//Rect BlobRect;
		//Scalar black = CV_RGB(0, 0, 0);

		//float max = 0;
		//float area, density, ratio;

		//for (int i = 0; i < contours1.size(); i++)
		//{
		//	BlobRect = boundingRect(contours1[i]);

		//	 ratio = ((float)BlobRect.width / ((float)BlobRect.height + (float)BlobRect.width)) * 100;
		//	 area = (float)contourArea(contours1[i]);
		//	 density = area / ((float)BlobRect.height * (float)BlobRect.width);

		//	/*if (contourArea(contours1[i]) > max)
		//	{
		//	cout << "area:" << area
		//	<< "  size:" << contourArea(contours1[i]) << "  Rect: " << ((float)BlobRect.height * (float)BlobRect.width)
		//	<< "  height: " << (float)BlobRect.height << "  width: " << (float)BlobRect.width << endl;

		//	max = contourArea(contours1[i]);
		//	}*/


		//	if (BlobRect.x < Blob.size().width / 10 || BlobRect.x > Blob.size().width * 9 / 10 || BlobRect.y < Blob.size().height / 10 || BlobRect.y > Blob.size().height * 9 / 10)
		//	{
		//		drawContours(img_dila, contours1, i, black, CV_FILLED, 8, hierachy1);
		//	}
		//	else if (BlobRect.width < 45 || BlobRect.width > 200 || BlobRect.height> 60 || BlobRect.height < 10)
		//	{
		//		drawContours(img_dila, contours1, i, black, CV_FILLED, 8, hierachy1);
		//	}
		//	else if (ratio < 55 || ratio > 90 || density < 0.3 || density > 0.85)
		//	{
		//		drawContours(img_dila, contours1, i, black, CV_FILLED, 8, hierachy1);
		//	}
		//	else
		//	{
		//		shape_result.push_back(contours1[i]);
		//		hierachy_result.push_back(hierachy1[i]);
		//	}
		//}

		//imshow("result" , ColorContours(img_dila, shape_result, hierachy_result));
		////waitKey();


		////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////// Morphology 2 //////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////

		//img_dila = morp(img_dila, dilation, plus1, 1);
		//img_dila = morp(img_dila, eroson, vertical, 8);
		//img_dila = morp(img_dila, dilation, horizontal, 3);

		//imshow("result", img_dila);
		////waitKey();

		////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////// FILTER 2 ////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////

		//for (int i = 0; i < shape_result.size(); i++)
		//{
		//	BlobRect = boundingRect(shape_result[i]);

		//	ratio = ((float)BlobRect.width / ((float)BlobRect.height + (float)BlobRect.width)) * 100;
		//	area = (float)contourArea(shape_result[i]);
		//	density = area / ((float)BlobRect.height * (float)BlobRect.width);

		//	printf("%-10d%-10.2f%-10.2f%-10.2f", n, ratio,area,density);

		//	if (ratio < 59 || ratio > 89 || density < 0.35 || density > 0.83)
		//	{
		//		drawContours(img_dila, shape_result, i, black, CV_FILLED, 8, hierachy_result);
		//		printf("%-10s\n", "X");
		//	}
		//	else if (ratio * density < 34 || ratio * density > 70)
		//	{
		//		drawContours(img_dila, shape_result, i, black, CV_FILLED, 8, hierachy_result);
		//		printf("%-10s\n", "X");
		//	}
		//	else
		//	{
		//		plate = img_gray(BlobRect);
		//		printf("%-10s\n","O");
		//	}
		//}


		//imshow("result", img_dila);
		//waitKey();


		//char input;
		//cout << "continue? y/n ";
		//cin >> input;
		//if (input == 'n')
		//{
		//	continue;
		//}
		

		//////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////// CHARACTER SEPERATE /////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////


		Mat plate = equatImage(imread("Dataset\\result_plate\\" + to_string(n) + ".jpg"),gray);
		Mat grayplate = plate.clone();

		float value = OTSU(plate) + 50;
		//printf("%-10d%-10.2f\n", n, value);
		float ratio = (float)plate.cols / ((float)plate.rows + (float)plate.cols);
		printf("%-10d%-10.2f%-10.2f%-10.2f", n, ratio, plate.cols, plate.rows);

		plate = equatImage(plate, binary, value);
		imshow("result_plate", plate);
		//waitKey();


		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////// Morphology //////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////


		/*if (plate.cols > 95 && ratio >= 0.75)
		{
			plate = morp(plate, eroson, vertical, 1, 2);
			plate = morp(plate, dilation, vertical, 1);
		}

		plate = morp(plate, eroson, horizontal, 1);
		plate = morp(plate, dilation, horizontal, 1);
		plate = morp(plate, eroson, vertical, 2);

		plate = morp(plate, dilation, plus1, 1);
		plate = morp(plate, dilation, plus1, 1);*/

		findContours(plate, contours1, hierachy1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
		imshow("result_plate", ColorContours(plate, contours1, hierachy1));
		waitKey();

		//////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////// FILTER 1 //////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////

		shape_result.clear();
		hierachy_result.clear();

		for (int i = 0; i < contours1.size(); i++)
		{
			Rect BlobRect = boundingRect(contours1[i]);
			Scalar black = CV_RGB(0, 0, 0);

			int ratio = ((float)BlobRect.width / ((float)BlobRect.height + (float)BlobRect.width)) * 100;
			float area = (float)contourArea(contours1[i]);
			float density = area / ((float)BlobRect.height * (float)BlobRect.width);

			if (BlobRect.width > 15 || BlobRect.height> 25 || BlobRect.height < 10)
			{
				drawContours(plate, contours1, i, black, CV_FILLED, 8, hierachy1);
			}
			//else if (ratio < 59 || ratio > 89 || density < 0.35 || density > 0.83)
			//{
			//	drawContours(dst, contours1, i, black, CV_FILLED, 8, hierachy1);
			//}
			//else if (ratio * density < 34 || ratio * density > 70)
			//{
			//	drawContours(dst, contours1, i, black, CV_FILLED, 8, hierachy1);
			//}
			else
			{
				shape_result.push_back(contours1[i]);
				hierachy_result.push_back(hierachy1[i]);

				//plate = img_gray(BlobRect);
				//imshow("character", grayplate(BlobRect));
				imwrite("Dataset\\try" + to_string(n) + "-" + to_string(i) + ".jpg", grayplate(Rect(BlobRect.x - 1, BlobRect.y - 1, BlobRect.width + 2, BlobRect.height + 2)));
			}

		}

		printf("%-10d\n", shape_result.size());

		imshow("result_plate", plate);
		waitKey();


		//////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////// CHARACTER RECOGNIZE ///////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////

		


	}

	waitKey();
    return 0;
}
