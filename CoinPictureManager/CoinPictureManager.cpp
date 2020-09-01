/*
 * CoinPictureManager.cpp - Handles main interaction with the user (creates and runs a console window)
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

#include "Dependencies.h"
#include "ChromaKey.h"
#include "CropImages.h"
#include "ImageFunctions.h"

std::string console_help_str = "\
Manage and prepare coin pictures located in subdirectories of DIRECTORY\
\n\
Usage:\n\
\tcoinpicturemanager [DIRECTORY] [-h] [-i] [-v] [-c|g|t|s|w|r]\n\
\n\
Options:\n\
\t-h\t\tPrint this help\n\
\t-i\t\tInteractive mode (default unless other option specified)\n\
\t-v\t\tVerbose mode\n\
\t-c\t\tCrop images (GUI required)\n\
\t-g\t\tChroma Key images (GUI required)\n\
\t-t\t\tCreate thumbnails\n\
\t-s\t\tCreate thumbnails from the first two images only\n\
\t-w\t\tCreate WebP images\n\
\t-r\t\tRename files to sequential numbers\n\
";

std::string help_str = "PictureManager\nManage and prepare coin pictures\n\
\nRuns the below commands on the images in the subdirectories of the specified directory (\"Images\" by default)\n\
\nAvailable commands: \n\t1: renaming files to sequential numbers\n\
\t2: create thumbnails from the first two images\n\t3: create thumbnails with all images\n\
\t4: create WebP images\n\t5: run chroma keying \n\
\t6: crop each image \n\th: show this help\n\n\n";

std::string verify_str = "Files MUST be organized as follows : \n\
/ This directory \n\
\tThis File.exe \n\
\t/ Images \n\
\t\t/ DIR1 \n\
\t\t\tPIC1.jpg \n\
\t\t\t... \n\
\t\t\tPICN.jpg \n\
\t\t/ ... \n\
\t\t/ DIRN \n\
\t\t\tPIC1.jpg \n\
\t\t\t... \n\
\t\t\tPICN.jpg\n";

/*
 * Rename the files in subdirectories of root_dir to sequential numbers
 *
 * @param root_dir Top directory (to search below)
 * @param verbose Verbose
 */
int renameFiles(fs::path root_dir, boolean verbose) {
	std::cout << "Renaming files in subdirectories" << std::endl;
	for (auto &d : fs::directory_iterator(root_dir)) { //Each sub-directory
		if (fs::is_directory(d)) {
			int f_no = 0;
			if (verbose) std::cout << "\tDirectory: " << d.path().filename() << std::endl;
			for (auto &f : fs::directory_iterator(d)) { //Each image file
				if (f.path().extension() == ".JPG" || f.path().extension() == ".jpg") {
					std::string name = std::to_string(f_no);
					name.insert(name.begin(), 4 - name.length(), '0');
					name = name + ".jpg";
					fs::path new_path = d.path() / name;
					fs::rename(f, new_path);
					if (verbose) std::cout << "\t\tRenaming " << f.path().filename() << " to " << name << std::endl;
					f_no++;
				}
			}
		}
	}
	return 0;
}

/*
 * Create thumbnail images in subdirectories of root_dir
 *
 * @param root_dir Top directory (to search below)
 * @param max_imgs Maximum number of images to use (-1 for all)
 * @param verbose Verbose
 */
int createThumbnail(fs::path root_dir, int max_imgs, boolean verbose) {
	std::cout << "Creating thumbnail files in subdirectories (up to two images)..." << std::endl;
	for (auto &d : fs::directory_iterator(root_dir)) { //Each sub-directory
		if (fs::is_directory(d)) {
			if (verbose) std::cout << "\tDirectory: " << d.path().filename() << std::endl;
			createThumbnail(d.path(), 250, max_imgs);
		}
	}
	return 0;
}

/*
 * Create WebP images in subdirectories of root_dir
 *
 * @param root_dir Top directory (to search below)
 * @param verbose Verbose
 */
