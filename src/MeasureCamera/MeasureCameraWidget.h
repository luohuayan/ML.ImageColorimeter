#pragma once

#include <QWidget>
#include <QTimer>
#include "ui_MeasureCameraWidget.h"
#include <QButtonGroup>
#include <QFutureWatcher>
#include "ImageViewer.h"
#include "service/Result.h"
#include "MLCameraShow.h"
#include "MLBusinessManage.h"
#include "ML_addInInterface.h"
class MeasureCameraWidget : public QWidget
{
	Q_OBJECT

public:
	MeasureCameraWidget(QWidget *parent = nullptr);
	~MeasureCameraWidget();
private:
	void initToolWidget();
	cv::Mat cvMatTo8Bit(const cv::Mat& intImg);

	void setEnabled(bool enabled);
	void stopThread();
	void unsubscribeCameras();
private slots:
	void on_connectAll_clicked();
	void on_closeAll_clicked();
	void on_loadImage_clicked();
	void handleFinished();
	void switchLiveImage(bool isChecked);
    void setSwitchLive(bool status);
	void connectStatus(bool connected);
    void NotifyCameraStateChanged(ML::CameraV2::MLCameraState old_state,
                                  ML::CameraV2::MLCameraState new_state);
	void NotifyCameraFrameReceived(cv::Mat m_mat, QImage m_image);
	void NotifyCameraGrayLevel(int gray_level);
	void NotifyCameraFrameRate(double fps);
signals:
	void sigConnectStatus(bool stat);
    void sigCamraGratLevel(int);

private:
	Ui::MeasureWidgetClass ui;
	cv::Mat curSquareImg;
	ImageViewer* m_imageViewer = nullptr;
	QHBoxLayout* imageLayout;
	QTimer* m_Timer;
    ML::MLBusinessManage::MLBusinessManage* manage = nullptr;
	QFutureWatcher<Result> watcher;
	MLCameraShow* m_cameraShow;
	QThread* m_thread;
	bool m_subscribe = false;
    bool isColorCamera = false;
    std::string m_cameraName;
    bool m_bIsConnected;
};