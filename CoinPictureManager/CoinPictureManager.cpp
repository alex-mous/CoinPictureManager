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

#include <iostream>
#include <filesystem>
#include <string>

#include "ChromaKey.h"
#include "Crop.h"



using namespace std;

namespace fs = std::experimental::filesystem;

const char *helpStr = "PictureManager\n\
Manage and prepare coin pictures \n\
\n\
Available commands: \n\
\t1: renaming files to sequential numbers \n\
\t2: create thumbnails (only first two images) \n\
\t3: create WebP copies of all JPG images \n\
\t4: create thumbnails with all images in each folder \n\
\t5: run greenscreening (and automatic cropping) on all images in each folder \n\
\t6: crop all images in each folder to the coin \n\
\t7: compile all images to Output folder\n\n\n";


const char *verifyStr = "Files MUST be organized as follows : \n\
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
	cout << "Please enter a command (0 to exit): ";
	cin >> sel;
	return sel;
}

/*
 * Run the given command
 *
 * @param command Integer command as defined in getSelection
 * @return success code
 */

int runCommand(int command) {
	fs::path path("../Debug");
	switch (command) {
		case 1:
			cout << "Renaming files in subdirectories" << endl;
			
			cout << fs::exists(path) << endl;
			return 0;
		default:
			cout << "Command not recognized" << endl;
			return 1;
	}
}

bool verifyPrompt() {
	cout << verifyStr;
	char res;
	while (1) {
		cin >> res;
		if (res == 'y' || res == 'Y') {
			return true;
		} else if (res == 'n' || res == 'N') {
			return false;
		} else {
			cout << "Please enter either y or n: ";
		}
	}
}

int main() {
	/*if (argc < 2) {
		std::cout << "Usage: display_image IMAGE_TO_LOAD" << std::endl;
		return 1;
	}
	//chromaKeyInterface(argv[1], "output.jpg");
	cropImage(argv[1], "cropped.jpg");*/
	std::cout << helpStr;
	bool verify = verifyPrompt();
	if (verify) {
		while (1) {
			int select = getSelection();
			if (select != '0') {
				int sel = select - '0';
				runCommand(sel);
			} else {
				cout << "Exiting..." << endl;
				break;
			}
		}
	} else {
		cout << "Please verify this file is in the correct directory before continuing";
	}
	return 0;
}
