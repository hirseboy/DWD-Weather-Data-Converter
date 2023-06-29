#ifndef DM_DATAITEMGROUP_H
#define DM_DATAITEMGROUP_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <QPointF>

#include <IBK_Time.h>

#include "DM_Data.h"


namespace DM {

/*! Class for all Graphics Items in Scene. */
class DataItemGroup : public QGraphicsItemGroup
{
public:

	DataItemGroup() {}

	DataItemGroup(const QRectF rect, const QString &str, const IBK::Time &minDate, const IBK::Time &maxDate,
			 const double &lat, const double &lon, const Data::DataType & type, int radius = 10);

	/*! Moves the item to a current position. */
	void moveItem(const QPointF &pos);

};

}
#endif // DM_DATAITEMGROUP_H
