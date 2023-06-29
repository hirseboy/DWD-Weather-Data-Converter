#ifndef DWDSortFilterProxyModelH
#define DWDSortFilterProxyModelH

#include <QObject>
#include <QSortFilterProxyModel>
#include <QDate>

class DWDSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	DWDSortFilterProxyModel(QObject *parent = nullptr);

	QDate filterMinimumDate() const { return m_minDate; }
	void setFilterMinimumDate(QDate date);

	QDate filterMaximumDate() const { return m_minDate; }
	void setFilterMaximumDate(QDate date);

	void setFilterMaximumDistance(double maxDistance);

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
//	bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

private:
	bool dateInRange(QDate minDate, QDate maxDate) const;
	bool distanceInRange(double distance) const;

	QDate				m_minDate;
	QDate				m_maxDate;
	double				m_maxdistance;

};

#endif // DWDSortFilterProxyModelH
