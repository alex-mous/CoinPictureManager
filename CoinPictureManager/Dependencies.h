#ifndef DEPENDENCIES_H
	#define DEPENDENCIES_H

	#include <iostream>
	#include <opencv2/core/core.hpp>
	#include <opencv2/highgui/highgui.hpp>
	#include <opencv2/imgproc/imgproc.hpp>
	#include <iostream>
	#include <iomanip>
	#include <filesystem>
	#include <string>
	#include <stdlib.h>
	

	#ifdef _WIN32
		#include <Windows.h>
		#include <filesystem>
	#elif __linux__
		#include <X11/Xlib.h>
		#include <experimental/filesystem>
	#endif

	namespace fs = std::experimental::filesystem;

	using namespace std;
	using namespace cv;

#endif