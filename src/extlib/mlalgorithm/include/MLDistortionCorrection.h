#pragma once
#include<vector>
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include"CalibrationProcess.h"

class __declspec(dllexport)DistortionProcessor:public CalibrationProcess
{
public:
	/// <summary>
	/// Set the distortion parameters that need to be used;
	/// </summary>
	/// <param name="instrinsic_matrix">The camera Intrinsic Matrix</param>
	/// <param name="distortion_coefficient">The camera Radial and Tangential parameters,the order: k1,k2,p1,p2,k3</param>
	void MLSetDistortionData(cv::Mat instrinsic_matrix, cv::Mat distortion_coefficient);
	/// <summary>
	///  Set the distortion parameters that need to be used;
	/// </summary>
	/// <param name="cameraMatrix">The camera Intrinsic Matrix</param>
	void SetCameraMatrix(cv::Mat cameraMatrix);
	/// <summary>
	///  Set the distortion parameters that need to be used;
	/// </summary>
	/// <param name="coefficient">The camera Radial and Tangential parameters,the order: k1,k2,p1,p2,k3</param>
	void SetCoefficient(cv::Mat coefficient);
	/// <summary>
	/// The distortion calibration process;
	/// </summary>
	/// <param name="img">The image taken by the camera including the setup parameters like bining,exposure time etc</param>
	/// <returns></returns>
	ImageData process(ImageData& img);

private:
	/// <summary>
	/// According to the image bining number to change the distortion parameters;
	/// </summary>
	/// <param name="img">The image taken by the camera including the setup parameters like bining,exposure time etc</param>
	void preDistortionProcess(ImageData img);
	void  DistortionProcessOperator_InputImageCheck(cv::Mat& img, const char* caller);

	cv::Mat m_map1, m_map2;
	cv::Mat m_instrinsic_matrix = cv::Mat(3, 3, CV_32F);
	cv::Mat m_distortion_coefficient = cv::Mat(1, 5, CV_32F);
	cv::Mat m_instrinsic_matrixTmp = cv::Mat(3, 3, CV_32F, cv::Scalar(0));
	cv::Mat m_distortion_coefficientTmp = cv::Mat(1, 5, CV_32F,cv::Scalar(0));

};

