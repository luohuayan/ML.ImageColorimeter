#pragma once

#include "imageviewer_global.h"
#include <fstream>
#include <iostream>
#include <QList>

#include <opencv2/opencv.hpp>

 struct IMAGEVIEWER_EXPORT ROI
{
	int index;
	int x;
	int y;
	int width;
	int height;
    QString name;
    QString type;
};
 class ROIClass
 {
 public:
     ~ROIClass();
     static ROIClass* instance();
     //static ROIClass* self;
     QList<ROI>& getROIs();
     void setROI(ROI roi);
     void updateROI(QString name, int x, int y, int width, int height);
     void removeROI(QString name);
     
 private:
     static ROIClass* self;
     QList<ROI> roiList;
     std::string filepath;
     ROI roiPosInfo;
     ROIClass() ;
 };

 struct IMAGEVIEWER_EXPORT XYZImage {
     cv::Mat imgX;
     cv::Mat imgY;
     cv::Mat imgZ;
 };
 //enum IMAGEVIEWER_EXPORT ImageViewerShowMode {
 //    ImageViewerShowContinuous,
 //    ImageViewerShowSingle
 //};
 enum IMAGEVIEWER_EXPORT ImageModel{
     ImageViewerShowContinuous,
     ImageViewerShowSingle
 };



