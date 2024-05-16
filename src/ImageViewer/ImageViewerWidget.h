#pragma once

#include <QWidget>
#include <QToolBar>
#include "ui_ImageViewerWidget.h"
#include <QGraphicsView>
#include "MLGraphicsView.h"
#include <QLabel>
#include <QTimer>
#include "bqgraphicsscene.h"
#include "colorMap.h"
class MLGraphicsView;
class QGraphicsScene;
class QGraphicsTextItem;
class QGraphicsPixmapItem;
class QGraphicsLineItem;
class QImage;

struct ViewInfo
{
	double fps;
	int imageCount;
	int errorCode;
	int imageWidth;
	int imageHeight;
	int grayValue;
	int scale;
	QPoint pos;
};

class ImageViewerWidget : public QWidget
{
	Q_OBJECT

public:
	ImageViewerWidget(QWidget *parent = nullptr);
	~ImageViewerWidget();
	void initUI();
	void initToolBarAction();
	void initGraphics();

private:
	void InitializeImageArea();
	void updateCutLineInfo();
signals:
	void sigROIChanged(QString);
	void sigCutLineChanged(QPoint start, QPoint end);
	void sigActionCutLineChanged(bool checked);

	void actionROI9trigger(bool);
	void actionROItrigger(bool);
	void sigSaveImageClicked(QString);
	void sigUpdateRGBImage(bool);
	void sigMousePosChanged(QPoint pixmapItemPos);
	void sigItemChoosed(QString);
  public slots:
	void onMousePosChanged(QPoint pos, Qt::MouseButton btn);
	void onMousePress(QPoint pos, Qt::MouseButton btn);
	void onMouseRelease(QPoint pos, Qt::MouseButton btn);
	void onUpdateImage(QImage image, bool pseudo);
	void onActionToggled(bool checked);
	void onROIChanged(QString);
	void onChooseItem(QString);
	void handleTimeout();
	void onRowSelected(QString);
	void onJsonParse(QString);
	void onAddRoi(QPoint center);
	void onClearRoi();
	void setFrameRate(double fps);
    void setImageModel(ImageModel model);
    void updateTextShowInfo(QPoint itemPos);

  private:
	Ui::ImageViewerWidgetClass ui;
	QToolBar* m_toolBar = nullptr;
	MLGraphicsView* m_graphicsView = nullptr;
	BQGraphicsScene* m_Scene = nullptr;
	QGraphicsTextItem *m_TextItem = nullptr;
	QGraphicsPixmapItem* m_PixmapItem = nullptr;
	QImage *m_Image = nullptr;
	QTimer* m_pTimer = nullptr;
	ViewInfo m_viewInfo = {0,0,0,0,0,0,0,QPoint(0,0)};
	QTableWidgetItem* fpsItem = nullptr;
	//QTableWidgetItem* imagesErrorsItem = nullptr;
	QTableWidgetItem* resolutionItem = nullptr;
	QTableWidgetItem* positionItem = nullptr;
	QTableWidgetItem* grayValueItem = nullptr;
	QTableWidgetItem* scaleItem = nullptr;
	QGraphicsLineItem* m_LineCutItem = nullptr;
	QAction* m_LineCutAction = nullptr;
	QHBoxLayout* graphlayout = nullptr;
	ColorMap* colorMap = nullptr;
	bool mousePress;
};
