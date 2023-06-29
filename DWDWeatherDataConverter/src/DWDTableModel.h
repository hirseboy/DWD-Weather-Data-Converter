#ifndef DWDTableModelH
#define DWDTableModelH

#include <QObject>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

#include <vector>

#include "DWDDescriptonData.h"

class DWDTableModel : public QAbstractTableModel {
	Q_OBJECT
public:
	DWDTableModel(QObject *parent);

	/*! Columns shown in the table view. */
	enum Columns {
		ColId,
		ColDistance,
		ColLongitude,
		ColLatitude,
		ColName,
		ColCountry,
		ColMinDate,
		ColMaxDate,
		ColAirTemp,
		ColRadiation,
		ColWind,
		ColPressure,
		ColPrecipitation,
		NumColumns
	};

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex & parent) const;
	int columnCount(const QModelIndex & parent) const;
	QVariant data(const QModelIndex & index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	bool setData(const QModelIndex & index, const QVariant & value, int role);
	Qt::ItemFlags flags(const QModelIndex & index) const;

	/*! Performs a model reset. */
	void reset();

	/*! Description input of all stations.
		Map Cointainging all necessairy data about the weather station
		key of map is station id
	*/
	std::vector<DWDDescriptonData>				*m_descData = nullptr;

	QSortFilterProxyModel						*m_proxyModel = nullptr;


};

#endif // DWDTableModelH
