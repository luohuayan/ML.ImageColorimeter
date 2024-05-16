#include "FourColorWidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QButtonGroup>
#include <qcoreevent.h>
#include "WaitWidget.h"
#include "PluginCore.h"
#include "LoggingWrapper.h"
#include "MLBusinessManagePlugin.h"
#include "MLBusinessManageConfig.h"
#include "ML_addInInterface.h"
#include <QtConcurrent>
#include <MeasureCameraPlugin.h>
#include <QMetaType>
#include <FilterWheelPlugin.h>
#include <CameraControlPlugin.h>
#include "ImageViewer.h"
#include "color-util\XYZ_to_RGB.hpp"
using namespace ML::MLBusinessManage;
using ML::MLBusinessManage::MLBusinessManageConfig;
using ML::MLFilterWheel::MLNDFilterConfiguation;
FourColorWidget::FourColorWidget(QWidget *parent)
{
	ui.setupUi(this);
    qRegisterMetaType<cv::Mat>("cv::Mat");
    //qRegisterMetaType<std::vector<cv::Mat>>("std::vector<cv::Mat>");
    initUI();
    //ui.label->hide();
    //ui.lineEdit_initET->hide();
    ui.autoExpourse->hide();
    ui.saveEtBtn->hide();
}

FourColorWidget::~FourColorWidget()
{}

