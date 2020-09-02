/*
 * ImageFunctions.cpp - Provides core image functions (creating thumbnails, creating WebP versions, etc) for CoinPictureManager
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

#include "ImageFunctions.h"

/*
 * Determine if file is an acceptable image format based on extension
 *
 * @param ext String of the file extension (case-insensitive)
 * @return boolean if file is an image
 */
boolean isImage(std::string ext) {
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	return ext==".jpg" || ext==".jpeg" || ext==".jpe" || ext==".jp2" || ext==".png";
}


/*
 * Create thumbnail images from the images in the given directory, up to a maximum of max_pics in the image (should be an even number of picures - obverse/reverse pairs). Saves to thumbnail.jpg in given directory
 *
 * @param image_dir Directory that the images are stored in
 * @param thumbnail_width Width (pixels) of the thumbnail image
 * @param max_pics Maximum number of pictures to show in the thumbnail
 */
int createThumbnail(fs::path image_dir, int thumbnail_height, int max_pics) {
	int c = 0;
	vector<Mat> pictures;
	int max_width[2]; int max_height = 0; //Maximum sizes of pictures

	for (auto &f : fs::directory_iterator(image_dir)) { //Read each image file into vector
		if (isImage(f.path().extension().string()) && (c < max_pics || max_pics < 1)) { //Only read JPG files and upto max_pics (but not if max_pics is less than 0)
			pictures.push_back(imread(f.path().string()));
			max_height = max(max_height, pictures[c].rows);
			max_width[c%2] = max(max_width[c % 2], pictures[c].cols);
			c++;
		}
	}

	if (c % 2 != 0) {
		std::cout << "Error! " << image_dir << ": Must be an even number of files in each folder (obverse/reverse pairs) or max_pics of >=1 (current number: " << c << ")" << std::endl;
		return 1;
	}

	int side = c / 2;
	int rows = round(sqrt(side));
	int cols = ceil(side / (float)rows);

	Mat3b thumbnail(rows*(max_height), cols*(max_width[0] + max_width[1])); //Calculate final image size
	for (int i = 0; i < 2; i++) {
		Mat3b temp(rows*max_height, cols*max_width[i], Vec3b(255, 255, 255));
		for (int row = 0; row < rows; row++) {
			int cols_left = (row == rows-1) ? (side - row*cols) : cols; //Number of pictures left - could be less than number of columns on last row
			int extra_x_padding = (cols_left == cols) ? 0 : (max_width[i]*(cols-cols_left)/(2*cols_left)); //Calculate extra padding on X if there are less pictures than expected
			for (int col = 0; col < cols_left; col++) {
				//Pad the picture with a border so they are all spaced evenly
				int index = 2 * (row*cols + col) + i; //Picture index (even pictures)
				int py = (max_height - pictures.at(index).rows);
				int px = (max_width[i] - pictures.at(index).cols);
				pictures.at(index).copyTo(temp(Rect(
					col*max_width[i] + px/2 + extra_x_padding*(col+1),
					row*max_height + py/2,
					max_width[i] - px,
					max_height - py))); //Copy the current image to the correct location in the final image
			}
		}
		temp.copyTo(thumbnail(Rect(i*cols*max_width[0], 0, temp.cols, temp.rows))); //Copy the image to the thumbnail
	}
	Mat3b res;
	resize(thumbnail, res, Size(thumbnail_height * thumbnail.cols / thumbnail.rows, thumbnail_height));
	imwrite(image_dir.append("thumbnail.jpg").string(), res);
	return 0;
}

/*
 * Create WebP images from the images in the given directory
 *
 * @param image_dir Directory that the images are stored in
 * @param quality WebP image quality (0-100)
 * @param verbose Verbose
 */
int createWebp(fs::path image_dir, int quality, boolean verbose) {
	vector<int> params;
	params.push_back(IMWRITE_WEBP_QUALITY);
	params.push_back(quality);
	for (auto &f : fs::directory_iterator(image_dir)) { //Each image file
		if (isImage(f.path().extension().string())) {
			if (verbose) std::cout << "\t\tCreating WebP image for " << f.path().filename() << std::endl;
			Mat img = imread(f.path().string());
			fs::path img_out = image_dir / (f.path().stem().string() + ".webp");
			imwrite(img_out.string(), img, params);
		}
	}
	return 0;
}