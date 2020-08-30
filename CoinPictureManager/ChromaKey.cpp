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

#include "ChromaKey.h"

const char *window_name = "Adjust Chroma Key";

//MAX and MIN values for alpha function, adjusted by trackbars
int ALPHA_MIN = 25;
int ALPHA_MAX = 75;

//Trackbar pointers
int alpha_min_slider = ALPHA_MIN;
int alpha_max_slider = ALPHA_MAX;

//Main image
Mat bgra; //Input image (BGRA format)

/*
 * Alpha mapping function for 0-510 color distance value to a 0-255 alpha value
 *
 * @param d Color distance value (2*color1 - color2 - color3)
 * @return Alpha value (unsigned char, or int of value 0-255)
 */
uchar alphaMap(int d) {
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

/*
 * Run chroma key function on supplied image (overwrites pixel values of given image)
 *
 * @param img Pointer to image to run chroma keying on
 */
void chromaKey(Mat *img) {
	uchar *bp = (*img).data;
	uchar *gp = bp + 1;
	uchar *rp = gp + 1;
	uchar *ap = rp + 1;

	for (int i = 0; i < (*img).cols*(*img).rows; i++) { //Iterate over each pixel in image
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
	//TODO add softening around edges 
}


/*
 * Update the current shown image
 */
void updateDisplay() {
	Mat img_show;
	Size newsize(900 * bgra.cols / bgra.rows, 900); //Resize image to a reasonable size
	resize(bgra, img_show, newsize);

	//std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();
	chromaKey(&img_show); //Run chroma key function
	//std::chrono::time_point<std::chrono::system_clock> end_time = std::chrono::system_clock::now(); //Calculate time elapsed and print
	//double elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	//std::cout << "Proccessing loop time: " << elapsed_ms << " ms" << std::endl;

	imshow(window_name, img_show); //Show final image
}

/*
 * Event handler for adjustments to the alpha min and max values
 *
 * @param sp Current val
 * @param val Pointer to other fields
 */
void onTrackbar(int sp, void *val) {
	if (alpha_min_slider < alpha_max_slider) {
		ALPHA_MIN = alpha_min_slider;
		ALPHA_MAX = alpha_max_slider;
	} else {
		alpha_min_slider = alpha_max_slider - 1;
		ALPHA_MIN = alpha_min_slider;
		ALPHA_MAX = alpha_max_slider;
	}
	updateDisplay();
}

/*
 * Run chroma keying on the image at the supplied filename and save to the supplied output filename, showing an OpenCV GUI to adjust the max and min alpha values
 *
 * @param filename Input filename
 * @param output_filename Output filename
 * @return Success code
 */
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

	chromaKey(&bgra);
	imwrite(output_filename, bgra);

	std::cout << "Image saved to " << output_filename << std::endl;

	return 0;
}