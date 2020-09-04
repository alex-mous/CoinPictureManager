/*
 * CropImages.cpp - Find the bounding box of the coin in the image and crop the image to it (user selection window for padding around coin)
 * This file is part of CoinPictureManager.
 *
 * Copyright (C) 2020  PolarPiBerry
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "CropImages.h"

//Global variables used in update routine
const char *crop_window_name = "Crop Image";
int x_slider; //Padding on bounding rectangle
int y_slider;
int w_slider;
int h_slider;
Mat img;
Rect bounding_box;

//Global var of the image size
Size newsize;


/*
 * Get the bounding box for the image
 *
 * @param img Input image
 * @param bounding_rect Pointer to a rectangle to store the resulting bounding box in
 */
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

	for (unsigned int i = 0; i < cnts.size(); i++) {
		double peri = arcLength(cnts[i], true);
		vector<Point> cnts_poly(1);
		approxPolyDP(cnts[i], cnts_poly, 0.0015 * peri, true);
		Rect rect = boundingRect(cnts_poly);
		if (rect.area() > (*bounding_rect).area()) {
			*bounding_rect = rect;
		}
	}
}

/*
 * Pad the bounding box given
 *
 * @param img Pointer to image where bounds are based
 * @param bounds Rectangle of the bounds
 * @param output Pointer to the output bound rectangle
 */
void padBounds(Mat *img, Rect bounds, Rect *output) {
	*output = bounds;
	(*output).x = x_slider;
	(*output).x = max((*output).x, 0);
	(*output).y = y_slider;
	(*output).y = max((*output).y, 0);
	(*output).width = min(max(w_slider, 10), (*img).cols-x_slider);
	(*output).height = min(max(h_slider, 10), (*img).rows-y_slider);
}

/*
 * Plot the bounding box supplied on the image supplied
 *
 * @param img_show Pointer to the image to display the bounding box on
 * @param bounding_rect Pointer to the bounding box
 */
void plotBounds(Mat *img_show, Rect *bounding_rect) {
	rectangle(*img_show, (*bounding_rect).tl(), (*bounding_rect).br(), Scalar(255, 255, 255), 5, 8, 0);
}

/*
 * Crop the image to the bounding rectangle supplied
 *
 * @param img Pointer to the image to crop
 * @param bounding_rect Pointer to the bounding rectangle to which the image is cropped
 */
void cropImage(Mat *img, Rect *bounding_rect) {
	Rect tmp = Rect((*bounding_rect).x, (*bounding_rect).y, (*bounding_rect).width, (*bounding_rect).height);
	*img = (*img)(tmp);
}

/*
 * Event handler for adjustments to the crop padding trackbar
 *
 * @param sp Current val
 * @param val Pointer to other fields
 */
void onCropTrackbar(int sp, void *val) {//Update the current shown image
	Mat img_show = img.clone();
	
	//Create a copy of the bounding box and adjust the padding based on the slider value
	Rect bounding_rect;
	padBounds(&img, bounding_box, &bounding_rect);

	plotBounds(&img_show, &bounding_rect);
	cropImage(&img_show, &bounding_rect);

	resize(img_show, img_show, newsize); 
	imshow(crop_window_name, img_show); //Show final image
}

/*
 * Crop the image at the supplied filename and save to the supplied output filename, showing an OpenCV GUI to adjust the padding
 *
 * @param filename Input filename
 * @param output_filename Output filename
 * @return Success code
 */
int cropImage(const char* filename, const char *output_filename) {
	img = imread(filename, IMREAD_COLOR);
	if (!img.data) {
		std::cout << "Error! " << filename << ": Unable to open image"<< std::endl;
		return 1;
	}

	//Set the displayed image size
	#ifdef _WIN32
		newsize = Size(GetSystemMetrics(SM_CYSCREEN) - 200, (GetSystemMetrics(SM_CYSCREEN) - 200) * img.rows / img.cols); //Resize image to a reasonable size for display
	#elif __linux__
		Display* d = XOpenDisplay(NULL);
		Screen*  s = DefaultScreenOfDisplay(d);
		newsize = new Size(s->width - 200, (s->height - 200) * img.rows / img.cols); //Resize image to a reasonable size for display
	#endif

	

	//Get bounding box
	getBounds(img, &bounding_box);
	x_slider = bounding_box.x;
	y_slider = bounding_box.y;
	w_slider = bounding_box.width;
	h_slider = bounding_box.height;

	namedWindow(crop_window_name, WINDOW_AUTOSIZE); //Create named window to place sliders and image upon

	createTrackbar("X", crop_window_name, &x_slider, img.cols, onCropTrackbar); //Create trackbars to adjust left, right, top and bottom padding
	createTrackbar("Y", crop_window_name, &y_slider, img.rows, onCropTrackbar);
	createTrackbar("Width", crop_window_name, &w_slider, img.cols, onCropTrackbar);
	createTrackbar("Height", crop_window_name, &h_slider, img.rows, onCropTrackbar);

	onCropTrackbar(0, 0);
	waitKey(0);

	//Crop and save final image
	Rect bounding_rect;
	padBounds(&img, bounding_box, &bounding_rect);
	cropImage(&img, &bounding_rect);
	imwrite(output_filename, img);

	return 0;
}