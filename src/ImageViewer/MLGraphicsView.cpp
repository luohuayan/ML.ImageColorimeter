#include "MLGraphicsView.h"
#include <QWheelEvent>
#include <QDebug>
#include <QToolTip>
#include <QScrollBar>
#include "RoiSetting.h"

#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()

MLGraphicsView::MLGraphicsView(QWidget*parent)
	: QGraphicsView(parent), m_Xaxis(nullptr), m_Yaxis(nullptr), m_gridItem(nullptr)
{
    setMouseTracking(true);
    connect(RoiSetting::instance(), &RoiSetting::roiSettingDone, this, &MLGraphicsView::updateRoiSetting);
    connect(this, &MLGraphicsView::sigInitRange, RoiSetting::instance(), &RoiSetting::initRanges);
    updateRoiSetting();
}

MLGraphicsView::~MLGraphicsView()
{}
void MLGraphicsView::setROIFlag(bool flag)
{
    m_viewRoiFlag = flag;
}
void MLGraphicsView::setROI9Flag(bool flag)
{
    m_viewRoi9Flag = flag;
}
void MLGraphicsView::ROIChanged(int x, int y, int width, int height)
{
    BGraphicsItem* b_item = (BGraphicsItem*)sender();
    ROIClass::instance()->updateROI(b_item->getName(), x, y, width, height);
    drawIndex(b_item);
    emit sigROIChanged(b_item->getName());
}
void MLGraphicsView::removeROI()
{
    BGraphicsItem* cropper = (BGraphicsItem*)sender();
    for (int i = m_crossAreas.size() - 1; i >= 0; i--) {
        BGraphicsItem* crp = m_crossAreas[i];
        if (crp->getName() == cropper->getName())
        {
            QGraphicsSimpleTextItem* txtitem = (QGraphicsSimpleTextItem*)cropper->property("INDEX_ITEM").value<void*>();
            if (txtitem != NULL)
            {
                delete txtitem;
                txtitem = NULL;
            }
            ROIClass::instance()->removeROI(crp->getName());
            scene()->removeItem(crp);
            emit sigROIChanged(crp->getName());
            m_crossAreas.removeOne(crp);
            delete crp;
            crp = nullptr;
            break;
        }
    }
   
}

void MLGraphicsView::removePartROI()
{
    BGraphicsItem* cropper = (BGraphicsItem*)sender();
    BGraphicsItem::ItemType itemType = cropper->getType();
    for (int i = m_crossAreas.size() - 1; i >= 0; i--) {
        BGraphicsItem* crp = m_crossAreas[i];
        if (crp->getType() == itemType)
        {
            QGraphicsSimpleTextItem* txtitem = (QGraphicsSimpleTextItem*)crp->property("INDEX_ITEM").value<void*>();
            if (txtitem != NULL)
            {
                delete txtitem;
                txtitem = NULL;
            }

            ROIClass::instance()->removeROI(crp->getName());
            scene()->removeItem(crp);
            emit sigROIChanged(crp->getName());
            m_crossAreas.removeAt(i);
            delete crp;
            crp = nullptr;
        }
    }
}

void MLGraphicsView::removeALLROI()
{
    for (int i = m_crossAreas.size() - 1; i >= 0; i--) {
        BGraphicsItem* crp = m_crossAreas[i];
        QGraphicsSimpleTextItem* txtitem = (QGraphicsSimpleTextItem*)crp->property("INDEX_ITEM").value<void*>();
        if (txtitem != NULL)
        {
            delete txtitem;
            txtitem = NULL;
        }
        ROIClass::instance()->removeROI(crp->getName());
        scene()->removeItem(crp);
        emit sigROIChanged(crp->getName());
        m_crossAreas.removeAt(i);
        delete crp;
        crp = nullptr;
    }
    QMap<QString, int>::iterator it;
    for (it = typeNumTotal.begin(); it != typeNumTotal.end(); ++it) {
        it.value() = 0;
    }
}