int createWebp(fs::path root_dir, boolean verbose) {
	std::cout << "Creating WebP images..." << std::endl;
	for (auto &d : fs::directory_iterator(root_dir)) { //Each sub-directory
		if (fs::is_directory(d)) {
			if (verbose) std::cout << "\tDirectory: " << d.path().filename() << std::endl;
			createWebp(d.path(), 50);
		}
	}
	return 0;
}

/*
 * Run chroma keying on images in subdirectories of root_dir
 *
 * @param root_dir Top directory (to search below)
 * @param verbose Verbose
 */
int chromaKey(fs::path root_dir, boolean verbose) {
	std::cout << "Running chroma keying..." << std::endl;
	for (auto &d : fs::directory_iterator(root_dir)) { //Each sub-directory
		if (fs::is_directory(d)) {
			if (verbose) std::cout << "\tDirectory: " << d.path().filename() << std::endl;
			for (auto &f : fs::directory_iterator(d)) { //Each image file
				if (f.path().extension() == ".JPG") {
					if (verbose) std::cout << "\t\tChroma keying image: " << f.path().string() << std::endl;
					chromaKeyInterface(f.path().string().c_str(), f.path().string().c_str());
				}
			}
		}
	}
	return 0;
}

/*
 * Run cropping on images in subdirectories of root_dir
 *
 * @param root_dir Top directory (to search below)
 * @param verbose Verbose
 */
int cropImages(fs::path root_dir, boolean verbose) {
	std::cout << "Cropping images..." << std::endl;
	for (auto &d : fs::directory_iterator(root_dir)) { //Each sub-directory
		if (fs::is_directory(d)) {
			if (verbose) std::cout << "\tDirectory: " << d.path().filename() << std::endl;
			for (auto &f : fs::directory_iterator(d)) { //Each image file
				if (f.path().extension() == ".JPG") {
					if (verbose) std::cout << "\t\tCropping image: " << f.path().string() << std::endl;
					cropImage(f.path().string().c_str(), f.path().string().c_str());
				}
			}
		}
	}
	return 0;
}

/*
 * Run the given command
 *
 * @param command Integer command as defined in getSelection
 * @return success code
 */
int runCommand(int command, boolean verbose, fs::path root_dir) {
	switch (command) {
		case '1':
			return renameFiles(root_dir, verbose);
		case '2':
			return createThumbnail(root_dir, 2, verbose);
		case '3':
			return createThumbnail(root_dir, -1, verbose);
		case '4':
			return createWebp(root_dir, verbose);
		case '5':
			return chromaKey(root_dir, verbose);
		case '6':
			return cropImages(root_dir, verbose);
		case 'h':
			std::cout << help_str;
		case 'q':
			std::cout << "Exiting..." << std::endl;
			return -1;
		default:
			std::cout << "Command not recognized. Please try again" << std::endl;
			return 1;
	}
}

/*
 * Run the interactive user interface to run commands
 *
 * @return status code
 */
int runUI(fs::path root_dir) {
	std::cout << help_str;
	std::cout << verify_str;
	while (1) {
		char sel;
		std::cout << "Please enter a command: ";
		std::cin >> sel;
		int status = runCommand(sel, true, root_dir);
		if (status == -1) { //Exit
			break;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	fs::path root_dir = fs::path("./Input");
	if (argc > 1) { //Parse commands
		std::vector<char> opts;
		for (int i = 1; i < argc; i++) {
			if (argv[i][0] == '-') { //General option
				std::cout << "Option selected: " << argv[i][1] << std::endl;
				std::cout << argv[i][1] << std::endl;
				opts.push_back(argv[i][1]);
			} else if (i == 1) { //Second arg should be directory (if any)
				root_dir = fs::path(argv[i]);
				if (!fs::is_directory(root_dir)) {
					std::cout << "Directory \"" << argv[i] << "\" does not exist";
					return 1;
				}
			}
		}
	} else {
		//Interactive mode
		runUI(root_dir);
	}


	return 0;
}
