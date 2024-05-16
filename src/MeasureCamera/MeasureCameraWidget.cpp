#include <QtWidgets/qfiledialog.h>
#include <QLabel>
#include <QtConcurrent>
#include "MeasureCameraWidget.h"
#include "MLBusinessManagePlugin.h"
#include "MLBusinessManageConfig.h"
#include "navigationBarPlugin.h"
#include "PluginCore.h"
#include "WaitWidget.h"
#include "LoggingWrapper.h"
#include "json4moderncpp/json.hpp"

using Json = nlohmann::json;
using namespace ML::CameraV2;
using namespace ML::MLBusinessManage;
using ML::MLBusinessManage::MLBusinessManageConfig;
MeasureCameraWidget::MeasureCameraWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    qRegisterMetaType<cv::Mat>("cv::Mat");
	initToolWidget();
    ui.loadImage->hide();
}

MeasureCameraWidget::~MeasureCameraWidget() {
    stopThread();
}

void MeasureCameraWidget::initToolWidget()
{
    // ui.loadImage->hide();
    ui.connectStatus->setText("Disconnected");
    MLBusinessManagePlugin* managePlugin = PluginCore::getinstance()->GetPlugin<MLBusinessManagePlugin>("MLBusinessManagePlugin");
    using ML::MLBusinessManage::MLBusinessManage;
	manage = dynamic_cast<MLBusinessManage*>(managePlugin);
	
	if (manage != nullptr)
	{
        QString configPath = QCoreApplication::applicationDirPath() + "/config";
        Result setResult = manage->ML_SetConfigPath(configPath.toStdString().c_str());
		if (setResult.success)
		{
			LoggingWrapper::getinstance()->Info("Set Config Successfully");
		}
        
        connect(manage, SIGNAL(connectStatus(bool)), this,SLOT(connectStatus(bool)));
	}

    ML::MLBusinessManage::ModuleConfig config =MLBusinessManageConfig::instance()->GetModuleInfo();
    for (auto it : config.CameraConfig_Map) {
        m_cameraName = it.second.Name;
        //m_GrabberModule = static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(it.second.Name));
        isColorCamera = it.second.ColourCamera;
    }

	m_thread = new QThread();
    m_cameraShow = new MLCameraShow();
	m_cameraShow->moveToThread(m_thread);
	m_thread->start();
 
	m_imageViewer = ImageViewer::instance();
	imageLayout = new QHBoxLayout;
	imageLayout->addWidget(m_imageViewer->getWidget());
	imageLayout->setMargin(5);
	ui.cameraViwer->setLayout(imageLayout);

	connect(ui.liveImg_Btn, &QCheckBox::toggled, this, &MeasureCameraWidget::switchLiveImage);
    connect(&watcher, &QFutureWatcher<Result>::finished, this,&MeasureCameraWidget::handleFinished);
}

void MeasureCameraWidget::on_loadImage_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "(*.tif *.tiff *.png)");
	if (!fileName.isEmpty())
	{
		cv::Mat img = cv::imread(fileName.toStdString(), cv::IMREAD_UNCHANGED);
		m_imageViewer->setImage(img);
		curSquareImg = img;
		LoggingWrapper::getinstance()->Info("Open image successfully");
	}
	else
	{
		LoggingWrapper::getinstance()->Warn("Imagefile path is empty");
	}
}

void MeasureCameraWidget::switchLiveImage(bool isChecked)
{
    if (!isChecked)
	{
        m_imageViewer->setImageModel(ImageViewerShowSingle);

        if (static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName)) != nullptr &&
            static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->IsOpened()) 
		{
            static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->StopGrabbing();
            MLPixelFormat m_formatType = isColorCamera ? MLPixelFormat::MLBayerGB12
                                            : MLPixelFormat::MLMono12;
            static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->SetFormatType(m_formatType);
			
		}

        disconnect(m_cameraShow,SIGNAL(SendCameraFrameReceiveSignal(cv::Mat, QImage)),this, SLOT(NotifyCameraFrameReceived(cv::Mat, QImage)));
        disconnect(m_cameraShow, SIGNAL(SendCameraGrayLevelSignal(int)),this, SLOT(NotifyCameraGrayLevel(int)));
  
    } 
	else 
	{
        m_imageViewer->setImageModel(ImageViewerShowContinuous);

        if (static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName)) != nullptr && static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->IsOpened())
		{
            static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->StartGrabbingAsync();
            MLPixelFormat m_formatType  = isColorCamera ? MLPixelFormat::MLBayerGB8
                                            : MLPixelFormat::MLMono8;
            static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->SetFormatType(m_formatType);
			
		}


        connect(m_cameraShow,SIGNAL(SendCameraFrameReceiveSignal(cv::Mat, QImage)), this,SLOT(NotifyCameraFrameReceived(cv::Mat, QImage)),Qt::QueuedConnection);
        connect(m_cameraShow, SIGNAL(SendCameraGrayLevelSignal(int)), this, SLOT(NotifyCameraGrayLevel(int)), Qt::QueuedConnection);
    }
}
void MeasureCameraWidget::setSwitchLive(bool status) {
    ui.liveImg_Btn->setChecked(status);
}
void MeasureCameraWidget::connectStatus(bool connected)
{  	 
	//switchLiveImage(connected);
    ui.liveImg_Btn->setChecked(connected);

	if (connected)
	{        
		if ((static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName)) != nullptr) && m_cameraShow != nullptr)
		{
            static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->Subscribe(MLCameraEvent::kFrameReceived,m_cameraShow);
            static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->Subscribe(MLCameraEvent::kGrayLevel,m_cameraShow);
		}

		m_subscribe = true;
		ui.connectStatus->setText("Connected.");
	}
	else
	{
		if (m_subscribe)
		{
            if ((static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName)) != nullptr) && (m_cameraShow != nullptr))
			{
                static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->Unsubscribe(MLCameraEvent::kFrameReceived,m_cameraShow);
                static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->Unsubscribe(MLCameraEvent::kGrayLevel,m_cameraShow);
			}

			m_subscribe = false;
		}
		ui.connectStatus->setText("Disconnected.");
	}

}
void MeasureCameraWidget::NotifyCameraStateChanged(MLCameraState old_state,MLCameraState new_state) {
}
void MeasureCameraWidget::NotifyCameraGrayLevel(int gray_level)
{
    emit sigCamraGratLevel(gray_level);
}
void MeasureCameraWidget::NotifyCameraFrameReceived(cv::Mat m_mat, QImage m_image)
{
	curSquareImg = m_mat;
	m_imageViewer->setMatList(m_mat);
	m_imageViewer->setImageList(m_image);
}

