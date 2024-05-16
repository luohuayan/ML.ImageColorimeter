#include "ImageViewerWidget.h"
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QDateTime>
#include "MLGraphicsView.h"
#include <QFileDialog>
#include <QGraphicsView>
#include <QToolButton>
#include "RoiSetting.h"
#include "StateMachine.h"

ImageViewerWidget::ImageViewerWidget(QWidget *parent)
	: QWidget(parent)/*, m_Xaxis(nullptr), m_Yaxis(nullptr), m_gridItem(nullptr)*/
{
	ui.setupUi(this);

	mousePress = false;

	initUI();
	initGraphics();
}

ImageViewerWidget::~ImageViewerWidget()
{}

void ImageViewerWidget::initUI()
{
	
	initToolBarAction();

	//ui.topWidget->setStyleSheet("QWidget{ background-color: #FFFFFF;}");
	ui.bottomTableWidget->setRowCount(1);
	ui.bottomTableWidget->setColumnCount(4);
	ui.bottomTableWidget->setRowHeight(0, 30);
	//if (!fpsItem)
	//{
	//	fpsItem = new QTableWidgetItem();
	//	fpsItem->setFlags(fpsItem->flags() & ~Qt::ItemIsEditable);
	//	ui.bottomTableWidget->setItem(0, 0, fpsItem);
	//}
	//if (!imagesErrorsItem)
	//{
	//	imagesErrorsItem = new QTableWidgetItem("Images:;Errors:");
	//	ui.bottomTableWidget->setItem(0, 1, imagesErrorsItem);
	//}
	if (!resolutionItem)
	{
		resolutionItem = new QTableWidgetItem();
		resolutionItem->setFlags(resolutionItem->flags() & ~Qt::ItemIsEditable);
		ui.bottomTableWidget->setItem(0, 0, resolutionItem);
	}
	if (!positionItem)
	{
		positionItem = new QTableWidgetItem();
		positionItem->setFlags(positionItem->flags() & ~Qt::ItemIsEditable);
		ui.bottomTableWidget->setItem(0, 1, positionItem);
	}
	if (!grayValueItem)
	{
		grayValueItem = new QTableWidgetItem();
		grayValueItem->setFlags(grayValueItem->flags() & ~Qt::ItemIsEditable);
		ui.bottomTableWidget->setItem(0, 2, grayValueItem);
	}
	if (!scaleItem)
	{
		scaleItem = new QTableWidgetItem();
		scaleItem->setFlags(scaleItem->flags() & ~Qt::ItemIsEditable);
		ui.bottomTableWidget->setItem(0, 3, scaleItem);
	}
	//ui.bottomTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui.bottomTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void ImageViewerWidget::initToolBarAction()
{
	QHBoxLayout* topWidgetLayout = new QHBoxLayout(ui.topWidget);
	ui.topWidget->setLayout(topWidgetLayout);
	//ui.topWidget->setStyleSheet("QWidget{background-color:#FFFFFF}");
	m_toolBar = new QToolBar(this);
	
	m_toolBar->setStyleSheet("QToolButton { padding-right: 30px; }"\
							 "QToolButton:hover,QToolButton:pressed,QToolButton:checked {background-color:#adceda;}"
	                         "QToolBar{background-color:#FFFFFF;color:#000000;}");
	m_toolBar->setFixedHeight(25);
	m_toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

	topWidgetLayout->addWidget(m_toolBar);
	topWidgetLayout->setMargin(0);

	QList<QAction*> actionList;
	QAction* actionSave = new QAction(m_toolBar);
	actionSave->setText("Save Image");
	m_toolBar->addAction(actionSave);
	actionSave->setCheckable(true);
	actionSave->setIcon(QIcon(":/icon/images/Save.png"));
	actionList.append(actionSave);

	QAction* actionSetting = new QAction(m_toolBar);
	actionSetting->setText("ArrowCursor");
	m_toolBar->addAction(actionSetting);
	actionSetting->setCheckable(true);
	actionSetting->setIcon(QIcon(":/icon/images/selection.png"));
	actionList.append(actionSetting);

	QAction* actionPanAxes = new QAction(m_toolBar);
	actionPanAxes->setText("PanAxes");
	m_toolBar->addAction(actionPanAxes);
	actionPanAxes->setCheckable(true);
	actionPanAxes->setIcon(QIcon(":/icon/images/Move.png"));
	actionList.append(actionPanAxes);

	QAction* actionZoomOut = new QAction(m_toolBar);
	actionZoomOut->setText("+");
	m_toolBar->addAction(actionZoomOut);
	actionZoomOut->setCheckable(true);
	actionZoomOut->setIcon(QIcon(":/icon/images/ZoomOut.png"));
	actionList.append(actionZoomOut);

	QAction* actionZoomIn = new QAction(m_toolBar);
	actionZoomIn->setText("-");
	m_toolBar->addAction(actionZoomIn);
	actionZoomIn->setCheckable(true);
	actionZoomIn->setIcon(QIcon(":/icon/images/ZoomIn.png"));
	actionList.append(actionZoomIn);

	QAction* actionCancelZoom = new QAction(m_toolBar);
	actionCancelZoom->setText("1:1");
	m_toolBar->addAction(actionCancelZoom);
	actionCancelZoom->setCheckable(true);
	actionCancelZoom->setIcon(QIcon(":/icon/images/1_1.png"));
	actionList.append(actionCancelZoom);

	QMenu* menu = new QMenu(m_toolBar);
	QAction* actionGirdLine = new QAction(m_toolBar);
	actionGirdLine->setText("Grid");
	actionGirdLine->setCheckable(true);
	actionGirdLine->setMenu(menu);
	actionGirdLine->setIcon(QIcon(":/icon/images/Grid.png"));
	actionList.append(actionGirdLine);
	m_toolBar->addAction(actionGirdLine);
	
	QAction* grid1 = menu->addAction(tr("1/2"));
	grid1->setCheckable(true);
	QAction* grid2 = menu->addAction(tr("1/5"));
	grid2->setCheckable(true);
	QAction* grid3 = menu->addAction(tr("1/10"));
	grid3->setCheckable(true);
	grid3->setChecked(true);
	QAction* grid4 = menu->addAction(tr("1/20"));
	grid4->setCheckable(true);
	QActionGroup* Actiongroup = new QActionGroup(this);
	Actiongroup->addAction(grid1);
	Actiongroup->addAction(grid2);
	Actiongroup->addAction(grid3);
	Actiongroup->addAction(grid4);
	connect(grid1, &QAction::triggered, this, [=]() {m_graphicsView->setGridSize(SIZE1); actionGirdLine->isChecked() ? m_graphicsView->EnableGrid() : m_graphicsView->UnenableGrid(); });
	connect(grid2, &QAction::triggered, this, [=]() {m_graphicsView->setGridSize(SIZE2); actionGirdLine->isChecked() ? m_graphicsView->EnableGrid() : m_graphicsView->UnenableGrid(); });
	connect(grid3, &QAction::triggered, this, [=]() {m_graphicsView->setGridSize(SIZE3); actionGirdLine->isChecked() ? m_graphicsView->EnableGrid() : m_graphicsView->UnenableGrid(); });
	connect(grid4, &QAction::triggered, this, [=]() {m_graphicsView->setGridSize(SIZE4); actionGirdLine->isChecked() ? m_graphicsView->EnableGrid() : m_graphicsView->UnenableGrid(); });
	Actiongroup->setExclusive(true);

	QAction* actionCenterPoint = new QAction(m_toolBar);
	actionCenterPoint->setText("Center");
	m_toolBar->addAction(actionCenterPoint);
	actionCenterPoint->setCheckable(true);
	actionCenterPoint->setIcon(QIcon(":/icon/images/Center.png"));
	actionList.append(actionCenterPoint);

	QMenu* menu2 = new QMenu(m_toolBar);
	QAction* roiSizeChange = menu2->addAction(tr("change roi"));
	roiSizeChange->setCheckable(true);
	QActionGroup* Actiongroup2 = new QActionGroup(this);
	Actiongroup2->addAction(roiSizeChange);
	Actiongroup2->setExclusive(true);
	connect(roiSizeChange, &QAction::triggered, this, [=]() { (RoiSetting::instance())->setVisible(true); });

	QAction* actionROI = new QAction(m_toolBar);
	actionROI->setText("ROI");
	actionROI->setCheckable(true);
	actionROI->setMenu(menu2);
	actionROI->setIcon(QIcon(":/icon/images/ROI.png"));
	actionList.append(actionROI);
	m_toolBar->addAction(actionROI);

	QAction* actionROI9 = new QAction(m_toolBar);
	actionROI9->setText("ROI-9");
	m_toolBar->addAction(actionROI9);
	actionROI9->setCheckable(true);
	actionROI9->setIcon(QIcon(":/icon/images/ROI-9.png"));
	actionList.append(actionROI9);

	QAction* actionLineCut = new QAction(m_toolBar);
	actionLineCut->setText("LineCut");
	m_toolBar->addAction(actionLineCut);
	actionLineCut->setCheckable(true);
	actionLineCut->setIcon(QIcon(":/icon/images/LineCut.png"));
	actionList.append(actionLineCut);
	m_LineCutAction = actionLineCut;

	QAction* actionRGB = new QAction(m_toolBar);
	actionRGB->setText("RGB");
	m_toolBar->addAction(actionRGB);
	actionRGB->setCheckable(true);
	actionRGB->setIcon(QIcon(":/icon/images/RGB.png"));
	actionList.append(actionRGB);

	QActionGroup* Actiongroup3 = new QActionGroup(this);
	for (int i = 0; i < actionList.size(); i++)
	{
		Actiongroup3->addAction(actionList.at(i));
		connect(actionList.at(i) , &QAction::toggled, this, &ImageViewerWidget::onActionToggled);
	}
	Actiongroup3->setExclusive(true);
}

void ImageViewerWidget::initGraphics()
{
	//m_Scene = new QGraphicsScene();
	m_Scene = new BQGraphicsScene();
	m_TextItem = new QGraphicsTextItem();
	m_PixmapItem = new QGraphicsPixmapItem();
	m_LineCutItem = new QGraphicsLineItem();

	m_Image = new QImage();
	m_pTimer = new QTimer();

	m_TextItem->setDefaultTextColor(Qt::white);
	//m_TextItem->hide();
	m_LineCutItem->setPen(QPen(QColor(59, 90, 255), 3));

	//m_LineCutItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
	
	m_LineCutItem->hide();
	m_Scene->addItem(m_PixmapItem);
	m_Scene->addItem(m_TextItem);
	
	m_Scene->addItem(m_LineCutItem);

	m_TextItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);

	m_graphicsView = new MLGraphicsView(this);
	graphlayout = new QHBoxLayout(ui.mainWidget);
	graphlayout->addWidget(m_graphicsView);
	ui.mainWidget->setLayout(graphlayout);
	m_graphicsView->setScene(m_Scene);
	m_graphicsView->setRenderHint(QPainter::Antialiasing);
	graphlayout->setMargin(0);

	connect((MLGraphicsView*)m_graphicsView, &MLGraphicsView::sigMousePosChanged, this, &ImageViewerWidget::onMousePosChanged);
	connect((MLGraphicsView*)m_graphicsView, &MLGraphicsView::sigMousePress, this, &ImageViewerWidget::onMousePress);
	connect((MLGraphicsView*)m_graphicsView, &MLGraphicsView::sigMouseRelease, this, &ImageViewerWidget::onMouseRelease);
	connect((MLGraphicsView*)m_graphicsView, &MLGraphicsView::chooseItem, this, &ImageViewerWidget::onChooseItem);

	connect((MLGraphicsView*)m_graphicsView, &MLGraphicsView::sigROIChanged, this, &ImageViewerWidget::onROIChanged);
	connect(m_pTimer,&QTimer::timeout,this, &ImageViewerWidget::handleTimeout);
	m_pTimer->start(200);

}


