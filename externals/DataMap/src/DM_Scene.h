#ifndef DM_SceneH
#define DM_SceneH

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QDebug>
#include <QWheelEvent>

#include <IBK_Time.h>

#include "DM_Data.h"
#include "DM_DataItem.h"
#include "DM_DataItemGroup.h"

namespace DM {
class Scene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit Scene(QObject* parent);

	/*! Adds a data point to the scene. */
	void addDwdDataPoint(double *maxDist, IBK::Time *start, IBK::Time *end, const Data::DataType &type, unsigned int id, const QString & str, const IBK::Time &minDate, const IBK::Time &maxDate,
						 const double &lat, const double &lon);

	/*! Converts coordinates to a position at the scene. */
	QPointF convertCoordinatesToPos(const double &lat, const double &lon);

	/*! Sets an item group with all graphics scene items visivle or invisible. */
	void setItemGroupVisiblity(const Data::DataType &type, bool visible);

	/*! Item to draw additional information close to the cursor. */
	QGraphicsTextItem			*m_cursor;

	/*! Item that is holding the svg map (vector graphics. */
	QGraphicsItem*				m_mapSvgItem = nullptr;

	/*! Item that is drawn to show the current location. */
	QGraphicsEllipseItem*		m_locationItem = nullptr;

	/*! Data Group with all items, that are drawn inside the
		graphics scene, such as radiation, wind, precipitation, etc.
	*/
	QGraphicsItemGroup			*m_dataGroup[Data::NUM_DT];

	/*! Longitude of mouse position. */
	double						m_longitude;

	/*! Latitude of mouse position. */
	double						m_latitude;

	/*! Size of data point. */
	double						m_dataPointSize = 10;

	/*! Map with id of weather station to graphics item. */
	std::map<std::pair<DM::Data::DataType, unsigned int>, DataItem*> m_idToDataItem;

signals:
	/*! Is being emitted, when the location is updated. */
	void updatedLocation();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;



private:

};
}

#endif // DWDSCENE_H
