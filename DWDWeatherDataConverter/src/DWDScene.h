#ifndef DWDSCENE_H
#define DWDSCENE_H

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QDebug>
#include <QWheelEvent>

class DWDScene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit DWDScene(QObject* parent) :
		QGraphicsScene(parent)
	{
		m_cursor = new QGraphicsTextItem("0, 0"); //Fixed at 0, 0
		addItem(m_cursor);
		m_cursor->setPos(0,0);
		m_cursor->setPos(0,0);
	}

	void mousePressEvent(QGraphicsSceneMouseEvent *event) override {

		QPointF point = event->scenePos();

		removeItem(m_locationItem);
		double thickness = 3;

		QGraphicsEllipseItem *item = new QGraphicsEllipseItem(point.x() - m_radius, point.y() - m_radius, 2*m_radius, 2*m_radius);
		item->setPen(QPen(Qt::black, 3));
		QColor color = Qt::red;
		color.setAlpha(200);
		item->setBrush(color);
		addItem(item);
		m_locationItem = item;

		//addEllipse(point.x()-5, point.y()-5, 10, 10, QPen(), QBrush(Qt::red));

	}


	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override 	{

		int height = m_mapItem->boundingRect().size().height();
		int width = m_mapItem->boundingRect().size().width();

		qDebug() << "Scene Size:" << height << " | " << width;

		QPointF pointPos = event->scenePos();
		QPointF mapPos = m_mapItem->pos();
		QPointF finalPos = pointPos - mapPos;

		m_latitude  = 47.271679 + ( height - finalPos.y() ) / height * ( 55.05864  - 47.271679 );
		m_longitude = 15.043611 - ( width  - finalPos.x() ) / width  * ( 15.043611 -  5.866944 );

		QString string = QString("%1° N,\n%2° O")
				.arg(m_latitude)
				.arg(m_longitude);

		m_cursor->setPlainText( string );

		// Set the new position of the cursor
		m_cursor->setPos(event->scenePos().x() + 20, event->scenePos().y() + 20 );
	}

	QGraphicsTextItem			*m_cursor;
	double						m_latitude;
	double						m_longitude;
	double						m_radius = 50;

	QGraphicsItem*				m_mapItem = nullptr;
	QGraphicsItem*				m_locationItem = nullptr;


	// QGraphicsScene interface
protected:
};


#endif // DWDSCENE_H