void MeasureCameraWidget::NotifyCameraFrameRate(double fps)
{
	m_imageViewer->setFrameRate(fps);
}

void MeasureCameraWidget::on_connectAll_clicked()
{
	if (manage)
	{
		WaitWidget::instance()->startAnimation();
		setEnabled(false);
        ui.connectStatus->setText("Connecting.");
        Result createResult = manage->ML_CreateModule();
        if (createResult.success) {
            LoggingWrapper::getinstance()->Info("Create Module Successfully");
        }
        QFuture<Result> future = QtConcurrent::run(manage, &ML::MLBusinessManage::MLBusinessManage::ML_ConnectModule);
		watcher.setFuture(future);
	}
}
void MeasureCameraWidget::on_closeAll_clicked() {
    Result result;
    ui.liveImg_Btn->setChecked(false);
	stopThread();
	unsubscribeCameras();
    if (manage)
	{
        result = manage->ML_DisconnectModule();

        if (!result.success) {
            return;
        }
        m_bIsConnected = false;

	}
    ui.connectStatus->setText("Not connected.");
	LoggingWrapper::getinstance()->Info( "Disconnect All Success.");
}

void MeasureCameraWidget::setEnabled(
        bool enabled) {
	ui.connectAll->setEnabled(enabled);
	ui.closeAll->setEnabled(enabled);
}




void MeasureCameraWidget::handleFinished()
{
	WaitWidget::instance()->stopAnimation();
	setEnabled(true);

	Result result = watcher.future().result();
	if (!result.success)
	{
		return;
	}
    m_bIsConnected = true;
	LoggingWrapper::getinstance()->Info("Connect All Success.");
}

cv::Mat MeasureCameraWidget::cvMatTo8Bit(const cv::Mat& intImg)
{
	/* cv::Mat img2 = intImg.clone();*/
	cv::Mat result = intImg;
	if (intImg.type() == CV_16UC1) {
		double minv = 0.0, maxv = 0.0;
		double* minp = &minv;
		double* maxp = &maxv;
		cv::minMaxIdx(intImg, minp, maxp);

		if (maxv > 4095.1)
		{
			cv::Mat tmp = intImg / 65535 * 255;

			tmp.convertTo(result, CV_8UC1);
		}
		else
		{
			cv::Mat tmp = intImg / 4095.0 * 255;

			tmp.convertTo(result, CV_8UC1);
		}
	}
	else if (intImg.type() == CV_32FC1) {
		double minv = 0.0, maxv = 0.0;
		double* minp = &minv;
		double* maxp = &maxv;
		cv::minMaxIdx(intImg, minp, maxp);
		cv::Mat outtmp = intImg - minv;
		outtmp.convertTo(result, CV_8U, 255.0 / (maxv - minv));

	}
	return result;
}
void MeasureCameraWidget::stopThread()
{
	if (m_thread != nullptr)
	{
		m_thread->exit();
		m_thread->wait();
		delete m_thread;
		m_thread = nullptr;
	}
}
void MeasureCameraWidget::unsubscribeCameras()
{
	if (m_subscribe)
	{
        if ((static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName)) != nullptr) && (m_cameraShow != nullptr))
		{
            static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->Unsubscribe(MLCameraEvent::kFrameReceived, m_cameraShow);
            static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(m_cameraName))->Unsubscribe(MLCameraEvent::kGrayLevel,m_cameraShow);
		}
	}
}