#ifndef DM_DataItemH
#define DM_DataItemH

#include <QGraphicsItem>
#include <QAbstractItemModel>

#include "DM_Data.h"

namespace DM {

/*! Custom Graphics Item for visualizing data. */
class DataItem : public QGraphicsItem
{
public:
	DataItem(QGraphicsItem* parent = nullptr)
		: QGraphicsItem(parent)
	{
		// You could add initialization code here if needed.
	}

	/*! Constructor. */
	DataItem(const QPointF &pos, const Data &data, const QColor &color, double &size):
		QGraphicsItem(nullptr),
		m_position(pos),
		m_color(color),
		m_size(size),
		m_data(data)
	{}

	/*! Data of Item. */
	const Data& data() const;

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	/*! Maximum distance. */
	double		*m_maximumDistance = nullptr;

	/*! Min Date */
	IBK::Time	*m_minDate;

	/*! Max Date */
	IBK::Time	*m_maxDate;

private:
	/*! Position of data item. */
	QPointF		m_position;

	/*! Color of data point. */
	QColor		m_color;

	/*! Height of data point. */
	double		m_size;

	/*! Data of graphics item. */
	Data		m_data;

	double		m_width = 3;

};

}

#endif // DM_DATAITEM_H
