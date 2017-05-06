// testOpenCV.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cstdio>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cmath>
#include <opencv2\features2d.hpp>

using namespace cv;
using namespace std;

RNG rng(12345);

Mat grayscale(Mat input);
Mat grayscale_unweighted(Mat input);
Mat binary(Mat gray);
Mat binary2(Mat gray);
void toFile(double area, double height, double width);
Mat binary_sky(Mat input);
Mat binary_land(Mat input);
Mat morphology_sky(Mat& bin);
Mat morphology_land(Mat& bin);
void stuff(Mat& morph);
void land_stuff(Mat& morph);

/*
	Detect sky
	Eclose in rectangle box
	extract objects from the bounding box
*/

int main()
{
	Mat img_sky;
	Mat img_land;
	for (int k = 1; k <= 128; k++) {
		/// Load colour image and create empty images for output:
		string a = to_string(k);

		//img = imread("D:\\Downloads\\image_data\\training_data\\" + a + ".jpg", IMREAD_ANYCOLOR);
		//img_sky = imread("D:\\Downloads\\image_data\\training_data\\5.jpg", IMREAD_ANYCOLOR);

		//img_land = imread("D:\\Downloads\\image_data\\landing_training\\training (10).jpg", IMREAD_ANYCOLOR);
		img_land = imread("D:\\Downloads\\image_data\\landing_training\\training (" + a + ").jpg", IMREAD_ANYCOLOR);

		//Size size(640, 480);
		Size size(800, 600);
		//resize(img_sky, img_sky, size);
		resize(img_land, img_land, size);

		///Convert to grayscale with modified Blue weight
		//Mat gray_sky = Mat(img_sky.rows, img_sky.cols, CV_8U);
		//gray_sky = grayscale(img_sky);

		Mat gray_land = Mat(img_land.rows, img_land.cols, CV_8U);
		gray_land = grayscale(img_land);


		///thresholding and returning a binary image
		//Mat bin_sky = Mat(gray_sky.rows, gray_sky.cols, CV_8U);
		//bin_sky = binary(gray_sky);

		Mat bin_land = Mat(gray_land.rows, gray_land.cols, CV_8U);
		bin_land = binary_land(gray_land);

		//Mat morph_sky = morphology_sky(bin_sky);
		Mat morph_land = morphology_land(bin_land);

		//imshow("Gray", gray_land);
		//imshow("Input", img_land);
		//imshow("Binary", bin_land);

		//stuff(morph_sky);
		land_stuff(morph_land);
}
	

	cvWaitKey(0);
	return 0;
}

Mat grayscale(Mat input) {
	Mat output = Mat(input.rows, input.cols, CV_8U);
	const float WR = 0.0;
	const float WG = 0.0;
	const float WB = 1.0;

	for (int x = 0; x < input.cols; ++x) {
		for (int y = 0; y < input.rows; ++y) {
			int red = (input.at<Vec3b>(Point(x, y))[2])*WR;
			int green = (input.at<Vec3b>(Point(x, y))[1])*WG;
			int blue = (input.at<Vec3b>(Point(x, y))[0])*WB;
			output.at<uchar>(Point(x, y)) = red + green + blue;
		}
	}
	return output;
}

Mat grayscale_unweighted(Mat input) {
	Mat output = Mat(input.rows, input.cols, CV_8U);
	const float WR = 0.299;
	const float WG = 0.587;
	const float WB = 0.114;

	for (int x = 0; x < input.cols; ++x) {
		for (int y = 0; y < input.rows; ++y) {
			int red = (input.at<Vec3b>(Point(x, y))[2])*WR;
			int green = (input.at<Vec3b>(Point(x, y))[1])*WG;
			int blue = (input.at<Vec3b>(Point(x, y))[0])*WB;
			output.at<uchar>(Point(x, y)) = red + green + blue;
		}
	}
	return output;
}

Mat binary2(Mat gray) {
	Mat bin = Mat(gray.rows, gray.cols, CV_8UC1);

	for (int x = 0; x < gray.cols; x++) {
		for (int y = 0; y < gray.rows; y++) {
			if (gray.at<uchar>(Point(x, y)) < 100) {
				bin.at<uchar>(Point(x, y)) = 255;
			}
			else {
				bin.at<uchar>(Point(x, y)) = 0;
			}
		}
	}
	return bin;
}

