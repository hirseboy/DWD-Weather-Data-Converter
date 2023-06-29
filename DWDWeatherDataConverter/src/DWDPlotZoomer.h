#ifndef DWDDateTimeScaleDrawH
#define DWDDateTimeScaleDrawH

#include <qwt_plot_zoomer.h>
#include <qwt_plot_canvas.h>
#include <qwt_text.h>

#include <QCoreApplication>
#include <QDateTime>

class QPainter;
class QPalette;

class DWDPlotZoomer: public QwtPlotZoomer
{
public:
	DWDPlotZoomer(QWidget *canvas):
		QwtPlotZoomer(canvas)
	{}

protected:
	virtual QwtText trackerTextF(const QPointF &pos) const
	{
		QDateTime dt = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(pos.x()));
		return QString("%1, %2")
				.arg(dt.toString("yyyy-MM-dd"))
				.arg(pos.y());
	}
};

#endif // QwtDateTimeScaleDrawH
