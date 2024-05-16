#ifndef CONFIGPARAM_H
#define CONFIGPARAM_H
#include <string>
#include <QObject>
using namespace std;

typedef struct  PLCControlConfig
{
	string ipAddress;
	int rock;
	int slot;
	int dataBaseNumber;
	map<string, string> tip_tilt;

};

typedef struct Motion3DConfig
{
	string ipAddress1;
	string type1;
	string ipAddress2;
	string type2;
};

typedef struct LevelAdjustConfig
{
	double motion3DX;
	double motion3DY;
	double motion3DZ;
	double angleRange;
	int adjustTimesMax;
	double pulseConversionAngle_X;
	double pulseConversionAngle_Y;
};

typedef struct GradienterConfig
{
	string ConnectAddress;
	string Type;
};

typedef struct AllConfig
{
	PLCControlConfig plcControl;
	Motion3DConfig motion3DConfig;
	LevelAdjustConfig levelAdjustConfig;
	GradienterConfig gradienterConfig;
};

typedef enum ConfigMode
{
	PLC_Config,
	Motion3D_Config,
	LevelAdjust_Config,
	Gradienter_Config
};

#endif