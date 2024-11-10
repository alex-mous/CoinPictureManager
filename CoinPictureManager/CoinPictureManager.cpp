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

#define DEFAULT_PATH "./Public"

std::string info_str = "Manage and prepare coin pictures located in subdirectories of DIRECTORY";

std::string console_usage_str = "\
\n\
Usage:\n\
\tcoinpicturemanager [DIRECTORY] [OPTIONS]\n\
\n\
Options:\n\
\t-h\t\tPrint this help\n\
\t-i\t\tInteractive mode (default unless other option specified)\n\
\t-v\t\tVerbose mode\n\
\t-c=COMMANDS\t\tRun command(s) (commands run in order listed; see available commands below)\n\
\n\
Commands:\n\
\t1\t\tRename files to sequential numbers\n\
\t2\t\tCreate thumbnails\n\
\t3\t\tCreate thumbnails from the first two images only\n\
\t4\t\tCreate WebP images\n\
\t5\t\tChroma Key images (GUI required)\n\
\t6\t\tCrop images (GUI required)\n\
";

std::string help_str = "-------------- PictureManager --------------\n----- Manage and prepare coin pictures -----\n\n\
------- Welcome to interactive mode  -------\n\n\
Run commands (enter l for a list or h for help)\n";

std::string command_str = "\nAvailable commands: \n\t1: renaming files to sequential numbers\n\
\t2: create thumbnails with all images\n\t3: create thumbnails from the first two images\n\
\t4: create WebP images\n\t5: run chroma keying \n\
\t6: crop each image \n\tl: show this list\n\tl: show help\n\tq: quit\n\n\n";

std::string verify_str = "Files MUST be organized as follows : \n\
/ This directory \n\
\tThis File.exe \n\
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
int renameFiles(fs::path root_dir, bool verbose) {
	std::cout << "Renaming files in subdirectories" << std::endl;
	for (auto &d: fs::directory_iterator(root_dir)) { //Each sub-directory
		if (fs::is_directory(d)) {
			int f_no = 0;
			if (verbose) std::cout << "\tDirectory: " << d.path().filename() << std::endl;
			for (auto &f: fs::directory_iterator(d)) { //Each image file
				if (isImage(f.path().extension().string())) {
					std::string name = std::to_string(f_no);
					name.insert(name.begin(), 4 - name.length(), '0');
					name = name + f.path().extension().string();
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
int createThumbnail(fs::path root_dir, int max_imgs, bool verbose) {
	if (max_imgs > 0) {
		std::cout << "Creating thumbnail files in subdirectories with a maximum number of pictures " << max_imgs << "..." << std::endl;
	} else {
		std::cout << "Creating thumbnail files in subdirectories..." << std::endl;
	}
	for (auto &d: fs::directory_iterator(root_dir)) { //Each sub-directory
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
int createWebp(fs::path root_dir, bool verbose) {
	std::cout << "Creating WebP images..." << std::endl;
	for (auto &d : fs::directory_iterator(root_dir)) { //Each sub-directory
		if (fs::is_directory(d)) {
			if (verbose) std::cout << "\tDirectory: " << d.path().filename() << std::endl;
			createWebp(d.path(), 50, verbose);
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
int chromaKey(fs::path root_dir, bool verbose) {
	std::cout << "Running chroma keying..." << std::endl;
	for (auto &d: fs::directory_iterator(root_dir)) { //Each sub-directory
		if (fs::is_directory(d)) {
			if (verbose) std::cout << "\tDirectory: " << d.path().filename() << std::endl;
			for (auto &f : fs::directory_iterator(d)) { //Each image file
				if (isImage(f.path().extension().string())) {
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
int cropImages(fs::path root_dir, bool verbose) {
	std::cout << "Cropping images..." << std::endl;
	for (auto &d: fs::directory_iterator(root_dir)) { //Each sub-directory
		if (fs::is_directory(d)) {
			if (verbose) std::cout << "\tDirectory: " << d.path().filename() << std::endl;
			for (auto &f : fs::directory_iterator(d)) { //Each image file
				if (isImage(f.path().extension().string())) {
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
int runCommand(char command, bool verbose, bool interactive_mode, fs::path root_dir) {
	switch (command) {
		case '1':
			return renameFiles(root_dir, verbose);
		case '2':
			return createThumbnail(root_dir, -1, verbose);
		case '3':
			return createThumbnail(root_dir, 2, verbose);
		case '4':
			return createWebp(root_dir, verbose);
		case '5':
			return chromaKey(root_dir, verbose);
		case '6':
			return cropImages(root_dir, verbose);
		case 'l':
			if (interactive_mode) {
				std::cout << command_str;
				return 0;
			}
		case 'h':
			if (interactive_mode) {
				std::cout << help_str << std::endl << command_str << std::endl << verify_str;
				return 0;
			}
		case 'q':
			if (interactive_mode) {
				return -1;
			}
		default:
			std::cout << "Command \"" << command << "\" not recognized. Please try again" << std::endl;
			return 1;
	}
}

/*
 * Run the interactive user interface to run commands
 *
 * @return status code
 */
int runUI(fs::path root_dir, bool verbose) {
	std::cout << help_str;
	while (1) {
		char sel;
		std::cout << ">>> ";
		std::cin >> sel;
		int status = runCommand(sel, verbose, true, root_dir);
		if (status == -1) { //Exit
			break;
		}
	}
	return 0;
}

int main(int argc, char **argv) { //Main loop - parse any command line options and run either command or interactive mode
	bool run_ui = false, verbose = false;
	std::vector<char> commands;
	fs::path root_dir = fs::path(DEFAULT_PATH);

	//Parse the arguments
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && strlen(argv[i]) >= 2) { //General option
			if (argv[i][1] == 'h') { //Show help
				std::cout << info_str << std::endl << console_usage_str;
				return 0;
			} else if (argv[i][1] == 'i') { //Interactive mode
				run_ui = true;
			} else if (argv[i][1] == 'v') { //Verbose mode
				verbose = true;
			} else if (argv[i][1] == 'c') { //Run command
				if (strlen(argv[i]) > 3 && argv[i][2] == '=') {
					for (unsigned int c = 3; c < strlen(argv[i]); c++) {
						commands.push_back(argv[i][c]);
					}
				} else {
					std::cout << "Please enter commands in the format -c=[COMMAND1][COMMAND2][COMMAND3]" << std::endl << std::endl;
					std::cout << console_usage_str;
					return 1;
				}
			} else {
				std::cout << "Option \"" << argv[i] << "\" not recognized" << std::endl << std::endl;
				std::cout << console_usage_str;
				return 1;
			}
		} else if (i == 1) { //Second arg should be directory (if any)
			root_dir = fs::path(argv[i]);
			if (!fs::is_directory(root_dir)) {
				std::cout << "Directory \"" << argv[i] << "\" does not exist" << std::endl << std::endl;
				std::cout << console_usage_str;
				return 1;
			}
		}
	}

	//Run any commands
	if (commands.size() > 0) {
		for (unsigned int i = 0; i < commands.size(); i++) {
			char comm = commands.at(i);
			int status = runCommand(comm, verbose, false, root_dir);
			if (status > 0) return 1; //Exit upon error
		}

		if (run_ui) { //Enter interactive mode if specified
			std::cout << "Entering interactive mode..." << std::endl;
			return runUI(root_dir, verbose);
		} else {
			return 0;
		}
	}

	//If no commands, enter interactive mode
	runUI(root_dir, true);
}
