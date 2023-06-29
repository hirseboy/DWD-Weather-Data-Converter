#include "DWDDateTimeScaleEngine.h"

#include <QDateTime>
#include <QPainter>
#include <QFontMetrics>
#include <QPalette>

#include "DWDUtilities.h"

QwtScaleDiv DWDDateTimeScaleEngine::divideScale(double x1, double x2,
		int , int , double ) const
{
	// we expect x1 and x2 to be in units [d]

	unsigned int mon1 = month(x1);
	unsigned int mon2 = month(x2);

	QwtScaleDiv scaleDiv;

	QwtInterval interval = QwtInterval(x1, x2).normalized();
	QList<double> ticks[QwtScaleDiv::NTickTypes];

	// if we span more than 3 month, use month scale
	// MajorTicks are month, MinorTicks are weeks
	if (mon2 - mon1 > 3) {
		// first tick is at
		if (x1 == 0) // hour 0
			ticks[QwtScaleDiv::MajorTick] += 0;
		for (unsigned int i=mon1; i<=mon2; ++i)
			ticks[QwtScaleDiv::MajorTick] += dayAtMonthStart[i];

		// now also create week start ticks
		for (unsigned int i=x1; i<=x2; ++i) {
			// if day is sunday, add a line
			QDate d(2001,1,1);
			d = d.addDays(i);
			if (d.dayOfWeek() == Qt::Monday && ticks[QwtScaleDiv::MinorTick].count() < 52) {
				ticks[QwtScaleDiv::MinorTick] += i;
			}
		}

	}
	// if we span several weeks
	// MajorTicks are weeks, MinorTicks are days
	else if (x2 - x1 > 14) {
		// now also create week start ticks
		for (unsigned int i=x1; i<=x2; ++i) {
			// if day is sunday, add a line
			QDate d(2001,1,1);
			d = d.addDays(i);
			// minor ticks are day ticks
			if (d.dayOfWeek() == Qt::Monday && ticks[QwtScaleDiv::MajorTick].count() < 52) {
				ticks[QwtScaleDiv::MajorTick] += i;
			}
			// minor ticks are day ticks
			ticks[QwtScaleDiv::MinorTick] += i;
		}
	}
	else {
		// MajorTicks are days, MinorTicks are hours
		for (unsigned int i=x1*24; i<=x2*24; ++i) {
			if (i % 24 == 0)
				ticks[QwtScaleDiv::MajorTick] += i/24.0;
			ticks[QwtScaleDiv::MinorTick] += i/24.0;
		}
	}

	scaleDiv = QwtScaleDiv(interval, ticks);
	return scaleDiv;
}
