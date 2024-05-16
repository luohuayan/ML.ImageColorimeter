#pragma once

#include "ImageViewerStructure.h"
#include <QMutex>

class StateMachine
{
public:
	static StateMachine* instance();
	void setShowMode(const ImageModel& showMode);
	ImageModel getShowMode();
	void setPixelPos(const QPoint &pixelPos);
	void getPixelPos(QPoint& pixelPos);
    void setGrayValue(const float& gray);
	void getGrayValue(int& gray);
    void setxyYValue(const float& x, const float& y, const float& Y);
	void getxyYValue(float& x, float& y, int& Y);
	QString getTextShowInfo();

private:
	StateMachine();
private:
	static StateMachine* m_StateMachine;
	QMutex m_Mutex;
	ImageModel m_ShowMode;

	float m_gray;
	float m_CIEx;
	float m_CIEy;
	float m_CIEY;
	QPoint m_CurrPixelPos;
};

