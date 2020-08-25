// Crop.cpp : This file contains the cropping functions
//

#include <iostream>
#include <chrono>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "windows.h"
#include "Crop.h"

using namespace cv;
using namespace std;

const char *crop_window_name = "Crop Image";

Mat img; //Original image (BGR)

int padding_slider = 50; //Padding on bounding rectangle
Rect bounding_box;

void getBounds(Mat img, Rect *bounding_rect) {
	Mat img_hsv;
	cvtColor(img, img_hsv, COLOR_BGR2HSV); //Convert to HSV colorspace for getBounds function
	Mat canny;
	vector<Mat> img_h;
	split(img_hsv, img_h);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(img_h[0], img_h[0], kernel, Point(-1, -1), 4);
	Canny(img_h[0], canny, 100, 200);
	dilate(canny, canny, kernel);

	vector<vector<Point>> cnts;
	findContours(canny, cnts, RETR_TREE, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < cnts.size(); i++) {
		float peri = arcLength(cnts[i], true);
		vector<Point> cnts_poly(1);
		approxPolyDP(cnts[i], cnts_poly, 0.0015 * peri, true);
		Rect rect = boundingRect(cnts_poly);
		if (rect.area() > (*bounding_rect).area()) {
			*bounding_rect = rect;
		}
	}
}

void padBounds(Rect bounds, int padding, Rect *output) {
	*output = bounds;
	(*output).x -= padding_slider;
	(*output).y -= padding_slider;
	(*output).width += 2 * padding_slider;
	(*output).height += 2 * padding_slider;
	//TODO: ensure sizes are within limits (>=0)
}

void plotBounds(Mat *img_show, Rect *bounding_rect) {
	rectangle(*img_show, (*bounding_rect).tl(), (*bounding_rect).br(), Scalar(255, 255, 255), 5, 8, 0);
}

void cropImage(Mat *img, Rect *bounding_rect) {
	Rect tmp = Rect(max((*bounding_rect).x, 0), max((*bounding_rect).y, 0), min((*bounding_rect).width, (*img).cols), min((*bounding_rect).height, (*img).rows));
	*img = (*img)(tmp);
}

void onCropTrackbar(int sp, void *val) {//Update the current shown image
	std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();
	Mat img_show = img.clone();
	
	//Create a copy of the bounding box and adjust the padding based on the slider value
	Rect bounding_rect;
	padBounds(bounding_box, padding_slider, &bounding_rect);

	plotBounds(&img_show, &bounding_rect);
	cropImage(&img_show, &bounding_rect);

	Size newsize(GetSystemMetrics(SM_CYSCREEN)-200, (GetSystemMetrics(SM_CYSCREEN)-200) * img_show.rows / img_show.cols); //Resize image to a reasonable size for display
	resize(img_show, img_show, newsize); 
	std::chrono::time_point<std::chrono::system_clock> end_time = std::chrono::system_clock::now(); //Calculate time elapsed and print
	double elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::cout << "Proccessing loop time: " << elapsed_ms << " ms" << std::endl;
	imshow(crop_window_name, img_show); //Show final image
}

int cropImage(const char* filename, const char *output_filename) {
	img = imread(filename, IMREAD_COLOR);
	if (!img.data) {
		std::cout << "Unable to open image " << filename << std::endl;
		return 1;
	}

	//Get bounding box
	getBounds(img, &bounding_box);

	namedWindow(crop_window_name, WINDOW_AUTOSIZE); //Create named window to place sliders and image upon

	createTrackbar("Adjust boundary", crop_window_name, &padding_slider, 200, onCropTrackbar); //Create trackbars to adjust alpha min and max values

	onCropTrackbar(0, 0);
	waitKey(0);

	std::cout << "Saving image..." << std::endl;

	//Crop and save final image
	Rect bounding_rect;
	padBounds(bounding_box, padding_slider, &bounding_rect);
	cropImage(&img, &bounding_rect);
	imwrite(output_filename, img);

	std::cout << "Image saved to " << output_filename << std::endl;

	return 0;
}