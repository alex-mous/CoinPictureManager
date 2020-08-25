/*
 * ChromaKey.cpp - Run chroma keying/blue screening functions on images using OpenCV (user selection window to adjust max and min values for chroma keying function)
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

#include <iostream>
#include <chrono>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "wtypes.h"
#include "ChromaKey.h"

using namespace cv;
using namespace std;

const char *window_name = "Adjust Chroma Key";

//MAX and MIN values for alpha function, adjusted by trackbars
int ALPHA_MIN = 25;
int ALPHA_MAX = 75;

//Trackbar pointers
int alpha_min_slider = ALPHA_MIN;
int alpha_max_slider = ALPHA_MAX;

//Main image
Mat bgra; //Input image (BGRA format)

uchar alphaMap(int d) { //Alpha mapping function for 0-510 color distance to 0-255 alpha value
	if (d < ALPHA_MIN) {
		return 255;
	}
	else if (d > ALPHA_MAX) {
		return 0;
	}
	else {
		return 255 * (1 - (d - ALPHA_MIN) / (ALPHA_MAX - ALPHA_MIN));
	}
}

void chromaKey(Mat img) { //Run chroma key function on <img> (warning: overwrites pixel values)
	uchar *bp = img.data;
	uchar *gp = bp + 1;
	uchar *rp = gp + 1;
	uchar *ap = rp + 1;

	for (int i = 0; i < img.cols*img.rows; i++) { //Iterate over each pixel in image
		if (*bp >= *gp && *bp >= *rp) {
			*ap = alphaMap(2 * *bp - (*gp + *rp));
			*bp = (255 - *ap) + *bp*(*ap) / 255;
			*gp = (255 - *ap) + *gp*(*ap) / 255;
			*rp = (255 - *ap) + *rp*(*ap) / 255;
		}
		else {
			*ap = 255; //Default full opacity - no need to recalculate other pixels
		}
		bp += 4; gp += 4; rp += 4; ap += 4;
	}
}

void updateDisplay() { //Update the current shown image
	Mat img_show;
	Size newsize(1200, 1200 * bgra.rows / bgra.cols); //Resize image to a reasonable size
	resize(bgra, img_show, newsize);

	std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();
	chromaKey(img_show); //Run chroma key function
	std::chrono::time_point<std::chrono::system_clock> end_time = std::chrono::system_clock::now(); //Calculate time elapsed and print
	double elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::cout << "Proccessing loop time: " << elapsed_ms << " ms" << std::endl;

	imshow(window_name, img_show); //Show final image
}

void onTrackbar(int sp, void *val) {
	ALPHA_MIN = alpha_min_slider;
	ALPHA_MAX = alpha_max_slider;
	updateDisplay();
}

int chromaKeyInterface(const char* filename, const char *output_filename) {
	Mat image;
	image = imread(filename, IMREAD_COLOR);
	if (!image.data) {
		std::cout << "Unable to open image " << filename << std::endl;
		return 1;
	}

	cvtColor(image, bgra, COLOR_BGR2BGRA);

	namedWindow(window_name, WINDOW_AUTOSIZE); //Create named window to place sliders and image upon

	createTrackbar("Alpha Min", window_name, &alpha_min_slider, 510, onTrackbar); //Create trackbars to adjust alpha min and max values
	createTrackbar("Alpha Max", window_name, &alpha_max_slider, 510, onTrackbar);

	onTrackbar(0, 0);
	waitKey(0);

	std::cout << "Saving image..." << std::endl;

	chromaKey(bgra);
	imwrite(output_filename, bgra);

	std::cout << "Image saved to " << output_filename << std::endl;

	return 0;
}