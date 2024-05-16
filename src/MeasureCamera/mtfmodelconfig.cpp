#include "mtfmodelconfig.h"
#include "LoggingWrapper.h"

MTFModelConfig *MTFModelConfig::self = nullptr;
MTFModelConfig *MTFModelConfig::instance()
{
    if (self == nullptr)
    {
        self = new MTFModelConfig();
    }
    return self;
}

void MTFModelConfig::load(const char *path)
{
    filepath = path;
    std::ifstream jsonFile(filepath);
    if (jsonFile.is_open())
    {
        std::string contents =
            std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();
        if (contents.empty())
        {
            LoggingWrapper::getinstance()->Warn("[--MTFMeasure--]:MTFModelConfig file is empty");
            return;
        }
        settingJsonObj = Json::parse(contents);
        LoggingWrapper::getinstance()->Error("[--MTFMeasure--]:MTFModelConfig file open success");
    }
}

ROIPosConfig MTFModelConfig::getROIPosCfg()
{
    return roiPosInfo;
}
double MTFModelConfig::getFocusLength()
{
    Json& focusLength = settingJsonObj["FocusLength"];
    if (!focusLength.is_null())
    {
        return focusLength.get<double>();
    }
    LoggingWrapper::getinstance()->Warn("[--MTFMeasure--]:FocusLength is null");
    return -1;
}
double MTFModelConfig::getROIcenterOffset()
{
    Json& roicenterOffset = settingJsonObj["ROIcenterOffset"];
    if (!roicenterOffset.is_null())
    {
        return roicenterOffset.get<double>();
    }
    LoggingWrapper::getinstance()->Warn("[--MTFMeasure--]:ROIcenterOffset is null");
    return -1;
}

std::vector<CrossAreaConfig *> MTFModelConfig::getCrossAreaConfig()
{
    crossAreaInfo.clear();
    for (auto iter = settingJsonObj.begin(); iter != settingJsonObj.end(); ++iter)
    {
        std::string  key = iter.key();
        if (key == "FocusLength" || key == "ROIcenterOffset")
            continue;
         Json  areas = iter.value();
         for (auto subElem : areas.items())
         {
             CrossAreaConfig* cross = new CrossAreaConfig();
             cross->name = subElem.key();
             cross->FromX = subElem.value()["x"];
             cross->FromY = subElem.value()["y"];
             cross->aoiWidth = subElem.value()["width"];
             cross->aoiHeight = subElem.value()["height"];
             crossAreaInfo.push_back(cross);
         }
    }
    return crossAreaInfo;
}
void MTFModelConfig::setROIPosCfg(ROIPosConfig cfg)
{
    roiPosInfo = cfg;
}

std::vector<std::string> MTFModelConfig::getSpecifedFreq()
{
    Json &specifedFreqs = settingJsonObj["SpecifedFreq"];
    if (specifedFreqs.is_array())
    {
        return specifedFreqs.get<std::vector<std::string>>();
    }
    return std::vector<std::string>();
}

void from_json(const nlohmann::json &j, MTFLimitConfig &p)
{
    j.at(0).get_to(p.freqId);
    j.at(1).get_to(p.lowerLimit);
    j.at(2).get_to(p.upperLimit);
}
std::vector<MTFLimitConfig> MTFModelConfig::getMTFLimit(int crossImgIdx)
{
    std::vector<MTFLimitConfig> result;
    Json &thresholds = settingJsonObj["threshold"];
    if (!thresholds.is_null())
    {
        std::string key = std::to_string(crossImgIdx);
        if (key != "")
        {
            Json val = thresholds.at(key);
            result = val.get<std::vector<MTFLimitConfig>>();
        }
    }
    std::vector<std::string> specFreqs = getSpecifedFreq();
    for (int i = 0; i < result.size(); i++)
    {
        if (i < specFreqs.size())
        {
            result.at(i).freqVal = specFreqs.at(i);
        }
    }

    return result;
}

void MTFModelConfig::setFocusLength(double val)
{
    focusLength = val;
}

double MTFModelConfig::getVid()
{
    return vid;
}

void MTFModelConfig::setVid(double val)
{
    vid = val;
}

std::string MTFModelConfig::getDisplayResultFreq()
{
    Json &displayFreq = settingJsonObj["DisplayFreq"];
    if (!displayFreq.is_null())
    {
        return displayFreq.get<std::string>();
    }
    LoggingWrapper::getinstance()->Warn("[--MTFMeasure--]:DisplayFreq is null");
    return "";
}

Json MTFModelConfig::getJsonObj()
{
    return settingJsonObj;
}

MTFModelConfig::MTFModelConfig()
{
}

MTFModelConfig::~MTFModelConfig()
{
    if (self != nullptr)
        delete self;
}

std::string MTFModelConfig::getNyHeaderFromFre(std::string freq)
{
    Json& freqMap = settingJsonObj["nyfreqmap"];
    if (!freqMap.is_null())
    {
        return freqMap[freq].get<std::string>();
    }
    LoggingWrapper::getinstance()->Warn("[--MTFMeasure--]:Nyfreqmap is null");
    return "";
}

bool MTFModelConfig::writeFileJson(const char *path, Json json)
{
    std::ofstream ofs(path, std::ios_base::out);
    if (!ofs.is_open())
    {
        LoggingWrapper::getinstance()->Error("[--MTFMeasure--]:Write to json failed");
        return false;
    }
    ofs << json.dump(4);
    ofs.close();
    return true;
}