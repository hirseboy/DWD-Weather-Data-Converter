#ifndef DWDCONVERSIONS_H
#define DWDCONVERSIONS_H

#include <QDate>

#include <IBK_Time.h>

class DWDConversions
{
public:
	DWDConversions();

	static QDate convertIBKTime2QDate(const IBK::Time &t) {
		int y;
		unsigned int m, d;
		double s;

		// Decompose IBK::Time
		t.decomposeDate(y, m, d, s);

		return QDate(y, m+1, d+1);
	}


	static IBK::Time convertQDate2IBKTime (const QDate &t) {
		return IBK::Time(t.year(), t.month()-1, t.day()-1, 0);
	}
};

#endif // DWDCONVERSIONS_H
