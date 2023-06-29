/*	The Thermal Room Model
	Copyright (C) 2010  Andreas Nicolai <andreas.nicolai -[at]- tu-dresden.de>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDateTime>
#include <QDebug>
#include "DWDTimePlotPicker.h"

DWDTimePlotPicker::DWDTimePlotPicker(int xAxis, int yAxis, QWidget * canvas) :
	QwtPlotPicker(xAxis, yAxis, canvas)
{
}

void DWDTimePlotPicker::setYUnit(const QString& unit) {
	d_yUnit = unit;
}

const QString & DWDTimePlotPicker::yUnit() const {
	return d_yUnit;
}

QwtText DWDTimePlotPicker::trackerTextF(const QPointF & pos) const {
	QString text;

	// convert x position to date string
	QDate d(1970,1,1); // january first 2000, just some reference date
	QTime t(0,0,0);
	QDateTime dt(d,t, Qt::UTC); // a year without 29th of february
	size_t secs = (size_t)(pos.x()/1000);
	dt = dt.addSecs(secs);
//	QDateTime absdt = dt.toUTC();
	QString xText = dt.toString(tr("dd. MMM hh:mm:ss","Date/time format."));
	// convert y position to value + unit
	QString yText = QString("%1 %2").arg(pos.y(), 0, 'f', 2).arg(d_yUnit);

	switch(rubberBand())
	{
		case HLineRubberBand:
			text = xText;
			break;
		case VLineRubberBand:
			text = yText;
			break;
		default:
			text = QString("%1, %2").arg(xText).arg(yText);
	}

	QwtText theText(text, QwtText::RichText);
#ifdef Q_OS_WIN
	theText.setFont(QFont("Consolas"));
#elif defined(Q_OS_LINUX)
	theText.setFont(QFont("Monospace"));
#else
	theText.setFont(QFont("Monaco"));
#endif
	return theText;
}

void DWDTimePlotPicker::begin() {
	setTrackerMode(QwtPicker::AlwaysOff);
	QwtPlotPicker::begin();
}

bool DWDTimePlotPicker::end(bool ok) {
	setTrackerMode(QwtPicker::AlwaysOn);
	return QwtPlotPicker::end(ok);
}
