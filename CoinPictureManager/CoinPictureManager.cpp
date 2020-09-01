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

std::string helpStr = "PictureManager\nManage and prepare coin pictures \n\
\nAvailable commands: \n\t1: renaming files to sequential numbers in each folder \n\
\t2: create thumbnails from the first two images in each folder \n\t3: create thumbnails with all images in each folder \n\
\t4: create WebP copies of all JPG images \n\t5: run greenscreening (if cropping, run 6 first) on all images in each folder \n\
\t6: crop all images in each folder to the coin \n\n\n";

std::string verifyStr = "Files MUST be organized as follows : \n\
/ This directory \n\
\tThis File.exe \n\
\t/ Input \n\
\t\t/ DIR1 \n\
\t\t\tPIC1.jpg \n\
\t\t\t... \n\
\t\t\tPICN.jpg \n\
\t\t/ ... \n\
\t\t/ DIRN \n\
\t\t\tPIC1.jpg \n\
\t\t\t... \n\
\t\t\tPICN.jpg \n\
\t/ Output\n\
\t\tOUTPUT_DIRS_AND_PICS\n\
\n\n\
Is this file in the correct location? (y/n) ";

/*
 * Get a command from the user and return it as an integer
 *
 * @return user command (-1 if invalid)
 */
int getSelection() {
	char sel;
	std::cout << "Please enter a command: ";
	std::cin >> sel;
	return sel;
}

/*
 * Run the given command
 *
 * @param command Integer command as defined in getSelection
 * @return success code
 */
int runCommand(int command) {
	fs::path path("./Input");
	switch (command) {
		case 1:
			std::cout << "Renaming files in subdirectories" << std::endl;
			for (auto &d : fs::directory_iterator(path)) { //Each sub-directory
				if (fs::is_directory(d)) {
					int f_no = 0;
					for (auto &f : fs::directory_iterator(d)) { //Each image file
						if (f.path().extension() == ".JPG" || f.path().extension() == ".jpg") {
							std::string name = std::to_string(f_no);
							name.insert(name.begin(), 4 - name.length(), '0');
							name = name + ".jpg";
							fs::path new_path = d.path() / name;
							fs::rename(f, new_path);
							std::cout << "Renaming JPG file " << f.path().filename() << " to " << name << std::endl;
							f_no++;
						}
					}
				}
			}
			return 0;
		case 2:
			std::cout << "Creating thumbnail files in subdirectories (up to two images)..." << std::endl;
			for (auto &d : fs::directory_iterator(path)) { //Each sub-directory
				if (fs::is_directory(d)) {
					std::cout << "Directory: " << d.path().filename() << std::endl;
					createThumbnail(d.path(), 250, 2);
				}
			}
			return 0;
		case 3:
			std::cout << "Creating thumbnail files in subdirectories..." << std::endl;
			for (auto &d : fs::directory_iterator(path)) { //Each sub-directory
				if (fs::is_directory(d)) {
					std::cout << "Directory: " << d.path().filename() << std::endl;
					createThumbnail(d.path(), 250, -1);
				}
			}
			return 0;
		case 4:
			std::cout << "Creating WebP images..." << std::endl;
			for (auto &d : fs::directory_iterator(path)) { //Each sub-directory
				if (fs::is_directory(d)) {
					std::cout << "Directory: " << d.path().filename() << std::endl;
					createWebp(d.path(), 50);
				}
			}
			return 0;
		case 5:
			for (auto &d : fs::directory_iterator(path)) { //Each sub-directory
				if (fs::is_directory(d)) {
					for (auto &f : fs::directory_iterator(d)) { //Each image file
						if (f.path().extension() == ".JPG") {
							std::cout << "Running chroma keying on JPG file " << f.path().string() << std::endl;
							chromaKeyInterface(f.path().string().c_str(), f.path().string().c_str());
						}
					}
				}
			}
			return 0;
		case 6:
			for (auto &d : fs::directory_iterator(path)) { //Each sub-directory
				if (fs::is_directory(d)) {
					for (auto &f : fs::directory_iterator(d)) { //Each image file
						if (f.path().extension() == ".JPG") {
							std::cout << "Cropping " << f.path().string() << "..." << std::endl;
							cropImage(f.path().string().c_str(), f.path().string().c_str());
						}
					}
				}
			}
			return 0;
		default:
			std::cout << "Command not recognized" << std::endl;
			return 1;
	}
}

/*
 * Verify the file structure with the user
 *
 * @return boolean based on if user accepts or denys
 */
bool verifyPrompt() {
	std::cout << verifyStr;
	char res;
	while (1) {
		std::cin >> res;
		if (res == 'y' || res == 'Y') {
			return true;
		} else if (res == 'n' || res == 'N') {
			return false;
		} else {
			std::cout << "Please enter either y or n: ";
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc == 1) { //Run interface
		std::cout << helpStr;
		bool verify = verifyPrompt();
		if (verify) {
			while (1) {
				int select = getSelection();
				int sel = select - '0';
				runCommand(sel);
			}
		}
		else {
			std::cout << "Please verify this file is in the correct directory before continuing";
		}
	} else { //Run function requested
		char opt;
		for (int i = 0; i < argc; i++) {
			if (argv[i] == "-") {
				std::cout << "Parameter\n";
			} else {
				std::cout << argv[i] << std::endl;
			}
		}
	}
	return 0;
}
