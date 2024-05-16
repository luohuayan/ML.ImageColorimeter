#pragma once

#include <QWidget>
#include "ui_CameraControlWidget.h"
#include "MLBusinessManage.h"

using namespace ML::CameraV2;
using namespace ML::MLBusinessManage;
class CameraControlWidget : public QWidget
{
	Q_OBJECT
public:
	CameraControlWidget(QWidget* parent = nullptr);
	~CameraControlWidget();

	void init();
 signals:
    void sigSetExposureTime(QString et);  
 public slots:
    void setBinningMode(QString binningMode);
    void setBining(QString binning);
    void setExposureTime();
    void autoExposure();
    void refreshSettingRes();
    void updateGrayLevelBar(int gray);
    void updateConnectStatus(bool status);
    inline void updateManualET(QString et) { ui.EtText->setText(et); };
 private:
	Ui::CameraControlWidgetClass ui;
    bool isColorCamera = false;
    MLBusinessManage* manage;
    std::string cameraName;
    QMap<BinningMode, QString> binningModeMap = {{BinningMode::SUM, "SUM"}, {BinningMode::AVERAGE, "AVERAGE"}};
    QMap<Binning, QString> binningMap = {{Binning::ONE_BY_ONE, "ONE_BY_ONE"},
                                        {Binning::TWO_BY_TWO, "TWO_BY_TWO"},
                                        {Binning::FOUR_BY_FOUR, "FOUR_BY_FOUR"},
                                        {Binning::EIGHT_BY_EIGHT, "EIGHT_BY_EIGHT"}};
    QMap<MLPixelFormat, QString> formatMap = {
        {MLPixelFormat::MLMono8, "MLMono8"},
        {MLPixelFormat::MLMono10, "MLMono10"},
        {MLPixelFormat::MLMono12, "MLMono12"},
        {MLPixelFormat::MLMono16, "MLMono16"},
        {MLPixelFormat::MLRGB24, "MLRGB24"},
        {MLPixelFormat::MLBayer, "MLBayer"},
        {MLPixelFormat::MLBayerGB8, "MLBayerGB8"},
        {MLPixelFormat::MLBayerGB10, "MLBayerGB10"},
        {MLPixelFormat::MLBayerGB12, "MLBayerGB12"}};
                                     
};