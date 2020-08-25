# CoinPictureManager

Console application to manage and prepare coin pictures, including green screening, automatic cropping, thumbnail creation, and more

Dependencies:
-  OpenCV
   -  Download the version used: ([download the opencv-4.4.0-vc14_vc15.exe file)](https://github.com/opencv/opencv/releases/tag/4.4.0))
   - Next, run the EXE file - make note of the directory where the files are extracted to (such as C:\opencv)
	- Open a Command Prompt window in "Run as administrator" mode and run the following commands (replacing YOUR_OPENCV_PATH with the path to where you extracted OpenCV to):
	   - for 32-bit Windows:
			`setx -m OPENCV_DIR C:\YOUR_OPENCV_PATH\opencv\build\x86\vc15`
	   - or, if you have 64-bit Windows
			`setx -m OPENCV_DIR C:\YOUR_OPENCV_PATH\opencv\build\x64\vc15`
   - Then, edit the system Environment Variables and add the following to the PATH variable ([see this site if you need guidance](https://www.howtogeek.com/118594/how-to-edit-your-system-path-for-easy-command-line-access/)):
      - `%OPENCV_DIR%\bin`