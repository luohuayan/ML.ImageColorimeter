#pragma once

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include "cropper.hpp"
#include "ImageViewerStructure.h"
#include "GridItem.h"
#include <QGraphicsPixmapItem>
#include "bqgraphicsitem.h"
enum GRIDSIZE
{
	SIZE1 = 2,
	SIZE2 = 5,
	SIZE3 = 10,
	SIZE4 = 20
};
class MLGraphicsView  : public QGraphicsView
{
	Q_OBJECT
	
public:
	MLGraphicsView(QWidget*parent);
	~MLGraphicsView();
	void EnableCross();
	void UnenableCross();
	void EnableGrid();
	void UnenableGrid();
	void setROIFlag(bool);
	void setROI9Flag(bool);
	void addRectItem(BGraphicsItem* Item);
	void addROI(QPoint center);
	void panAxesCilcked(bool isMove);
	void zoomInCilcked(bool isfirst, float sacle = 1.2);
	void zoomOutCilcked(bool isfirst, float sacle = 0.8);
	void restoreCilcked(bool isRestore);
	qreal getScale();
	void drawIndex(BGraphicsItem* Item);
	void setGridSize(GRIDSIZE size);
	void createItem(QPointF mousePos);
	QString transType(BGraphicsItem::ItemType itemType);
	void arrowCursor();
protected:
	virtual void wheelEvent(QWheelEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	void zoomIn();
	void zoomOut();
	void zoom(float scaleFactor);
	void translate(QPointF delta);

signals:
	void sigMousePosChanged(QPoint pos, Qt::MouseButton btn);
	void sigMousePress(QPoint pos, Qt::MouseButton btn);
	void sigMouseRelease(QPoint pos, Qt::MouseButton btn);
	void sigROIChanged(QString);
	void sigInitRange(int,int);
	void chooseItem(QString);
	
public slots:
	void ROIChanged(int x, int y, int width, int height);
	void removeROI();
	void removePartROI();
	void removeALLROI();
	void cropperMousePressEvent(QGraphicsSceneMouseEvent* event);
	void updateRoiSetting();
	void choosedROI(QString name);
	void parseJson(QString filePath);
private:
	bool m_viewRoiFlag = false;
	bool m_viewRoi9Flag = false;
	int curROIIdx = 0;
	int m_gridSize = 10;
	QPointF startPoint;
	QRectF rect;
	QList<BGraphicsItem*> m_crossAreas;
	QList<ROI> rectAreas;
	QGraphicsLineItem* m_Xaxis;
	QGraphicsLineItem* m_Yaxis;
	GridItem* m_gridItem;
	QPoint m_LastPanPoint;
	qreal m_scale = 1.0;
	qreal m_Lastscale = 1.0;
	qreal m_zoomDelta = 0.2;
	bool m_bIsPressed = false;
	bool m_bIsMove = false;
	bool m_isInit = false;

	QMap<QString, QString>* newROIset = new QMap<QString, QString>;
	QMap<QString, int> typeNumTotal = { {"Rectangle",0},{"Circle",0},{"Ellipse",0}};
};