void FourColorWidget::initUI() {
    ui.savePathStr->setEnabled(false);

    MLBusinessManagePlugin* managePlugin = PluginCore::getinstance()->GetPlugin<MLBusinessManagePlugin>("MLBusinessManagePlugin");
    manage = dynamic_cast<ML::MLBusinessManage::MLBusinessManage*>(managePlugin);

    // Add filter item from config
    ML::MLBusinessManage::ModuleConfig m_moduleCig =
        MLBusinessManageConfig::instance()->GetModuleInfo();
    std::map<std::string, MLNDFilterConfiguation> ndConfig_Map =
        m_moduleCig.NDFilterConfig_Map;
    std::map<std::string, CameraConfig> cameraConfig_Map =
        m_moduleCig.CameraConfig_Map;

    std::map<std::string, MLNDFilterConfiguation>::iterator iter;
    for (iter = ndConfig_Map.begin(); iter != ndConfig_Map.end(); ++iter) {
        std::string name = iter->first;
        MLNDFilterConfiguation ndConfig = iter->second;

        std::vector<std::pair<std::string, int>> nameVec(
            ndConfig.positionName_List.begin(),
            ndConfig.positionName_List.end());
        std::sort(nameVec.begin(), nameVec.end(), Mycmp());
        for (auto i = nameVec.begin(); i != nameVec.end(); ++i) {
            if (name == "ND") {
                ui.ndfilter->addItem((i->first).c_str());
            } else {
                ui.xyzFilter->addItem((i->first).c_str());
            }
        }
    }

    for (auto it : cameraConfig_Map) {
        CameraConfig cameraConfig = it.second;
        isColourCamera = cameraConfig.ColourCamera;
        cameraName = cameraConfig.Name;
    }
    if (isColourCamera) {
        ui.saveEtBtn->hide();
        ui.label_9->hide();
        ui.xyzFilter->hide();
        ui.correctColorShift->hide();
        ui.XYZgroupBox->hide();
    }

    QButtonGroup* btnGroup = new QButtonGroup();
    btnGroup->addButton(ui.resultData);
    btnGroup->addButton(ui.allData);

    MeasureCameraPlugin* measureCameraPlugin = PluginCore::getinstance()->GetPlugin<MeasureCameraPlugin>("MeasureCameraPlugin");
    FilterWheelPlugin* filterPlugin = PluginCore::getinstance()->GetPlugin<FilterWheelPlugin>("FilterWheelPlugin");
    CameraControlPlugin* cameraPlugin =PluginCore::getinstance()->GetPlugin<CameraControlPlugin>("CameraControlPlugin");
    connect(this, SIGNAL(liveImageSignal(bool)),measureCameraPlugin->GetWidget(), SLOT(setSwitchLive(bool)));
    //connect(ui.ndfilter, &QComboBox::currentTextChanged, this,&FourColorWidget::setNDFilterAsync);
    //connect(ui.xyzFilter, &QComboBox::currentTextChanged, this, &FourColorWidget::setXYZFilterAsync);
    //connect(ui.lineEdit_manualET, &QLineEdit::editingFinished,this,&FourColorWidget::setExposureTime);

    //Associate filters in other interfaces
    connect(ui.ndfilter, SIGNAL(currentTextChanged(QString)),filterPlugin->GetWidget(), SLOT(updateNDComb(QString)));
    connect(ui.xyzFilter, SIGNAL(currentTextChanged(QString)),filterPlugin->GetWidget(), SLOT(updateXYZComb(QString)));
    connect(ui.lineEdit_manualET, SIGNAL(textChanged(QString)),cameraPlugin->GetWidget(), SLOT(updateManualET(QString)));
    connect(filterPlugin->GetWidget(), SIGNAL(sigfilterChanged(QString,QString)), this,SLOT(updateFilter(QString,QString)));
    connect(filterPlugin->GetWidget(), SIGNAL(sigfilterChanged(QString,QString)), this,SLOT(updateFilter(QString,QString)));
    connect(cameraPlugin->GetWidget(), SIGNAL(sigSetExposureTime(QString)),this, SLOT(updateET(QString)));
    connect(&m_watcher, &QFutureWatcher<Result>::finished, this, &FourColorWidget::ThreadFinshed);

     connect(ui.AutoETBox, &QGroupBox::toggled, [=](bool checked) {
        if (checked) {
            ui.XYZgroupBox->setChecked(false);
            autoET = true;
        } else 
        {
            ui.XYZgroupBox->setChecked(true);
            autoET = false;
        }
    });

    connect(ui.XYZgroupBox, &QGroupBox::toggled, [=](bool checked) {
         if (checked) {
             ui.AutoETBox->setChecked(false);
             autoET = false;
         } else {
             ui.AutoETBox->setChecked(true);
             autoET = true;
         }
     });
}
void FourColorWidget::on_CalculateFourColor_clicked()
{
	try
	{
        ui.calStatus->setText("Ongoing!Please Wait...");
        ui.CalculateFourColor->setEnabled(false);
        emit liveImageSignal(false);

		WaitWidget::instance()->startAnimation();
        QFuture<Result> future = QtConcurrent::run(this, &FourColorWidget::CalculateFourColor);
        m_watcher.setFuture(future);
	}
	catch (const std::exception& e)
	{
		//throw runtime_error("Imageprocessing Exception:" + std::string(e.what()));
	}
	
}