Mat binary(Mat gray) {
	Mat bin = Mat(gray.rows, gray.cols, CV_8UC1);

	for (int x = 0; x < gray.cols; x++) {
		for (int y = 0; y < gray.rows; y++) {
			if (gray.at<uchar>(Point(x, y)) < 120) {
				bin.at<uchar>(Point(x, y)) = 0;
			}
			else {
				bin.at<uchar>(Point(x, y)) = 255;
			}
		}
	}
	return bin;
}

Mat binary_sky(Mat input) {
	Mat bin = Mat(input.rows, input.cols, CV_8UC1);

	for (int x = 0; x < input.cols; x++) {
		for (int y = 0; y < input.rows; y++) {
			if (input.at<Vec3b>(Point(x, y))[2] > 0){
				bin.at<uchar>(Point(x, y)) = 0;
			}
			else {
				bin.at<uchar>(Point(x, y)) = 255;
			}
		}
	}
	return bin;
}

Mat binary_land(Mat gray) {
	Mat bin = Mat(gray.rows, gray.cols, CV_8UC1);

	for (int x = 0; x < gray.cols; x++) {
		for (int y = 0; y < gray.rows; y++) {
			if (gray.at<uchar>(Point(x, y)) < 150) {
				bin.at<uchar>(Point(x, y)) = 0;
			}
			else {
				bin.at<uchar>(Point(x, y)) = 255;
			}
		}
	}
	return bin;
}

void toFile(double area, double height, double width) {
	ofstream outfile;
	outfile.open("D:\\Downloads\\results.m", ios::app);
	outfile << area << ">>" <<height << ">>" <<width << endl;
	outfile.close();
}


Mat morphology_sky(Mat& bin) {
	Mat morph = Mat(bin.rows, bin.cols, CV_8U);

	Mat element1 = getStructuringElement(MORPH_ELLIPSE, Size(22, 22));
	morphologyEx(bin, morph, MORPH_ERODE, element1);
	//imshow("Morph", morph);
	
	Mat element2 = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
	morphologyEx(morph, morph, MORPH_DILATE, element2);
	//imshow("Morph2", morph);

	return morph;
}

Mat morphology_land(Mat& bin) {
	Mat morph = Mat(bin.rows, bin.cols, CV_8U);

	Mat element1 = getStructuringElement(MORPH_RECT, Size(2, 2));
	morphologyEx(bin, morph, MORPH_ERODE, element1);
	//imshow("Morph", morph);

	//Mat element2 = getStructuringElement(MORPH_RECT, Size(10, 10));
	//morphologyEx(morph, morph, MORPH_DILATE, element2);
	//imshow("Morph2", morph);

	return morph;
}