void ImageViewerWidget::onUpdateImage(QImage image,bool pseudo)
{
	//qDebug() << "ImageViewerWidget::onUpdateImage" << QThread::currentThread() << endl;
	*m_Image = image.copy();
	m_viewInfo.imageWidth = m_Image->width();
	m_viewInfo.imageHeight = m_Image->height();
	m_Scene->setSceneRect(0, 0, m_Image->width(), m_Image->height());
	m_PixmapItem->setPixmap(QPixmap::fromImage(*m_Image));
	if (pseudo)
	{
		colorMap = new ColorMap(this);
		colorMap->setMinMax(0,255);
		graphlayout->addWidget(colorMap);
	}
	else 
	{
		if (colorMap!=nullptr)
		{
			{
				graphlayout->removeWidget(colorMap);
				delete colorMap;
				colorMap = nullptr;
			}
		}
	}
	//emit m_graphicsView->sigInitRange(m_Image->width(), m_Image->height());
}


void ImageViewerWidget::onActionToggled(bool checked)
{
	QAction* action = static_cast<QAction*>(sender());

	qDebug() << "ImageViewerWidget::onActionToggled" << action->text() << action->isChecked();
	if (action->text() == "Save Image")
	{
		if(checked)
		{
			QString fileName = QFileDialog::getSaveFileName(
				this, QStringLiteral("Save image:"), "d:", QStringLiteral("(*.png *.jpg *.xpm  *.tif);;All files (*.*)"));
			if (!fileName.isEmpty())
			{
				emit sigSaveImageClicked(fileName);
				qDebug() << "ImageViewerWidget::onToolBarActionTrigge--->Save" << action->text();
			}
			action->setChecked(false);
		}
	}
	else if (action->text() == "ArrowCursor")
	{
		m_graphicsView->arrowCursor();
	}
	else if (action->text() == "LineCut")
	{
		emit sigActionCutLineChanged(action->isChecked());
		if (action->isChecked())
		{
			m_LineCutItem->show();
		}
		else
		{
			m_LineCutItem->hide();
		}
	}	
	else if (action->text() == "PanAxes")
	{
		m_graphicsView->panAxesCilcked(checked);
	}
	else if (action->text() == "+")
	{
		if(checked)
		{
			m_graphicsView->zoomInCilcked(false);
			action->setChecked(false);
		}
	}
	else if (action->text() == "-")
	{
		if (checked)
		{
			m_graphicsView->zoomOutCilcked(false);
			action->setChecked(false);
		}
	}
	else if(action->text() == "1:1")
	{
		if (checked)
		{
			m_graphicsView->restoreCilcked(checked);
			action->setChecked(false);
		}
	}
	else if (action->text() == "RGB")
	{
		emit sigUpdateRGBImage(checked);
	}
	else if (action->text() == "ROI")
	{
		m_graphicsView->setROIFlag(checked);
	}
	else if (action->text() == "ROI-9")
	{
		m_graphicsView->setROI9Flag(checked);
	}
	else if (action->text() == "Center")
	{
		checked ? m_graphicsView->EnableCross() : m_graphicsView->UnenableCross();
	}
	else if (action->text() == "Grid")
	{
		checked ? m_graphicsView->EnableGrid() : m_graphicsView->UnenableGrid();
	}
}
void ImageViewerWidget::onROIChanged(QString name)
{
	emit sigROIChanged(name);
}

