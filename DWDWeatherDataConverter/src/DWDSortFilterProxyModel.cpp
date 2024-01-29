#include "DWDSortFilterProxyModel.h"
#include "DWDTableModel.h"


DWDSortFilterProxyModel::DWDSortFilterProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{
}

void DWDSortFilterProxyModel::setFilterMinimumDate(QDate date) {
	m_minDate = date;
	invalidateFilter();
}

void DWDSortFilterProxyModel::setFilterMaximumDate(QDate date) {
	m_maxDate = date;
	invalidateFilter();
}

void DWDSortFilterProxyModel::setFilterMaximumDistance(double maxDistance){
	m_maxdistance = maxDistance;
}

bool DWDSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const{
	QModelIndex maxDistance = sourceModel()->index(sourceRow, DWDTableModel::ColDistance, sourceParent);
	QModelIndex minDate = sourceModel()->index(sourceRow, DWDTableModel::ColMinDate, sourceParent);
	QModelIndex maxDate = sourceModel()->index(sourceRow, DWDTableModel::ColMaxDate, sourceParent);
	QModelIndex name = sourceModel()->index(sourceRow, DWDTableModel::ColName, sourceParent);

	QDate minQDate = QDate::fromString(sourceModel()->data(minDate).toString(), "dd.MM.yyyy");
	QDate maxQDate = QDate::fromString(sourceModel()->data(maxDate).toString(), "dd.MM.yyyy");

	return (m_maxdistance > sourceModel()->data(maxDistance).toDouble()) &&
			dateInRange(minQDate, maxQDate) &&
			sourceModel()->data(name).toString().contains(filterRegExp());
}

bool DWDSortFilterProxyModel::dateInRange(QDate minDate, QDate maxDate) const {
	return (!m_minDate.isValid() || minDate < m_minDate) && (!m_maxDate.isValid() || maxDate > m_maxDate);
}
