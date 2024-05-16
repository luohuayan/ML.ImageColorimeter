#pragma once
//#include "json4moderncpp/json.hpp"
#include <QObject>
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
#include "./json4moderncpp/json.hpp"
#include "opencv2/opencv.hpp"
using Json = nlohmann::json;

struct ROIMTFValues {
    int result = 0;
    int roiId = 0;
    int binLen = 0;
    double* m_mtfVals = nullptr;
    double* m_lsfVals = nullptr;
    double* m_esfVals = nullptr;
    double* m_freqVals = nullptr;
    double* m_lsfreqVals = nullptr;

    ~ROIMTFValues() {
        if (m_mtfVals != nullptr) {
            delete[] m_mtfVals;
        }
        if (m_freqVals != nullptr) {
            delete[] m_freqVals;
        }

        if (m_lsfreqVals != nullptr) {
            delete m_lsfreqVals;
        }
        if (m_lsfVals != nullptr) {
            delete[] m_lsfVals;
        }
        if (m_esfVals != nullptr) {
            delete[] m_esfVals;
        }
    }
    //QMap<QString, double> specMtfVal;
};
struct ROIPosConfig
{
    int centerOffset = 40;
    int roiWidth = 60;
    int roiHeight = 60;
};
struct CrossAreaConfig
{
    int FromX;
    int FromY;
    int aoiWidth;
    int aoiHeight;
    std::string name;
};
struct MTFLimitConfig
{
    int id = 0;
    std::string freqVal = "0";
    int freqId = 0;
    double upperLimit = 23.0;
    double lowerLimit = 20.0;
};
class MTFModelConfig
{
  public:
    static MTFModelConfig *instance();
    void load(const char *path);
    ROIPosConfig getROIPosCfg();

    std::vector<CrossAreaConfig *> getCrossAreaConfig();
    void setROIPosCfg(ROIPosConfig cfg);
    std::vector<std::string> getSpecifedFreq();
    std::vector<MTFLimitConfig> getMTFLimit(int crossImgIdx);
    double getROIcenterOffset();
    double getFocusLength();
    void setFocusLength(double val);
    double getVid();
    void setVid(double val);
    std::string getDisplayResultFreq();
    Json getJsonObj();
    ~MTFModelConfig();
    bool updateCrossAreaConfig(std::vector<CrossAreaConfig> crossAreas);
    std::vector<double> crossesPosition;
    std::string getNyHeaderFromFre(std::string freq);

  private:
    bool writeFileJson(const char *path, Json json);
  private:
    MTFModelConfig();
    Json settingJsonObj;
    std::string filepath;
    ROIPosConfig roiPosInfo;
    std::vector<CrossAreaConfig *> crossAreaInfo;
    static MTFModelConfig *self;

    double focusLength = 10.0;
    double vid = 1000000;
    //std::vector<s_SingleTargetMatch> resultAutoROI;
};
void from_json(const nlohmann::json &, MTFLimitConfig &p);
