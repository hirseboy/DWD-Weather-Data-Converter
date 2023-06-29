#include "DWDSortFilterProxyModel.h"


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
	QModelIndex maxDistance = sourceModel()->index(sourceRow, 1, sourceParent);
	QModelIndex minDate = sourceModel()->index(sourceRow, 6, sourceParent);
	QModelIndex maxDate = sourceModel()->index(sourceRow, 7, sourceParent);
	QModelIndex name = sourceModel()->index(sourceRow, 4, sourceParent);

	return (m_maxdistance > sourceModel()->data(maxDistance).toDouble()) &&
			dateInRange(sourceModel()->data(minDate).toDate(), sourceModel()->data(maxDate).toDate() ) &&
			sourceModel()->data(name).toString().contains(filterRegExp());
}

bool DWDSortFilterProxyModel::dateInRange(QDate minDate, QDate maxDate) const {

	return (!m_minDate.isValid() || minDate < m_minDate) && (!m_maxDate.isValid() || maxDate > m_maxDate);
}
