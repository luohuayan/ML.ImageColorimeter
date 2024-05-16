#pragma once
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include"CalibrationProcess.h"
class  __declspec(dllexport)MLFourColor
{
public:
	MLFourColor();
	~MLFourColor();
	/// <summary>
	/// Set the R matrix parameter of the four color calibration.
	/// </summary>
	/// <param name="R">The R matrix</param>
	void setRMatrix(cv::Mat R);
	/// <summary>
	/// Use the Rmatrix to calibrate the image from the camera
	/// </summary>
	/// <param name="X">The input image from the camera in X filter that need to be calibrated</param>
	/// <param name="Y">The input image from the camera in Y filter that need to be calibrated</param>
	/// <param name="Z">The input image from the camera in Z filter that need to be calibrated</param>
	/// <returns></returns> Return the data after four color calibration.
	map<string, cv::Mat>fourColorProcess(map<string,cv::Mat>matMap);
private:
	void FourColorOperator_RmatrixParameterCheck(cv::Mat& Rmatrix, const char* caller);
	void FourColorOperator_InputImageCheck(cv::Mat& X, cv::Mat& Y, cv::Mat& Z, const char* caller);

	cv::Mat m_R;
};

