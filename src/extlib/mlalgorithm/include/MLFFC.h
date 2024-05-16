#pragma once
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include"CalibrationProcess.h"


class  __declspec(dllexport) FFCProcessor:public CalibrationProcess
{
public:
	/// <summary>
	/// Set the parameters those used to calculate the FFC map;
	/// </summary>
	/// <param name="light">The input image from camera that the exposure time is higher than dark</param>
	/// <param name="dark">The inout image from camera that the exposure time is lower than light</param>
	/// <param name="length">The length of ROI in the middle of the light image if the value=0</param>
	/// <param name="value">The default value is 0</param>
	void setFFCImage(cv::Mat light, cv::Mat dark, int length, double value=0);
	void setFFCImage(cv::Mat light, int length, double value = 0);

	/// <summary>
	/// Set the dark image.
	/// </summary>
	/// <param name="dark">The dark image</param>
	void setDarkImage(cv::Mat dark);
	/// <summary>
	///  The FFC calibration process;
	/// </summary>
	/// <param name="img">The image taken by the camera including the setup parameters like bining,exposure time etc  </param>   
	/// <returns></returns>
	ImageData  process(ImageData& img);
private:
	/// <summary>
	/// Calculate the FFC map that could be used to the ffc calibration.
	/// </summary>
	/// <param name="light">The input image from camera that the exposure time is higher than dark</param>
	/// <param name="dark">The inout image from camera that the exposure time is lower than light</param>
	/// <param name="length">The length of ROI in the middle of the light image if the value=0</param>
	/// <param name="value">The default value is 0</param>
	/// <returns></returns> Return the FFC map that could used to do the FFC calibration.
	cv::Mat getFFCMap(cv::Mat light, cv::Mat dark, int length, double value);
	/// <summary>
	/// According to the bining num and crop area, change light and dark image to the crrrect ROI image; 
	/// </summary>
	/// <param name="img">The image taken by the camera including the setup parameters like bining,exposure time etc</param>
	void preFFCProcess(ImageData& img);
	/// <summary>
	/// Sanity check arguments for FFC operators.This is called internally by the FFCMap functions.
	/// Violations will throw exceptions. 
	/// </summary>
	/// <param name="light">The input image with higher exposure time</param>
	/// <param name="dark">The input image with lower exposure time </param>
	/// <param name="caller">Name of calling function for error reporting</param>
	void  FFCOperator_CalibrationParameterCheck(cv::Mat& light, cv::Mat& dark, const char* caller);
	/// <summary>
	/// Sanity check arguments for FFC operators.This is called internally by the FFCCorrect functions.
	/// Violations will throw exceptions.
	/// </summary>
	/// <param name="rows">The rows of the input image</param>
	/// <param name="cols">The cols of the input image</param>
	/// <param name="length">The ROI lengh</param>
	/// <param name="caller">Name of calling function         for error reporting</param>
	void  FFCOperator_ROIlenghParameterCheck(int rows, int cols, int length, const char* caller);
	/// <summary>
	/// Sanity check arguments for FFC operators.This is called internally by the FFC functions.
	/// Violations will throw exceptions. 
	/// </summary>
	/// <param name="img">The input image</param>
	/// <param name="caller">Name of calling function for error reporting</param>
	void  FFCOperator_InputImageCheck(cv::Mat& img, const char* caller);

	cv::Mat m_light;
	cv::Mat m_dark;
	double m_value;
	int m_length;
	cv::Mat m_map;
};

