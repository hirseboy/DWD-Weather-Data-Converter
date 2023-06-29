#include "DM_DataItemGroup.h"
#include "DM_Conversions.h"
#include "DM_DataItem.h"

#include <QPainter>

namespace DM {

DataItemGroup::DataItemGroup(const QRectF rect, const QString & str, const IBK::Time &minDate, const IBK::Time &maxDate,
				   const double & lat, const double & lon, const Data::DataType &type, int radius) {
}

void DataItemGroup::moveItem(const QPointF & pos) {
	setPos(pos);
}

}
