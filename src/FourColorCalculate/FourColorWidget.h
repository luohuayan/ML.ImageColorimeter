#pragma once
#include <QWidget>
#include "ui_FourColorWidget.h"
#include <MLBusinessManage.h>
#include <QFutureWatcher>
using ML::MLBusinessManage::MLBusinessManage;
class FourColorWidget : public QWidget {
    Q_OBJECT

 public:
	FourColorWidget(QWidget *parent = nullptr);
	~FourColorWidget();
    void xyz2rgb(cv::Mat& rgbMat, const cv::Mat& xImage, const cv::Mat& yImage,const cv::Mat& zImage);
    Result xyz2rgbUtil(cv::Mat& rgbMat, const cv::Mat& xImage,
                       const cv::Mat& yImage, const cv::Mat& zImage);
    void DoExposure(bool isAutoExposure,QString manualEt);
 signals:
    void liveImageSignal(bool isClose);
    void updateYxyImage(QList<cv::Mat> caliMatList);
    
 public slots:
	void on_CalculateFourColor_clicked();
    void ThreadFinshed();
    void on_saveEtBtn_clicked();
    void on_selectPath_clicked();
    void setNDFilterAsync(QString name);
    void setXYZFilterAsync(QString name);
    void setExposureTime(); 
    void updateFilter(QString type, QString name);
    void updateET(QString et);
    Result CalculateFourColor();

 private:
	void initUI();

    struct Mycmp {
        bool operator()(const std::pair<std::string, int>& p1,
                        const std::pair<std::string, int>& p2) {
            return p1.second < p2.second;
        }
    };

    QString m_savePath = "";
    bool isSetColorLight = false;
    bool isSetNDFilter = false;

private:
	Ui::FourColorWidgetClass ui;
    MLBusinessManage* manage;
    QFutureWatcher<Result> m_watcher;
    bool isColourCamera = false;
    bool autoET = true;
    std::string cameraName;
};
