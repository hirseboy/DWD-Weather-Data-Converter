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

#ifndef DWDTimePlotPickerH
#define DWDTimePlotPickerH

#include "qwt_plot_picker.h"
#include "qwt_text.h"

#include <QString>

class QwtPlotCanvas;

class DWDTimePlotPicker : public QwtPlotPicker {
	Q_OBJECT
public:
	DWDTimePlotPicker (QWidget * canvas, const QString &yUnit);

	void setYUnit(const QString& unit);
	const QString & yUnit() const;

protected:
	virtual QwtText trackerTextF(const QPointF &) const override;

	virtual void begin();
	virtual bool end(bool ok = true);

	QString	d_yUnit;
};

#endif // DWDTimePlotPickerH