void ImageViewerWidget::onChooseItem(QString name)
{
	emit sigItemChoosed(name);
}

void ImageViewerWidget::handleTimeout()
{
	m_viewInfo.scale = m_graphicsView->getScale();
	//fpsItem->setText(QString("fps:%1").arg(QString::number(m_viewInfo.fps)));
	resolutionItem->setText(QString("Size:%1x%2").arg(QString::number(m_viewInfo.imageWidth)).arg(QString::number(m_viewInfo.imageHeight)));
	positionItem->setText(QString("x:%1,y:%2").arg(QString::number(m_viewInfo.pos.x())).arg(QString::number(m_viewInfo.pos.y())));
	grayValueItem->setText(QString("Graylevel:%1").arg(QString::number(m_viewInfo.grayValue)));
	scaleItem->setText(QString("Scale:%1\%").arg(QString::number(m_viewInfo.scale)));

}

void ImageViewerWidget::onRowSelected(QString name)
{
	m_graphicsView->choosedROI(name);
}

void ImageViewerWidget::onJsonParse(QString filePath)
{
	m_graphicsView->parseJson(filePath);
}

void ImageViewerWidget::onAddRoi(QPoint center)
{
	m_graphicsView->addROI(center);
}

void ImageViewerWidget::onClearRoi()
{
	m_graphicsView->removeALLROI();
}
void ImageViewerWidget::updateCutLineInfo()
{
	if (m_LineCutAction->isChecked())
	{
		emit sigCutLineChanged(m_LineCutItem->line().p1().toPoint(), m_LineCutItem->line().p2().toPoint());
	}
}
void ImageViewerWidget::setImageModel(ImageModel model)
{
    if (model == ImageViewerShowContinuous)
    {
        m_toolBar->actions()[10]->setEnabled(false);
        m_toolBar->actions()[11]->setEnabled(false);
    }
    else if (model == ImageViewerShowSingle)
    {
        m_toolBar->actions()[10]->setEnabled(true);
        m_toolBar->actions()[11]->setEnabled(true);
    }
}
void ImageViewerWidget::setFrameRate(double fps)
{
	m_viewInfo.fps = fps;
}