void MLGraphicsView::choosedROI(QString name)
{
    for (int i = m_crossAreas.size() - 1; i >= 0; i--) {
        BGraphicsItem* crp = m_crossAreas[i];
        if (crp->getName() == name) 
            crp->setFocus();
    }
}
void MLGraphicsView::parseJson(QString filePath)
{
    QFile file(filePath);
    file.open(QFile::ReadOnly);
    QByteArray json = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(json);
    if (!doc.isObject()) {
        qDebug() << "Read the data is not an object";
        return;
    }

    QJsonObject obj = doc.object();
    QStringList keys = obj.keys();
    for (int i = 0; i < keys.size(); i++) {
        QString key = keys[i];
        if (key == "Rectangle")
        {
            QJsonValue value = obj.value(key);
            if (value.isObject())
            {
                QJsonObject subObj = value.toObject();
                QStringList subkeys = subObj.keys();
                for (int jj = 0; jj < subkeys.size(); jj++)
                {
                    QString subkey = subkeys[jj];
                    QJsonValue subvalue = subObj.value(subkey);
                    if (subvalue.isObject())
                    {
                        QJsonObject sub2Obj = subvalue.toObject();
                        int x = sub2Obj["x"].toInt();
                        int y = sub2Obj["y"].toInt();
                        int w = sub2Obj["width"].toInt();
                        int h = sub2Obj["height"].toInt();
                        BGraphicsItem* Item = new BRectangle(x+w/2, y+h/2, w, h, QColor("#00FFFF"), BGraphicsItem::ItemType::Rectangle);
                        addRectItem(Item);
                    }
                }
            }   
        }
        else if (key == "Circle")
        {
            QJsonValue value = obj.value(key);
            if (value.isObject())
            {
                QJsonObject subObj = value.toObject();
                QStringList subkeys = subObj.keys();
                for (int jj = 0; jj < subkeys.size(); jj++)
                {
                    QString subkey = subkeys[jj];
                    QJsonValue subvalue = subObj.value(subkey);
                    if (subvalue.isObject())
                    {
                        QJsonObject sub2Obj = subvalue.toObject();
                        int x = sub2Obj["x"].toInt();
                        int y = sub2Obj["y"].toInt();
                        int w = sub2Obj["width"].toInt();
                        BGraphicsItem* Item = new BCircle(x+w/2, y+w/2, w/2, QColor("#00FFFF") , BGraphicsItem::ItemType::Circle);
                        addRectItem(Item);
                    }
                }
            }
        }
        else if (key == "Ellipse")
        {
            QJsonValue value = obj.value(key);
            if (value.isObject())
            {
                QJsonObject subObj = value.toObject();
                QStringList subkeys = subObj.keys();
                for (int jj = 0; jj < subkeys.size(); jj++)
                {
                    QString subkey = subkeys[jj];
                    QJsonValue subvalue = subObj.value(subkey);
                    if (subvalue.isObject())
                    {
                        QJsonObject sub2Obj = subvalue.toObject();
                        int x = sub2Obj["x"].toInt();
                        int y = sub2Obj["y"].toInt();
                        int w = sub2Obj["width"].toInt();
                        int h = sub2Obj["height"].toInt();
                        BGraphicsItem* Item = new BEllipse(x + w / 2, y + h / 2, w, h,QColor("#00FFFF"), BGraphicsItem::ItemType::Ellipse);
                        addRectItem(Item);
                    }
                }
            }
        }

    }
}
void MLGraphicsView::cropperMousePressEvent(QGraphicsSceneMouseEvent* event)
{
    BGraphicsItem* crp = (BGraphicsItem*)sender();
    drawIndex(crp);
}
void MLGraphicsView::updateRoiSetting()
{
    newROIset = RoiSetting::instance()->getRoiSettings();
}

void MLGraphicsView::addRectItem(BGraphicsItem* Item)
{
    QString  typeNow = transType(Item->getType());
    typeNumTotal[typeNow]++;
    Item->Index = typeNumTotal[typeNow];
    Item->setZValue(1);
    drawIndex(Item);
    scene()->addItem(Item);

    m_crossAreas.append(Item);
    ROI m_roi;
    QPointF pt = Item->boundingRect().topLeft();
    int w = Item->boundingRect().width();
    int h = Item->boundingRect().height();
    QPointF LTgen = Item->mapToScene(pt);
    int Xgen = static_cast<int>(LTgen.x());
    int Ygen = static_cast<int>(LTgen.y());

    m_roi.x = Xgen;
    m_roi.y = Ygen;
    m_roi.width = w;
    m_roi.height = h;
    m_roi.index = Item->Index;
    m_roi.name = Item->getName();
    m_roi.type = typeNow;
    ROIClass::instance()->setROI(m_roi);

    connect(Item, SIGNAL(ROIRegionChange(int, int, int, int)), this, SLOT(ROIChanged(int, int, int, int)));
    connect(Item, &BGraphicsItem::chooseItem, this, [=](QString name) {emit chooseItem(name); });
    connect(Item, SIGNAL(deleteOne()), this, SLOT(removeROI()));
    connect(Item, SIGNAL(deletePart()), this, SLOT(removePartROI()));
    connect(Item, SIGNAL(deleteAll()), this, SLOT(removeALLROI()));
    emit sigROIChanged(Item->getName());
}

