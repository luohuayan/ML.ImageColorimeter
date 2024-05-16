#include "ImageViewer.h"
#include "ImageViewerWidget.h"
#include "ImageViewerLogic.h"
#include "LineCutWindow.h"
#include <QThread>
#include <QDebug>
#include <QList>
#include "StateMachine.h"

ImageViewer* ImageViewer::self = nullptr;
ImageViewer::ImageViewer(QObject* parent /*= nullptr*/) 
	: QObject(parent)
{

	qRegisterMetaType<QList<float> >("QList<float>");
	qRegisterMetaType<QList<QPointF> >("QList<QPointF>");
	qRegisterMetaType<ROI>("ROI");
	m_widget = new ImageViewerWidget();
	m_viewerLogic = new ImageViewerLogic();
	m_lineCutWindow = new LineCutWindow();

	//interface-->logic-->view
	connect(this, &ImageViewer::sigUpdateImage, m_viewerLogic, &ImageViewerLogic::onUpdateImage, Qt::QueuedConnection);
    connect(m_viewerLogic, &ImageViewerLogic::sigUpdateImage, m_widget,&ImageViewerWidget::onUpdateImage, Qt::QueuedConnection);

	connect(this, &ImageViewer::sigRowSelected, m_widget, &ImageViewerWidget::onRowSelected);
	connect(this, &ImageViewer::sigJsonParse, m_widget, &ImageViewerWidget::onJsonParse);
	
	//view-->logic-->interface
	//connect(m_widget, &ImageViewerWidget::sigROIChanged, m_viewerLogic, &ImageViewerLogic::onROIchanged);
	//connect(m_viewerLogic, &ImageViewerLogic::sigROIChanged, this, &ImageViewer::ROIUpdate);

	connect(m_widget, &ImageViewerWidget::sigROIChanged, this, &ImageViewer::ROIUpdate);

	connect(m_widget, &ImageViewerWidget::sigItemChoosed, this, &ImageViewer::ItemChoosed);

	connect(m_widget, &ImageViewerWidget::sigSaveImageClicked, m_viewerLogic, &ImageViewerLogic::onSaveImageClicked, Qt::QueuedConnection);
	
	connect(m_widget, &ImageViewerWidget::sigActionCutLineChanged, m_lineCutWindow, &LineCutWindow::setVisible);
	connect(m_widget, &ImageViewerWidget::sigCutLineChanged, m_viewerLogic, &ImageViewerLogic::onLinCutChanged, Qt::QueuedConnection);

	connect(m_viewerLogic, &ImageViewerLogic::sigUpdateCutLine, m_lineCutWindow, &LineCutWindow::onUpdateCutLine, Qt::QueuedConnection);
	connect(m_widget, &ImageViewerWidget::sigUpdateRGBImage, m_viewerLogic, &ImageViewerLogic::onUpdateRGBImage);
    connect(m_widget, &ImageViewerWidget::sigMousePosChanged, m_viewerLogic,&ImageViewerLogic::onMousePosChanged, Qt::QueuedConnection);
    connect(m_viewerLogic, &ImageViewerLogic::sigUpdateGrayValue, m_widget,
            &ImageViewerWidget::updateTextShowInfo);

	
	//move to logic thread
	m_viewerLogicThread = new QThread(this);
	m_viewerLogicThread->setObjectName("ViewLogicThread");
	m_viewerLogic->moveToThread(m_viewerLogicThread);
	connect(m_viewerLogicThread, &QThread::started, m_viewerLogic, &ImageViewerLogic::onThreadStarted);
	m_viewerLogicThread->start();
}

ImageViewer::~ImageViewer()
{
	m_viewerLogicThread->isRunning();
	m_viewerLogicThread->quit();
	m_viewerLogicThread->wait();

	delete m_viewerLogicThread;
	m_viewerLogicThread = nullptr;
}

ImageViewer* ImageViewer::instance()
{
	if (self == nullptr) {
		self = new ImageViewer();
	}
	return self;
}

QList<ROI> ImageViewer::getROIs()
{
	return m_rois;
}
void ImageViewer::ROIUpdate(QString name) 
{
	//std::copy(ROIVector.begin(), ROIVector.end(), std::back_inserter(m_rois));
	m_rois = ROIClass::instance()->getROIs();
	emit sigROIChanged(name);
}
void ImageViewer::ItemChoosed(QString name)
{
	emit sigItemChoosed(name);
}

QWidget* ImageViewer::getWidget()
{
	return m_widget;
}

void ImageViewer::setImage(cv::Mat img)
{
	m_viewerLogic->setImage(img);
	emit sigUpdateImage();
}

void ImageViewer::setYxyImage(cv::Mat Y, cv::Mat x, cv::Mat y)
{
	m_viewerLogic->setYxyImage(Y, x, y);
}

void ImageViewer::setXYZImage(cv::Mat& X, cv::Mat& Y, cv::Mat& Z)
{
	m_viewerLogic->setXYZImage(X,Y,Z);
	emit sigUpdateImage();
}
void ImageViewer::setImageList(QImage image)
{
    m_viewerLogic->setImageList(image);
	emit sigUpdateImage();
}
void ImageViewer::setMatList(cv::Mat image)
{
    m_viewerLogic->setMatList(image);
}
void ImageViewer::setFrameRate(double fps) 
{
	m_widget->setFrameRate(fps);
}
void ImageViewer::setImageModel(ImageModel model)
{
	m_viewerLogic->setImageModel(model);
    m_widget->setImageModel(model);
	StateMachine::instance()->setShowMode(model);
}
void ImageViewer::addRoi(QPoint center)
{
	m_widget->onAddRoi(center);
}
void ImageViewer::clearRoi()
{
	m_widget->onClearRoi();
}
