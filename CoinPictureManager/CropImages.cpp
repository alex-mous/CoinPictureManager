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
int padding_slider = 50; //Padding on bounding rectangle
Mat img;
Rect bounding_box;

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

/*
 * Pad the bounding box given
 *
 * @param img Pointer to image where bounds are based
 * @param bounds Rectangle of the bounds
 */
void padBounds(Mat *img, Rect bounds, int padding, Rect *output) {
	*output = bounds;
	(*output).x -= padding_slider;
	(*output).x = max((*output).x, 0);
	(*output).y -= padding_slider;
	(*output).y = max((*output).y, 0);
	(*output).width += 2 * padding_slider;
	(*output).width = min((*output).width, (*img).cols);
	(*output).height += 2 * padding_slider;
	(*output).height = min((*output).height, (*img).rows);
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
	std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();
	Mat img_show = img.clone();
	
	//Create a copy of the bounding box and adjust the padding based on the slider value
	Rect bounding_rect;
	padBounds(&img, bounding_box, padding_slider, &bounding_rect);

	plotBounds(&img_show, &bounding_rect);
	cropImage(&img_show, &bounding_rect);

	Size newsize(GetSystemMetrics(SM_CYSCREEN)-200, (GetSystemMetrics(SM_CYSCREEN)-200) * img_show.rows / img_show.cols); //Resize image to a reasonable size for display
	resize(img_show, img_show, newsize); 
	std::chrono::time_point<std::chrono::system_clock> end_time = std::chrono::system_clock::now(); //Calculate time elapsed and print
	double elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::cout << "Proccessing loop time: " << elapsed_ms << " ms" << std::endl;
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
	padBounds(&img, bounding_box, padding_slider, &bounding_rect);
	cropImage(&img, &bounding_rect);
	imwrite(output_filename, img);

	std::cout << "Image saved to " << output_filename << std::endl;

	return 0;
}