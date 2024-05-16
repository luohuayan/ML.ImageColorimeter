#ifndef _MEASURESHOWCONFIG_H_
#define _MEASURESHOWCOBFIG_H_
#include <iostream>
#include <string>
#include <map>
#include "mlconfig_global.h"
#include "json.hpp"
using json = nlohmann::json;

typedef struct _ModuleInfo {
	int enable;
}ModuleInfo;

typedef struct _MotionWidget {
	int ndFilter;
	int colorFilter;
	int rx;
	int capture;
}MotionWidget;

typedef struct _FocusWidget {
	int cameraMotion;
	int LenMotion;
}FocusWidget;

typedef struct _MotionInfo {
	int enable;
	MotionWidget motionWidget;
}MotionInfo;

typedef struct _FocusInfo {
	int enable;
	FocusWidget focusWidget;
}FocusInfo;

typedef struct _ModuleShow {
	ModuleInfo ipdModule;
	MotionInfo motionModule;
	FocusInfo  focusModule;
	ModuleInfo cameraModule;
	ModuleInfo captureModule;
	ModuleInfo fourColorModule;
    ModuleInfo mtfMeasureModule;
}ModuleShow;

typedef struct _FilterArray
{
	std::vector<std::string> XYZArray;
	std::vector<std::string> NDArray;
	std::vector<std::string> CylinderArray;
}FilterArray;

typedef struct _cameraModule
{
	std::string name;
	std::string serial;
    std::string aperture;
}CameraModule;

typedef std::vector<std::string> stringArray;
typedef std::vector<CameraModule> cameraModuleArray;

class MLCONFIG_EXPORT MeasureShowConfig
{
	
public:
	static MeasureShowConfig* GetInstance();
	static void DeleteInstance();
	bool LoadMeasureConfig(std::string path);
	bool LoadFilterConfig(std::string path);
	bool LoadInstructmentModule(std::string path);
	bool LoadRxConfig(std::string path);
	ModuleShow GetModuleShowInfo();
	FilterArray GetFilterArray();
	stringArray GetCameraModuleArray();
    cameraModuleArray GetCameraModule();
	stringArray GetSphereArray();
private:
	MeasureShowConfig();
	~MeasureShowConfig();
	MeasureShowConfig(const MeasureShowConfig&)=delete;
	MeasureShowConfig& operator=(MeasureShowConfig&)=delete;
private:
	static MeasureShowConfig* m_measureShowCfg;
	ModuleShow m_moduleShow;
	FilterArray m_filterArray;
	stringArray m_cameraModuleNameArray;
	cameraModuleArray m_cameraModuleArray;
	stringArray m_sphereArray;
};
#endif