void ImageViewerWidget::onMousePosChanged(QPoint pos, Qt::MouseButton btn)
{
	//qDebug() << "ImageViewerWidget::onMousePosChanged" << mousePress  << btn << endl;
	QPointF scenePos = m_graphicsView->mapToScene(pos);
	QPointF itemPos = m_PixmapItem->mapFromScene(scenePos);
	emit sigMousePosChanged(itemPos.toPoint());
	if (mousePress)
	{
		m_LineCutItem->setLine(m_LineCutItem->line().x1(), m_LineCutItem->line().y1(), scenePos.x(), scenePos.y());
	}

	StateMachine::instance()->setPixelPos(itemPos.toPoint());
	m_TextItem->setPos(scenePos);
	if (itemPos.x() < 0 || itemPos.x() >= m_Image->size().width() || itemPos.y() < 0 || itemPos.y() >= m_Image->height())
	{
        m_TextItem->hide();
	}

    m_viewInfo.pos = scenePos.toPoint();

}

void ImageViewerWidget::onMousePress(QPoint pos, Qt::MouseButton btn)
{
	if (btn == Qt::LeftButton)
	{
		mousePress = true;
		QPointF scenePos = m_graphicsView->mapToScene(pos);
		m_LineCutItem->setLine(scenePos.x(), scenePos.y(), scenePos.x(), scenePos.y());
		
	}
	
	
}

