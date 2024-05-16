#include "ImageViewerStructure.h"
ROIClass* ROIClass::self = nullptr;

ROIClass::ROIClass()
{
}
ROIClass* ROIClass::instance()
{
	if (self == nullptr) {
		self = new ROIClass();
	}
	return self;
}

void ROIClass::updateROI(QString name, int x, int y, int width, int height)
{
    QList<ROI>::iterator iter;
    for (iter = roiList.begin(); iter != roiList.end(); ++iter) {
        if (iter->name == name) {
            iter->x = x;
            iter->y = y;
            iter->width = width;
            iter->height = height;
            break;
        }
    }
}
void ROIClass::removeROI(QString name)
{
    QList<ROI>::iterator iter;
    for (iter = roiList.begin(); iter != roiList.end(); ++iter) {
        if (iter->name == name) {
            roiList.erase(iter);
            break;
        }
    }
}

void ROIClass::setROI(ROI roi) 
{ 
    roiList.push_back(roi);
}

QList<ROI>& ROIClass::getROIs()
{ 
    return roiList;
}
ROIClass::~ROIClass()
{
    self = nullptr;
}