void stuff(Mat& morph){

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	///RETR_CCOMP - This flag retrieves all the contours and arranges them to a 2-level hierarchy.
	findContours(morph, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	///Find all contours within an image 
	for (int i = 0; i < contours.size(); i++) {
		double area = contourArea(contours[i]);
		if (area > 100000) {
			Mat blankImg = Mat(morph.rows, morph.cols, CV_8UC3);
			//Scalar color(rand() & 255, rand() & 255, rand() & 255);
			Scalar color(0, 0, 255);
			//drawContours(blankImg, contours, i, Scalar(0, 0, 255), 1);
			drawContours(blankImg, contours, i, color, CV_FILLED, 8, hierarchy);

			///Draw a rectangle around the found contour
			vector<vector<Point> > contours_poly(contours.size());
			vector<Rect> boundRect(contours.size());
			approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
			boundRect[i] = boundingRect(Mat(contours_poly[i]));
			Scalar color_rect(255, 0, 0);
			rectangle(blankImg, boundRect[i].tl(), boundRect[i].br(), color_rect, 1, 8, 0);

			string b = to_string(i);
			//imshow(b, blankImg);


			Mat output = Mat(boundRect[i].height - 2, boundRect[i].width - 2, CV_8UC3);
			for (int x = 0; x < output.cols; ++x) {
				for (int y = 0; y < output.rows; ++y) {
					output.at<Vec3b>(Point(x, y))[2] = blankImg.at<Vec3b>(Point(x + 1, y + 1))[2];
					output.at<Vec3b>(Point(x, y))[1] = blankImg.at<Vec3b>(Point(x + 1, y + 1))[1];
					output.at<Vec3b>(Point(x, y))[0] = blankImg.at<Vec3b>(Point(x + 1, y + 1))[0];
				}
			}

			Mat grays = grayscale(output);
			//imshow("OUT", grays);
			Mat binar = binary(grays);
			//imshow("BIN", binar);

			//imwrite("D:\\img\\contours\\"+a+"_"+b+"_contour.jpg", blankImg);
			vector<vector<Point>> contours_2;
			vector<Vec4i> hierarchy_2;

			///RETR_CCOMP - This flag retrieves all the contours and arranges them to a 2-level hierarchy.
			findContours(binar, contours_2, hierarchy_2, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
			Mat blankImg_2;
			for (int x = 0; x < contours_2.size(); x++) {
				double area_2 = contourArea(contours_2[x]);
				//Mat blankImg_2;

				///vector<vector<Point> > contours_poly_2(contours_2.size());
				///vector<Rect> boundRect_2(contours_2.size());
				///approxPolyDP(Mat(contours_2[x]), contours_poly_2[x], 3, true);
				///boundRect_2[x] = boundingRect(Mat(contours_poly_2[x]));
				Rect boundRect_2 = boundingRect(contours_2[x]);
				double height_2 = boundRect_2.height;
				double width_2 = boundRect_2.width;

				Scalar color_rect(255, 0, 0);
				rectangle(blankImg_2, boundRect_2.tl(), boundRect_2.br(), color_rect, 1, 8, 0);
				//double height_2 = boundRect_2[x].height;
				//double width_2 = boundRect_2[x].width;

				cout << "A: " << area_2 << endl;
				cout << "H: " << height_2 << endl;
				cout << "W: " << width_2 << endl;

				//toFile(k, area_2, height_2, width_2);

			}

			///Areas for objects in the sky
			//cout << "area" << i << " =" << area << endl;
			toFile(area, output.rows, output.cols);
		}
		///All areas
		//cout << "area"<<i<<" =" << area << endl;
	}
}

void land_stuff(Mat& morph) {

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	///RETR_CCOMP - This flag retrieves all the contours and arranges them to a 2-level hierarchy.
	findContours(morph, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	///Find all contours within an image
	Mat blankImg = Mat(morph.rows, morph.cols, CV_8UC3);

	for (int i = 0; i < contours.size(); i++) {
		double area = contourArea(contours[i]);
		if (area>400){
			//Scalar color(rand() & 255, rand() & 255, rand() & 255);
			Scalar color(0, 0, 255);
			//drawContours(blankImg, contours, i, Scalar(0, 0, 255), 1);
			drawContours(blankImg, contours, i, color, CV_FILLED, 8, hierarchy);

			///Draw a rectangle around the found contour
			vector<vector<Point> > contours_poly(contours.size());
			vector<Rect> boundRect(contours.size());
			
			/*
			vector<RotatedRect> minRect(contours.size());
			minRect[i] = minAreaRect(Mat(contours[i]));

			double height_2 = minRect[i].size[];
			double width_2 = boundRect_2.width;

			Point2f rect_points[4]; minRect[i].points(rect_points);
			for (int j = 0; j < 4; j++)
				line(blankImg, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
				*/


			approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
			boundRect[i] = boundingRect(Mat(contours_poly[i]));
			double width = boundRect[i].width;
			double height = boundRect[i].height;
			
			Scalar color_rect(255, 0, 0);
			rectangle(blankImg, boundRect[i].tl(), boundRect[i].br(), color_rect, 1, 8, 0);

			string b = to_string(i);
			//imshow(b, blankImg);

			//cout << "area" << i << " =" << area << endl;
			toFile(area, height, width);
		}

		///Areas for objects in the sky
		//cout << "area" << i << " =" << area << endl;
	}
	//imshow("Blank", blankImg);
}
