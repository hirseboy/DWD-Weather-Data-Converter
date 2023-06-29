#include "DM_DataItem.h"

#include <QPen>
#include <QPainter>

namespace DM {

const Data &DataItem::data() const {
	return m_data;
}

QRectF DataItem::boundingRect() const {
	// Return the bounding rect for your item.
	// In this case, it is a m_sizexm_size square.
	QRectF rect(m_position.x() - 4*m_width,
				m_position.y() - 3.*m_width,
				6.0*m_width,
				m_size+2*m_width);

	return rect;
}

void DataItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

	if (m_data.m_minDate.secondsUntil(*m_minDate) < 0)
		return;


	if (m_data.m_maxDate.secondsUntil(*m_maxDate) > 0)
		return;

	// Draw two lines
	QPen pen(m_color);

	painter->setPen(pen);
	//painter->drawRect(boundingRect());
	pen.setWidth(m_width);
	painter->setPen(pen);
//	painter->setPen(Qt::NoPen);
//	painter->setBrush(m_color);

	double currentOpacity = opacity();
	double newOpacity = m_data.m_currentDistance > *m_maximumDistance ? 0.2 : 1.0;

	if (newOpacity != currentOpacity)
		setOpacity(newOpacity);

	Q_ASSERT(m_maximumDistance != nullptr);

	double offset = - m_width * 3;

	switch (m_data.m_type) {
	case Data::DT_TemperatureAndHumidity:	break;
	case Data::DT_Solar:					offset += m_width; break;
	case Data::DT_Wind:						offset += 2*m_width; break;
	case Data::DT_Pressure:					offset += 3*m_width; break;
	case Data::DT_Precipitation:			offset += 4*m_width; break;
	case Data::NUM_DT:											break;
	}

	// Adjust these values to change the lines
//	painter->drawLine(m_position.x() - m_size/2, m_position.y() - m_size/2,
//					  m_position.x() + m_size/2, m_position.y() + m_size/2);
	painter->drawLine(m_position.x() + offset, m_position.y() + m_size/2,
					  m_position.x() + offset, m_position.y() - m_size/2);

//	painter->drawEllipse(m_position.x() - m_size/2, m_position.y() - m_size/2,
//						 m_size, m_size);

	setZValue(50); // always on top

}
}
