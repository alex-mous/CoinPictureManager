# CoinPictureManager
### Console application to manage and prepare coin pictures, including cropping, blue screening, renaming to sequential numbers, thumbnail creation, and more.

## Installation
### Dependencies:
-  OpenCV
	- **Windows Installation**
		- Download the version used from GitHub: ([download the opencv-4.4.0-vc14_vc15.exe file)](https://github.com/opencv/opencv/releases/tag/4.4.0))
		- Next, run the EXE file - make note of the directory where the files are extracted to (such as C:\opencv)
		- Open a Command Prompt window in "Run as administrator" mode and run the following commands (replacing YOUR_OPENCV_PATH with the path to where you extracted OpenCV to):
			- for 32-bit Windows:
			`setx -m OPENCV_DIR C:\YOUR_OPENCV_PATH\opencv\build\x86\vc15`
			- or, if you have 64-bit Windows
			`setx -m OPENCV_DIR C:\YOUR_OPENCV_PATH\opencv\build\x64\vc15`
		- Then, add the variable to your path. Run the following command in the same Command Prompt window:
			- `SET PATH=%PATH%;%OPENCV_DIR%\bin`
	- **Linux Installation**
		- Follow the instructions [on the OpenCV website](https://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html#linux-installation)

### Windows
- For the latest stable version, please see the releases tab and download the EXE file.
- Next, either place this file in the directory where you want to manage pictures or place it in a safe directory and add the variable to the path as above so it can be run globally (run the command `SET PATH=%PATH%;c:\PATH\TO\YOUR\EXE`)
- *To download the latest code, clone this repository and open the project in Visual Studio*
### Linux
*To be added*
## Usage

`coinpicturemanager [DIRECTORY] [OPTIONS]`

*Without a specified directory, the app uses the current directory. Without any options or when run as a desktop app on Windows, it will run in verbose interactive mode.*

### Options
	-h			Print this help
	-i			Interactive mode (default unless other option specified)
	-v			Verbose mode
	-c=COMMANDS	Run command(s) (commands run in order listed; see available commands below)

### Commands
	1		Rename files to sequential numbers
	2		Create thumbnails
	3		Create thumbnails from the first two images only
	4		Create WebP images
	5		Chroma Key images (GUI required)
	6		Crop images (GUI required)

*Note: works for JPEG, JPEG 2000 and PNG images*

## License
This code is licensed under the GNU GPL. See COPYING for more information.