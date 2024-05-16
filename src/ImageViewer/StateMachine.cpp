#include "StateMachine.h"
#include <QMutexLocker>

StateMachine::StateMachine()
{
	m_ShowMode = ImageViewerShowContinuous;
	m_CIEx = 0.0f;
	m_CIEy = 0.0f;
	m_CIEY = 0;
}

StateMachine* StateMachine::m_StateMachine = nullptr;
StateMachine* StateMachine::instance()
{
	if (m_StateMachine == nullptr)
	{
		m_StateMachine = new StateMachine;
	}
	return m_StateMachine;
}

void StateMachine::setShowMode(const ImageModel& showMode)
{
	QMutexLocker locker(&m_Mutex);
	m_ShowMode = showMode;
}

ImageModel StateMachine::getShowMode()
{
	QMutexLocker locker(&m_Mutex);
	return m_ShowMode;
}

void StateMachine::setPixelPos(const QPoint& pixelPos)
{
	QMutexLocker locker(&m_Mutex);
	m_CurrPixelPos = pixelPos;
}

void StateMachine::getPixelPos(QPoint& pixelPos)
{
	QMutexLocker locker(&m_Mutex);
	pixelPos = m_CurrPixelPos;
}

void StateMachine::setGrayValue(const float& gray)
{
	QMutexLocker locker(&m_Mutex);
	m_gray = gray;
}

void StateMachine::getGrayValue(int& gray)
{
	QMutexLocker locker(&m_Mutex);
	gray = m_gray;
}

void StateMachine::setxyYValue(const float& x, const float& y, const float& Y) {
	QMutexLocker locker(&m_Mutex);
	m_CIEx = x;
	m_CIEy = y;
	m_CIEY = Y;
}

void StateMachine::getxyYValue(float& x, float& y, int& Y)
{
	QMutexLocker locker(&m_Mutex);
	x = m_CIEx;
	y = m_CIEy;
	Y = m_CIEY;
}

QString StateMachine::getTextShowInfo()
{
	if (m_ShowMode == ImageViewerShowContinuous)
	{
		return QString(" PixelCoords: x=%1,y=%2\n GrayValue=%3")
			.arg(m_CurrPixelPos.x())
			.arg(m_CurrPixelPos.y())
			.arg(m_gray);
	}
	else if (m_ShowMode == ImageViewerShowSingle)
	{
		return QString(" PixelCoords: x=%1,y=%2\n ChrmCoords: x=%3 y=%4 \n Lum: Y=%5")
			.arg(m_CurrPixelPos.x()).arg(m_CurrPixelPos.y())
			.arg(QString::number(m_CIEx, 'f', 5))
            .arg(QString::number(m_CIEy, 'f', 5))
            .arg(QString::number(m_CIEY, 'f', 5));
	}
	
}