QString MLGraphicsView::transType(BGraphicsItem::ItemType itemType)
{
    switch (itemType) {
    case BGraphicsItem::ItemType::Ellipse: {
        return "Ellipse";
    }
    case BGraphicsItem::ItemType::Circle: {
        return "Circle";
    }
    case BGraphicsItem::ItemType::Rectangle: {
        return "Rectangle";
    }
    default: break;
    }
}
void MLGraphicsView::arrowCursor()
{
    setCursor(Qt::ArrowCursor);
}
void MLGraphicsView::drawIndex(BGraphicsItem* Item)
{
    QGraphicsSimpleTextItem* oldTxtitem = (QGraphicsSimpleTextItem*)Item->property("INDEX_ITEM").value<void*>();
    if (oldTxtitem != NULL)
    {
        delete oldTxtitem;
        oldTxtitem = NULL;
    }
    BGraphicsItem::ItemType itemType = Item->getType();
    QString type;
    if (itemType == BGraphicsItem::ItemType::Circle)
        type = "C";
    if (itemType == BGraphicsItem::ItemType::Ellipse)
        type = "E";
    if (itemType == BGraphicsItem::ItemType::Rectangle)
        type = "R";

    int index = Item->Index;
    QGraphicsSimpleTextItem* txtitem = new QGraphicsSimpleTextItem(type + QString("").number(index).append("#"));
    //QFont font;
    //font.setPointSizeF(Item->boundingRect().width() / 30); // Adjust the division factor to control the font size

    Item->setName(txtitem->text());
    txtitem->setPen(QPen(Qt::red));
    txtitem->setBrush(QBrush((QColor(Qt::red))));
    txtitem->setFlag(QGraphicsItem::ItemIsMovable);
    //txtitem->setFont(font);
    QPointF m_gen = Item->getTopleft();
    txtitem->setPos(m_gen.x() - 20, m_gen.y() - 20);
    txtitem->setParentItem(Item);
    Item->setProperty("INDEX_ITEM", QVariant::fromValue((void*)txtitem));
}
void MLGraphicsView::setGridSize(GRIDSIZE size)
{
    m_gridSize = static_cast<int>(size);
}
qreal MLGraphicsView::getScale()
{
    return m_scale * 100;
}

void MLGraphicsView::EnableCross()
{
    UnenableCross();
    int height = scene()->height();
    int width = scene()->width();
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(5);
    m_Xaxis = new QGraphicsLineItem(QLineF(QPoint(0, height / 2), QPoint(width, height / 2)));
    m_Xaxis->setPen(pen);
    m_Yaxis = new QGraphicsLineItem(QLineF(QPoint(width / 2, 0), QPoint(width / 2, height)));
    m_Yaxis->setPen(pen);
    scene()->addItem(m_Xaxis);
    scene()->addItem(m_Yaxis);
}

void MLGraphicsView::UnenableCross()
{
    if (m_Xaxis)
    {
        scene()->removeItem(m_Xaxis);
        delete m_Xaxis;
        m_Xaxis = nullptr;
    }

    if (m_Yaxis)
    {
        scene()->removeItem(m_Yaxis);
        delete m_Yaxis;
        m_Yaxis = nullptr;
    }
}

void MLGraphicsView::EnableGrid()
{
    UnenableGrid();
    int sizeX = scene()->width() / m_gridSize;
    int sizeY = scene()->height() / m_gridSize;
    m_gridItem = new GridItem(sizeX, sizeY, Qt::lightGray, scene()->sceneRect());
    scene()->addItem(m_gridItem);
}

void MLGraphicsView::UnenableGrid()
{
    if (m_gridItem)
    {
        scene()->removeItem(m_gridItem);
        delete m_gridItem;
        m_gridItem = nullptr;
    }

}


void MLGraphicsView::wheelEvent(QWheelEvent* event)
{
	int wheelValue = event->angleDelta().y();
    wheelValue > 0 ? zoomIn() : zoomOut();
}