Result FourColorWidget::CalculateFourColor() {
    Result rt;
    std::map<std::string, ML::MLBusinessManage::CaptureData> captureDataMap;
    if (static_cast<FilterWheelBase*>(manage->ML_GetOneModuleByName("XYZ")) == nullptr ||
        static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(cameraName)) ==nullptr) 
    {
        rt.success = false;
        rt.errorMsg = "Module is not created";
        return rt;
    }
    if (!isColourCamera)
    {
        //Grayscale camera
        rt = manage->ML_MoveND_XYZFilterByNameSync("XYZ", "X");
        ui.xyzFilter->setCurrentText(QString::fromStdString(manage->ML_GetND_XYZChannelName("XYZ")));

        if (!rt.success) {
            throw rt.errorMsg;
        }
        DoExposure(autoET, ui.xETLineEdit->text());
        ui.lineEdit_manualET->setText(QString::number(manage->ML_GetExposureTime(cameraName)));

        rt = manage->ML_CaptureImageSync(cameraName);
        ML::MLBusinessManage::CaptureData xdata = manage->ML_GetCaptureData();   
        captureDataMap.insert(std::make_pair(xdata.ColorFilter, xdata));

        rt = manage->ML_MoveND_XYZFilterByNameSync("XYZ", "Y");
        ui.xyzFilter->setCurrentText( QString::fromStdString(manage->ML_GetND_XYZChannelName("XYZ")));

        if (!rt.success) {
            throw rt.errorMsg;
        }
        DoExposure(autoET, ui.yETLineEdit->text());
        ui.lineEdit_manualET->setText(QString::number(manage->ML_GetExposureTime(cameraName)));

        rt = manage->ML_CaptureImageSync(cameraName);
        ML::MLBusinessManage::CaptureData ydata = manage->ML_GetCaptureData();
        captureDataMap.insert(std::make_pair(ydata.ColorFilter, ydata));

        rt = manage->ML_MoveND_XYZFilterByNameSync("XYZ", "Z");
        ui.xyzFilter->setCurrentText(QString::fromStdString(manage->ML_GetND_XYZChannelName("XYZ")));

        if (!rt.success) {
            throw rt.errorMsg;
        }
        DoExposure(autoET, ui.zETLineEdit->text());
        ui.lineEdit_manualET->setText(QString::number(manage->ML_GetExposureTime(cameraName)));

        rt = manage->ML_CaptureImageSync(cameraName);
        ML::MLBusinessManage::CaptureData zdata = manage->ML_GetCaptureData(); 
        captureDataMap.insert(std::make_pair(zdata.ColorFilter, zdata));
    }
    else
    {
        //Colour camera
        
        if (autoET) {
            int initialTime = ui.lineEdit_initET->text().toInt();

            if (initialTime != 0) {
                rt = manage->ML_AutoExposureSync(cameraName, initialTime);
            } else {
                rt = manage->ML_AutoExposureSync(cameraName);
            }
            if (!rt.success) {
                throw rt.errorMsg;
            }
            ui.lineEdit_manualET->setText(QString::number(manage->ML_GetExposureTime(cameraName)));

        }
        rt = manage->ML_CaptureImageSync(cameraName);
        captureDataMap = manage->ML_GetColorCameraCaptureData();
    }

    rt = manage->ML_SetCaptureDataMap(captureDataMap);
    if (ui.correctFFC->isChecked()) {
        rt = manage->ML_LoadFFCData();
        if (!rt.success) {
            throw rt.errorMsg;
        }
        rt = manage->ML_DoFFCCorrect();
        if (!rt.success) {
            throw rt.errorMsg;
        }
    }
    if (!isColourCamera) 
    {
        if (ui.correctColorShift->isChecked()) {
            rt = manage->ML_LoadColorShiftData();
            if (!rt.success) {
                throw rt.errorMsg;
            }
            rt = manage->ML_DoColorShiftCorrect();
            if (!rt.success) {
                throw rt.errorMsg;
            }
        }
    }

    if (ui.correctDistortion->isChecked()) {
        rt = manage->ML_LoadDistortionData();
        if (!rt.success) {
            throw rt.errorMsg;
        }
        rt = manage->ML_DoDistortionCorrect();
        if (!rt.success) {
            throw rt.errorMsg;
        }
    }
    rt = manage->ML_LoadExpsoureData();
    if (!rt.success) {
        throw rt.errorMsg;
    }
    rt = manage->ML_DoExposureCorrect();
    if (!rt.success) {
        throw rt.errorMsg;
    }
    rt = manage->ML_LoadFourColorData();
    if (!rt.success) {
        throw rt.errorMsg;
    }
    rt = manage->ML_DoFourColorCalculation();
    if (!rt.success) {
        throw rt.errorMsg;
    }
    if (ui.savegroupBox->isChecked() && ui.savePathStr->text() != "") 
    {
        rt = manage->ML_SaveCalibrationImage(
            ui.savePathStr->text().toStdString(),
            ui.prefixEdit->text().toStdString(), cv::Rect(), true,
            ui.resultData->isChecked(), ui.allData->isChecked());

    }

    return rt;
}
void FourColorWidget::DoExposure(bool isAutoExposure,QString manualEt) {
    Result result;
    if (isAutoExposure) 
    {

        int initialTime = ui.lineEdit_initET->text().toInt();

        if (initialTime != 0) 
        {
            result = manage->ML_AutoExposureSync(cameraName, initialTime);
        } else 
        {
            result = manage->ML_AutoExposureSync(cameraName);
        }  
        if (!result.success) {
            throw result.errorMsg;
        }
    }
   else {
        if (manualEt != "") {
            result = manage->ML_SetExposureTime(cameraName, manualEt.toDouble());
            if (!result.success) {
                throw result.errorMsg;
            }
        }
    }
}
void FourColorWidget::ThreadFinshed() {
    WaitWidget::instance()->stopAnimation();
    try
    {
        ui.CalculateFourColor->setEnabled(true);
       /* emit liveImageSignal(true);*/
        Result ret = m_watcher.future().result();
        if (!ret.success)
        {
            ui.calStatus->setText("Fail!");
            QMessageBox::critical(this, "Four Color correct error", QString::fromStdString(ret.errorMsg),
                                  QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }
        ui.calStatus->setText("Done!");

       std::map<ML::MLBusinessManage::CalibrationEnum,
                 std::map<std::string, cv::Mat>>
            m_CalibrationData = manage->ML_GetCalibrationData();
        auto it = m_CalibrationData.find(ML::MLBusinessManage::CalibrationEnum::FourColor);
      
       if (it != m_CalibrationData.end()) 
       {
           cv::Mat xImage, yImage, XImage, YImage, ZImage;
           std::map<std::string, cv::Mat> caliMatMap = it->second;

           //QList<cv::Mat> caliMatList;
           //for (const auto& pair : caliMatMap) {
           //    cv::Mat caliMat;
           //    caliMatList.append(pair.second);
           //}
           // emit updateYxyImage(caliMatList);
           auto iter = caliMatMap.find("xx");
           if (iter != caliMatMap.end()) { 
               xImage = iter->second;         
           } 
           iter = caliMatMap.find("yy");
           if (iter != caliMatMap.end()) {
               yImage = iter->second;
           } 
           iter = caliMatMap.find("X");
           if (iter != caliMatMap.end()) {
               XImage = iter->second;
           } 
           iter = caliMatMap.find("Y");
           if (iter != caliMatMap.end()) {
               YImage = iter->second;
           } 
           iter = caliMatMap.find("Z");
           if (iter != caliMatMap.end()) {
               ZImage = iter->second;
           } 
           cv::Mat rgbMat;
           ImageViewer* m_imageViewer = ImageViewer::instance();
           m_imageViewer->setYxyImage(YImage, xImage, yImage);
           //xyz2rgb(rgbMat, XImage, YImage, ZImage);
           //xyz2rgbUtil(rgbMat, XImage, YImage, ZImage);
           //m_imageViewer->setImage(rgbMat);
           m_imageViewer->setImage(YImage);
       }
    }
    catch (const std::exception &e)
    {
        ui.calStatus->setText("Fail!");
    }
   
}

void FourColorWidget::on_saveEtBtn_clicked()
{
    if (ui.xyzFilter->currentText() == "X" && ui.lineEdit_manualET->text() != "") {
        ui.xETLineEdit->setText(ui.lineEdit_manualET->text());
    } else if (ui.xyzFilter->currentText() == "Y" && ui.lineEdit_manualET->text() != "") {
        ui.yETLineEdit->setText(ui.lineEdit_manualET->text());
    } else if (ui.xyzFilter->currentText() == "Z" && ui.lineEdit_manualET->text() != "") {
        ui.zETLineEdit->setText(ui.lineEdit_manualET->text());
    }
}

void FourColorWidget::on_selectPath_clicked() {
    ui.savePathStr->clear();
    QString savePath = QFileDialog::getExistingDirectory(nullptr, "choose folder", "", QFileDialog::ShowDirsOnly);
    if (savePath == "") {
        QMessageBox::warning(this, tr("Calculate Four Color Error"),
                             "Please Set Save Path!");
        return;
    } else {
        ui.savePathStr->setText(savePath);
        m_savePath = savePath;
    }
}

void FourColorWidget::setNDFilterAsync(QString name) 
{
    if (manage == nullptr)
        return;
    if (static_cast<FilterWheelBase*>(manage->ML_GetOneModuleByName("ND")) !=nullptr) 
    {
        std::string nd = name.toStdString();
        Result ret =
            manage->ML_MoveND_XYZFilterByNameAsync("ND", name.toStdString());

        if (ret.success) {
            LoggingWrapper::getinstance()->Info("Move ND Filter Successfully");
        }
    }
}

void FourColorWidget::setXYZFilterAsync(QString name) 
{
    if (manage == nullptr)
        return;
    if (static_cast<FilterWheelBase*>(manage->ML_GetOneModuleByName("XYZ")) != nullptr) 
    {
        std::string xyz = name.toStdString();
        Result ret =
            manage->ML_MoveND_XYZFilterByNameAsync("XYZ", name.toStdString());
        if (ret.success) {
            LoggingWrapper::getinstance()->Info("Move XYZ Filter Successfully");
        }
    }
}
void FourColorWidget::setExposureTime() 
{
    if (manage == nullptr)
        return;
    if (static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(cameraName)) != nullptr) 
    {
        Result result;
        double expTime = ui.lineEdit_manualET->text().toDouble();
        result = manage->ML_SetExposureTime(cameraName, expTime);
        if (result.success) {
            LoggingWrapper::getinstance()->Info("Set Exposure Time Successfully");
        }
    }

}
void FourColorWidget::updateFilter(QString type, QString name) {
    if (type == "ND") {
        ui.ndfilter->setCurrentText(name);
    } else {
        ui.xyzFilter->setCurrentText(name);
    }        
}
void FourColorWidget::updateET(QString et) {
     ui.lineEdit_manualET->setText(et); 
}
void FourColorWidget::xyz2rgb(cv::Mat& rgbMat, const cv::Mat& xImage,
                               const cv::Mat& yImage, const cv::Mat& zImage) {

    cv::Mat R = cv::Mat::zeros(xImage.size(), xImage.type());
    cv::Mat G = cv::Mat::zeros(xImage.size(), xImage.type());
    cv::Mat B = cv::Mat::zeros(xImage.size(), xImage.type());

    for (int i = 0; i < xImage.rows; i++) {
        for (int j = 0; j < xImage.cols; j++) {

            float var_X = xImage.at<float>(i, j);
            float var_Y = yImage.at<float>(i, j);
            float var_Z = zImage.at<float>(i, j);


            float var_R = 3.2404542 * var_X + (-1.5371385) * var_Y + (-0.4985314) * var_Z;
            float var_G = (-0.9692660) * var_X + (1.8760108) * var_Y +(0.0415560) * var_Z;
            float var_B = (0.0556434) * var_X + (-0.2040259) * var_Y +(1.0572252) * var_Z;

            if (var_R > 0.0031308)
                // var_R = pow(1.055 * var_R, (1 / 2.4)) - 0.055;
                // var_R = 1.055 * pow(var_R, (1 / 2.4)) - 0.055;
                var_R = pow((var_R - 0.055) * 1.055, 1 / 2.4);
            else
                var_R = 12.92 * var_R;
            if (var_G > 0.0031308)
                // var_G = pow(1.055 * var_G, (1 / 2.4)) - 0.055;
                // var_G = 1.055 * pow(var_G, (1 / 2.4)) - 0.055;
                var_G = pow((var_G - 0.055) * 1.055, 1 / 2.4);
            else
                var_G = 12.92 * var_G;
            if (var_B > 0.0031308)
                // var_B = pow(1.055 * var_B, (1 / 2.4)) - 0.055;
                // var_B = 1.055 * pow(var_B, (1 / 2.4)) - 0.055;
                var_B = pow((var_B - 0.055) * 1.055, 1 / 2.4);
            else
                var_B = 12.92 * var_B;

            R.at<float>(i, j) = var_R;
            G.at<float>(i, j) = var_G;
            B.at<float>(i, j) = var_B;
        }
    }
    if (R.type() == CV_32FC1) {
        double minv = 0.0, maxv = 0.0;
        double* minp = &minv;
        double* maxp = &maxv;
        cv::minMaxIdx(R, minp, maxp);
        cv::Mat outtmp = R - minv;
        outtmp.convertTo(R, CV_8U, 255.0 / (maxv - minv));
    }
    if (G.type() == CV_32FC1) {
        double minv = 0.0, maxv = 0.0;
        double* minp = &minv;
        double* maxp = &maxv;
        cv::minMaxIdx(G, minp, maxp);
        cv::Mat outtmp = G - minv;
        outtmp.convertTo(G, CV_8U, 255.0 / (maxv - minv));
    }
    if (B.type() == CV_32FC1) {
        double minv = 0.0, maxv = 0.0;
        double* minp = &minv;
        double* maxp = &maxv;
        cv::minMaxIdx(B, minp, maxp);
        cv::Mat outtmp = B - minv;
        outtmp.convertTo(B, CV_8U, 255.0 / (maxv - minv));
    }
    //R.convertTo(R, CV_8UC1);
    //G.convertTo(G, CV_8UC1);
    //B.convertTo(B, CV_8UC1);

    std::vector<cv::Mat> imgV;
    imgV.push_back(B);
    imgV.push_back(G);
    imgV.push_back(R);
    cv::merge(imgV, rgbMat);
}
Result FourColorWidget::xyz2rgbUtil(cv::Mat& rgbMat, const cv::Mat& xImage,
                                const cv::Mat& yImage, const cv::Mat& zImage) {
    cv::Mat X, Y, Z;
    X = xImage;
    Y = yImage;
    Z = zImage;
   

    cv::Mat R = cv::Mat::zeros(X.size(), X.type());
    cv::Mat G = cv::Mat::zeros(X.size(), X.type());
    cv::Mat B = cv::Mat::zeros(X.size(), X.type());

    for (int i = 0; i < X.rows; i++) {
        for (int j = 0; j < X.cols; j++) {
            float x = X.at<float>(i, j);
            float y = Y.at<float>(i, j);
            float z = Z.at<float>(i, j);

            const colorutil::XYZ xyz_color(x, y, z);
            const colorutil::RGB rgb_color = colorutil::convert_XYZ_to_RGB(xyz_color);
            R.at<float>(i, j) = rgb_color(0);
            G.at<float>(i, j) = rgb_color(1);
            B.at<float>(i, j) = rgb_color(2);
        }
    }
    // G.setTo(0, G > 65000);
    std::vector<cv::Mat> imgV;
    //R.convertTo(R, CV_8UC1);
    //G.convertTo(G, CV_8UC1);
    //B.convertTo(B, CV_8UC1);
    if (R.type() == CV_32FC1) {
        double minv = 0.0, maxv = 0.0;
        double* minp = &minv;
        double* maxp = &maxv;
        cv::minMaxIdx(R, minp, maxp);
        cv::Mat outtmp = R - minv;
        outtmp.convertTo(R, CV_8U, 255.0 / (maxv - minv));
    }
    if (G.type() == CV_32FC1) {
        double minv = 0.0, maxv = 0.0;
        double* minp = &minv;
        double* maxp = &maxv;
        cv::minMaxIdx(G, minp, maxp);
        cv::Mat outtmp = G - minv;
        outtmp.convertTo(G, CV_8U, 255.0 / (maxv - minv));
    }
    if (B.type() == CV_32FC1) {
        double minv = 0.0, maxv = 0.0;
        double* minp = &minv;
        double* maxp = &maxv;
        cv::minMaxIdx(B, minp, maxp);
        cv::Mat outtmp = B - minv;
        outtmp.convertTo(B, CV_8U, 255.0 / (maxv - minv));
    }
    imgV.push_back(B);
    imgV.push_back(G);
    imgV.push_back(R);

    cv::merge(imgV, rgbMat);
    return Result();
}
