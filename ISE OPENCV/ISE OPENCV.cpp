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



#define NELEMS(x)  (int)(sizeof(x) / sizeof((x)[0]))

//to refer keyword (datatype: Mat)
using namespace std;
using namespace cv;

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




int main(int argc, char** argv)
{

	cout << setprecision(2) ;

	for (int n = 20; n > 1; n--)
	{
		Mat img_orig = imread("Dataset\\" + to_string(n) + ".jpg");

		//waitKey();
		imshow("result", img_orig);
		Mat img_gray = equatImage(img_orig, gray);
		waitKey();
		imshow("result", img_gray);
		Mat img_shar = Masking(img_gray, Masks(sharpen));
		waitKey();
		imshow("result", img_shar);
		Mat img_cont = equatImage(img_shar, contrast, 40);
		waitKey();
		imshow("result", img_cont);

		Mat img_ench = EqualizerHist(img_cont);
		waitKey();
		imshow("result", img_ench);
		Mat img_blur = Masking(img_ench, Masks(box_blur,2), 2, 25);
		waitKey();
		imshow("result", img_blur);
		Mat img_soby = Masking(img_blur, Masks(sobely_edge));
		waitKey();
		imshow("result", img_soby);
		Mat img_dila = equatImage(img_soby, binary, 100);

		imshow("gray", img_cont);
		//waitKey();
		//imshow("gray", img_dila);

		img_dila = morp(img_dila, dilation, plus1, 1);
		waitKey();
		imshow("result", img_dila);
		img_dila = morp(img_dila, eroson, vertical, 4);
		waitKey();
		imshow("result", img_dila);
		img_dila = morp(img_dila, dilation, horizontal, 3);
		waitKey();
		imshow("result", img_dila);
		//img_dila = morp(img_dila, dilation, vertical, 1);
		//img_dila = morp(img_dila, dilation, horizontal, 1);
		//
		//img_dila = morp(img_dila, dilation, full, 1, 5);




		Mat Blob = img_dila.clone();
		vector<vector<Point>> contours1;
		vector<Vec4i> hierachy1;
		findContours(img_dila, contours1, hierachy1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
		Mat dst = Mat::zeros(img_gray.size(), CV_8UC3);

		if (!contours1.empty())
		{
			for (int i = 0; i < contours1.size(); i++)
			{
				Scalar colour((rand() & 255), (rand() & 255), (rand() & 255));
				drawContours(dst, contours1, i, colour, CV_FILLED, 8, hierachy1);
			}
		}

		imshow("dila", dst);






		Mat plate;
		Rect BlobRect;
		Scalar black = CV_RGB(0, 0, 0);

		float max = 0;

		float area, density, ratio;


		for (int i = 1; i < contours1.size(); i++)
		{
			BlobRect = boundingRect(contours1[i]);

			 ratio = ((float)BlobRect.width / ((float)BlobRect.height + (float)BlobRect.width)) * 100;
			 area = (float)contourArea(contours1[i]);
			 density = area / ((float)BlobRect.height * (float)BlobRect.width);

			/*if (contourArea(contours1[i]) > max)
			{
			cout << "area:" << area
			<< "  size:" << contourArea(contours1[i]) << "  Rect: " << ((float)BlobRect.height * (float)BlobRect.width)
			<< "  height: " << (float)BlobRect.height << "  width: " << (float)BlobRect.width << endl;

			max = contourArea(contours1[i]);
			}*/


			if (BlobRect.x < Blob.size().width / 10 || BlobRect.x > Blob.size().width * 9 / 10 || BlobRect.y < Blob.size().height / 10 || BlobRect.y > Blob.size().height * 9 / 10)
			{
				drawContours(dst, contours1, i, black, CV_FILLED, 8, hierachy1);
			}
			else if (BlobRect.width < 45 || BlobRect.width > 200 || BlobRect.height> 60 || BlobRect.height < 10)
			{
				drawContours(dst, contours1, i, black, CV_FILLED, 8, hierachy1);
			}
			else if (ratio < 59 || ratio > 89 || density < 0.35 || density > 0.83)
			{
				drawContours(dst, contours1, i, black, CV_FILLED, 8, hierachy1);
			}
			else if (ratio * density < 34 || ratio * density > 70)
			{
				drawContours(dst, contours1, i, black, CV_FILLED, 8, hierachy1);
			}
			else
			{

				plate = img_gray(BlobRect);

			}

		}

		

		/*dst = morp(morp(morp(dst 
			, dilation, horizontal, 10)
			, eroson, vertical, 5)
			, dilation, plus1, 5);*/
		waitKey();
		imshow("result", dst);
		Mat grayplate = plate;
		float value = OTSU(plate);
		plate = equatImage(plate, binary, 200);

		cout << to_string(value) << endl;

		imshow("result0", plate);

		//imwrite("Dataset\\result" + to_string(i) + ".jpg", dst);

		ratio = (float)plate.cols / ((float)plate.rows + (float)plate.cols);

		cout << plate.cols << " " << plate.rows << " " << ratio  << endl;

		if (plate.cols > 95 && ratio >= 0.75)
		{
			plate = morp(plate, eroson, vertical, 1, 2);
			plate = morp(plate, dilation, vertical, 1);
		}

		//plate = morp(plate, eroson, horizontal, 1);
		//plate = morp(plate, dilation, horizontal, 1);
		//plate = morp(plate, eroson, vertical, 2);

		//plate = morp(plate, dilation, plus1, 1);
		//plate = morp(plate, dilation, plus1, 1);


		findContours(plate, contours1, hierachy1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
		dst = Mat::zeros(plate.size(), CV_8UC3);

		if (!contours1.empty())
		{
			for (int i = 0; i < contours1.size(); i++)
			{
				Scalar colour((rand() & 255), (rand() & 255), (rand() & 255));
				drawContours(dst, contours1, i, colour, CV_FILLED, 8, hierachy1);
			}
		}

		//imshow("result", dst);





		for (int i = 0; i < contours1.size(); i++)
		{
			BlobRect = boundingRect(contours1[i]);



			int ratio = ((float)BlobRect.width / ((float)BlobRect.height + (float)BlobRect.width)) * 100;
			float area = (float)contourArea(contours1[i]);
			float density = area / ((float)BlobRect.height * (float)BlobRect.width);


			//if (BlobRect.x < Blob.size().width / 100 || BlobRect.x > Blob.size().width * 99 / 100 || BlobRect.y < Blob.size().height / 100 || BlobRect.y > Blob.size().height * 99 / 100)
			//{
			//	drawContours(dst, contours1, i, black, CV_FILLED, 8, hierachy1);
			//}
			//else 
			if (BlobRect.width < 5 || BlobRect.width > 15 || BlobRect.height> 25 || BlobRect.height < 10)
			{
				drawContours(dst, contours1, i, black, CV_FILLED, 8, hierachy1);
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

				//plate = img_gray(BlobRect);
				imshow("character", grayplate(BlobRect));
				//imwrite("Dataset\\try" + to_string(n) + "-" + to_string(i) + ".jpg", grayplate(BlobRect));

				//cout << n << "\t density:" << density << "\t ratio:" << ratio << "\t a:" << ratio * area
				//	<< "\t area:" << contourArea(contours1[i]) << "\t Rect: " << ((float)BlobRect.height * (float)BlobRect.width)
				//	<< "\t height: " << (float)BlobRect.height << "\t width: " << (float)BlobRect.width << endl;
			}

		}





		imshow("result2", dst);





		waitKey();
	}


    return 0;
}
