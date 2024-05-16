#pragma once
#include "imageviewer_global.h"

#include "ImageViewerStructure.h"
#include <QObject>
#include <QThread>
#include <opencv2/opencv.hpp>
class ImageViewerWidget;
class ImageViewerLogic;
class LineCutWindow;

class IMAGEVIEWER_EXPORT ImageViewer: public QObject
{
	Q_OBJECT
public:
    ImageViewer(QObject *parent = nullptr);
	~ImageViewer();
	static ImageViewer* instance();


	QList<ROI> getROIs();
	QWidget* getWidget();

	void setImage(cv::Mat img);
	void setYxyImage(cv::Mat Y, cv::Mat x, cv::Mat y);
	void setXYZImage(cv::Mat& X, cv::Mat& Y, cv::Mat& Z);
	void setImageList(QImage image);
	void setMatList(cv::Mat image);
	void setImageModel(ImageModel model);
	void setFrameRate(double fps);
	void addRoi(QPoint center);
	void clearRoi();
signals:
	void sigROIChanged(QString);
	void sigUpdateImage();
	void sigItemChoosed(QString);
	void sigRowSelected(QString name);
	void sigJsonParse(QString filePath);
  private slots:
	void ROIUpdate(QString);
	void ItemChoosed(QString);

  private:
	static ImageViewer* self;
	ImageViewerWidget* m_widget = nullptr;
	ImageViewerLogic* m_viewerLogic = nullptr;
	LineCutWindow* m_lineCutWindow = nullptr;
	QThread* m_viewerLogicThread = nullptr;
	QList<ROI> m_rois;

};
