#ifndef DWDPLOTZOOMER_H
#define DWDPLOTZOOMER_H

#include "qwt_plot_zoomer.h"
#include "qwt_plot.h"

class DWDPlotZoomer : public QwtPlotZoomer {

public:
	DWDPlotZoomer(QWidget *widget) :
		QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, widget, true)
	{}

	QwtText trackerText(const QPoint &/*pos*/) const {
		return QwtText("");
	}

};
#endif // DWDPLOTZOOMER_H
