#include "DM_Scene.h"
#include "DM_Conversions.h"
#include "DM_DataItem.h"

#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QGraphicsItemGroup>
#include <QGraphicsDropShadowEffect>

namespace DM {
Scene::Scene(QObject * parent) :
	QGraphicsScene(parent)
{
	m_cursor = new QGraphicsTextItem("0, 0"); //Fixed at 0, 0
	addItem(m_cursor);
	m_cursor->setPos(0,0);

	// Initialize all item groups
	for (unsigned int i=0; i<Data::NUM_DT; ++i) {
		m_dataGroup[(Data::DataType)i] = new DataItemGroup;
		addItem(m_dataGroup[(Data::DataType)i]);
	}

	// Add svg renderer and load map of germany as svg
	QSvgRenderer *renderer = new QSvgRenderer(this);

	// load svg
	renderer->load(QString(":/gfx/Karte_Deutschland.svg"));
	renderer->setFramesPerSecond(60);

	// construct an graphics item and then we set the renderer
	QGraphicsSvgItem *item = new QGraphicsSvgItem();
	//item->setSharedRenderer(renderer);
	item->setZValue(-99);
	item->setPos(0,0);
	addItem(item); // add the item to the scene

	//QGraphicsEllipseItem *ellipse = addEllipse(0-10,0-10,20,20, QPen(Qt::red), Qt::blue);
	// Hold the pointer
	m_mapSvgItem = new QGraphicsSvgItem(":/gfx/Karte_Deutschland.svg");
	addItem(m_mapSvgItem);

	m_locationItem = new QGraphicsEllipseItem(-10,-10,20,20);
	// m_locationItem->setBrush(Qt::black);
	m_locationItem->setPen(QPen(Qt::black, 1));
	m_locationItem->setZValue(100);

	// Erstellen Sie den Schatteneffekt und konfigurieren Sie ihn
	QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
	effect->setOffset(0);  // Setzen Sie den Offset des Schattens
	effect->setBlurRadius(20);  // Setzen Sie den Unschärferadius

	// Fügen Sie den Schatteneffekt zum Ellipsenobjekt hinzu
	m_locationItem->setGraphicsEffect(effect);

	addItem(m_locationItem);
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent * event) {
	if (event->button() == Qt::LeftButton) {
		QPointF pos = event->scenePos() - m_locationItem->scenePos();

		m_locationItem->moveBy(pos.x(), pos.y());

		int height = m_mapSvgItem->boundingRect().size().height();
		int width = m_mapSvgItem->boundingRect().size().width();

		convertPosToCoordinates(event->scenePos() + m_mapSvgItem->scenePos(), height, width, m_latitude, m_longitude);

		emit updatedLocation();
	}
	QGraphicsScene::mousePressEvent(event);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

	int height = m_mapSvgItem->boundingRect().size().height();
	int width = m_mapSvgItem->boundingRect().size().width();

	QPointF pointPos = event->scenePos() + m_mapSvgItem->pos();

	convertPosToCoordinates(pointPos, height, width, m_latitude, m_longitude);

	QString string = QString("%1° N,\n%2° O")
			.arg(m_latitude)
			.arg(m_longitude);

	m_cursor->setPlainText(string);
	m_cursor->setZValue(99); // always on top
	m_cursor->setDefaultTextColor(Qt::black);

	// Set the new position of the cursor
	m_cursor->setPos(event->scenePos().x() + 10, event->scenePos().y() + 10 );

	QGraphicsScene::mouseMoveEvent(event);

}

void Scene::addDwdDataPoint(double *maxDist, IBK::Time *start, IBK::Time *end, const Data::DataType & type, unsigned int id, const QString &str, const IBK::Time &minDate, const IBK::Time &maxDate, const double & lat, const double & lon) {
	Data data(str, minDate, maxDate, lat, lon, type);

	QPointF pos = ::DM::convertCoordinatesToPos(m_mapSvgItem->boundingRect(), lat, lon);

	DataItem *item = new DataItem(pos, data, colorFromDataType(type), m_dataPointSize);
	m_dataGroup[type]->addToGroup(item);

	item->m_maximumDistance = maxDist;
	item->m_minDate = start;
	item->m_maxDate = end;

	std::pair<DM::Data::DataType, unsigned int> key (type, id);

	m_idToDataItem[key] = item;

	update();
}

void Scene::setItemGroupVisiblity(const Data::DataType &type, bool visible) {
	m_dataGroup[type]->setVisible(visible);
}


}