void ImageViewerWidget::onMouseRelease(QPoint pos, Qt::MouseButton btn)
{
	QPointF scenePos = m_graphicsView->mapToScene(pos);
	QPointF itemPos = m_PixmapItem->mapFromScene(scenePos);

	if (btn == Qt::LeftButton)
	{
		mousePress = false;
		
		m_LineCutItem->setLine(m_LineCutItem->line().x1(), m_LineCutItem->line().y1(), scenePos.x(), scenePos.y());
		updateCutLineInfo();
	}
	
	QPoint f = scenePos.toPoint();
	//m_viewInfo.pos = f;
	if (0 < f.x() && f.x() < m_Image->width() && 0 < f.y() && f.y() < m_Image->height())
	{
		//QColor color = m_Image->pixelColor(itemPos.toPoint());
		//QString info = QString("  x:%5,y:%6\n  Mono %1, %2 %3 %4").arg(qGray(color.red(), color.green(), color.blue())).arg(color.red()).arg(color.green()).arg(color.blue()).arg(QString::number(scenePos.toPoint().x())).arg(QString::number(scenePos.toPoint().y()));
		//m_viewInfo.grayValue = qGray(color.red(), color.green(), color.blue());

	}
	else
	{
		m_TextItem->hide();
	}
}
void ImageViewerWidget::updateTextShowInfo(QPoint itemPos) {
    if (itemPos.x() >= 0 && itemPos.x() < m_Image->size().width() &&
        itemPos.y() >= 0 && itemPos.y() < m_Image->height()) {
        QString info = StateMachine::instance()->getTextShowInfo();
        m_TextItem->setPlainText(info);
        m_TextItem->setDefaultTextColor(Qt::red);
        m_TextItem->show();
        StateMachine::instance()->getGrayValue(m_viewInfo.grayValue);
    } 
}

void ImageViewerWidget::InitializeImageArea()
{
	float a = ui.mainWidget->width();
	float b = ui.mainWidget->height();
	bool width = (m_Image->width() > ui.mainWidget->width()) ? true : false;
	bool height = (m_Image->height() > ui.mainWidget->height()) ? true : false;
	float w = float(ui.mainWidget->width()) / float(m_Image->width());
	float h = float(ui.mainWidget->height()) / float(m_Image->height());
	float scaleFactor = qMin(w, h);
	if (width || height)
	{
		m_graphicsView->zoomOutCilcked(true, scaleFactor);
	}
	else if (!width && !height)
	{
		m_graphicsView->zoomInCilcked(true, scaleFactor);
	}
}