void MLGraphicsView::mousePressEvent(QMouseEvent* event)
{

	Qt::MouseButton btn = event->button();
	emit sigMousePress(event->pos(), event->button());

    m_bIsPressed = true;
    Qt::MouseButton mouseBtn = event->button();
    if (Qt::LeftButton == mouseBtn)
    {
        /* For panning the view */
        m_LastPanPoint = event->pos();

        if (horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible())
            setCursor(Qt::ClosedHandCursor);
        else if ((!horizontalScrollBar()->isVisible() && !verticalScrollBar()->isVisible()))
            setCursor(Qt::ArrowCursor);
        QPointF mousePos = mapToScene(event->pos());

        if (m_viewRoiFlag)
        {      
            createItem(mousePos);
        }
        else if(m_viewRoi9Flag)
        {
            int offset_factor = newROIset->value("offset").toInt();
            int offsetX = -100;
            int offsetY = -100;
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 3; col++) {
                    QPointF itemPos = mousePos + QPointF(col * offset_factor + offsetX, row * offset_factor + offsetY);
                    //QString name = QString("Cropper %1,%2").arg(row).arg(col);
                    //Cropper* rectItem = new Cropper(this, "", itemPos.x(), itemPos.y());
                    /*Cropper* rectItem = new Cropper(this, "", mousePos.x(), mousePos.y(), width, height, rgb);*/
                    createItem(itemPos);
                }
            }
        }     
    }
    QGraphicsView::mousePressEvent(event);

}
void MLGraphicsView::createItem(QPointF mousePos)
{
    int width = newROIset->value("width").toInt();
    int height = newROIset->value("height").toInt();
    QColor color = QColor(newROIset->value("color"));
    QString shape = newROIset->value("shape");
    if (shape == "Rectangle")
    {
        BGraphicsItem* Item = new BRectangle(mousePos.x(), mousePos.y(), width, height, color, BGraphicsItem::ItemType::Rectangle);
        addRectItem(Item);
    }
    else if (shape == "Circle")
    {
        BGraphicsItem* Item = new BCircle(mousePos.x(), mousePos.y(), width, color, BGraphicsItem::ItemType::Circle);
        addRectItem(Item);
    }
    else if (shape == "Ellipse")
    {
        BGraphicsItem* Item = new BEllipse(mousePos.x(), mousePos.y(), width, height, color, BGraphicsItem::ItemType::Ellipse);
        addRectItem(Item);
    }
}
void MLGraphicsView::mouseMoveEvent(QMouseEvent* event)
{

	emit sigMousePosChanged(event->pos(), event->button());
    QGraphicsView::mouseMoveEvent(event);

    if (m_bIsPressed && m_bIsMove)
    {
        if ((horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible()) && !m_bIsPressed)
            setCursor(Qt::OpenHandCursor);
        else if ((!horizontalScrollBar()->isVisible() && !verticalScrollBar()->isVisible()))
            setCursor(Qt::ArrowCursor);

        if (!m_LastPanPoint.isNull())
        {
            QPointF point = mapToScene(event->pos());
            QGraphicsItem* item = scene()->itemAt(point, transform());
            if (item != NULL) {
                QGraphicsPixmapItem* imgPix = dynamic_cast<QGraphicsPixmapItem*>(item);
                //if (imgPix != NULL) {
                    QPointF delta = mapToScene(event->pos()) - mapToScene(m_LastPanPoint);
                    m_LastPanPoint = event->pos();
                    translate(delta);
                //}
            }
            else {
                m_LastPanPoint = event->pos();
            }
        }
    }
}

void MLGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    emit sigMouseRelease(event->pos(), event->button());
    m_bIsPressed = false;
    if (horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible())
        setCursor(Qt::CrossCursor);
    m_LastPanPoint = QPoint();
    QGraphicsView::mouseReleaseEvent(event);
}
void MLGraphicsView::addROI(QPoint center)
{
    createItem(center);
}
void MLGraphicsView::translate(QPointF delta) {
    delta *= (m_scale* m_Lastscale);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(VIEW_WIDTH / 2 - delta.x(), VIEW_HEIGHT / 2 - delta.y());
    centerOn(mapToScene(newCenter));

    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

void MLGraphicsView::zoom(float scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    //if (factor < 0.05 || factor > 25)
    //    return;

    if (m_isInit)
    {
        scale(scaleFactor/ (m_Lastscale* m_scale), scaleFactor/ (m_Lastscale * m_scale));
        m_scale = 1.0;
        m_Lastscale = scaleFactor;
    }
    else
    {
        scale(scaleFactor, scaleFactor);
        m_scale *= scaleFactor;
    }
}

void MLGraphicsView::zoomIn()
{
    m_isInit = false;
    zoom(1 + m_zoomDelta);
}

void MLGraphicsView::zoomOut()
{
    m_isInit = false;
    zoom(1 - m_zoomDelta);
}

void MLGraphicsView::panAxesCilcked(bool isMove)
{
    m_bIsMove = isMove;
}

void MLGraphicsView::zoomInCilcked(bool isfirst,float sacle)
{
    m_isInit = isfirst;
    zoom(sacle);
}

void MLGraphicsView::zoomOutCilcked(bool isfirst,float sacle)
{
    m_isInit = isfirst;
    zoom(sacle);
}

void MLGraphicsView::restoreCilcked(bool isRestore)
{
    scale(1 / m_scale, 1 / m_scale);
    m_scale = 1.0;
    m_isInit = false;
}