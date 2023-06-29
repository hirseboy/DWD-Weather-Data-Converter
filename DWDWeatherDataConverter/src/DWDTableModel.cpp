#include "DWDTableModel.h"

#include <QColor>


QDate convertIBKTimeToQDate(const IBK::Time &time) {

	int y;
	unsigned int m, d;
	double s;

	time.decomposeDate(y, m, d, s);

	return QDate(y, m+1, d+1);

};

DWDTableModel::DWDTableModel(QObject * parent):
	QAbstractTableModel(parent)
{

}

int DWDTableModel::rowCount(const QModelIndex & parent) const {
	return m_descData->size();
}

int DWDTableModel::columnCount(const QModelIndex & parent) const {
	return 13;
}

QVariant DWDTableModel::data(const QModelIndex & index, int role) const {

	if (!index.isValid()) // we do not want to take an invalid index
		return QVariant();

	DWDDescriptonData & dwdData = (*m_descData)[index.row()];

	DWDDescriptonData::DWDDataType dataType = DWDDescriptonData::NUM_D;

	switch (index.column() ) {
		case ColWind:
			dataType = DWDDescriptonData::D_Wind;
		break;
		case ColAirTemp:
			dataType = DWDDescriptonData::D_TemperatureAndHumidity;
		break;
		case ColPressure:
			dataType = DWDDescriptonData::D_Pressure;
		break;
		case ColRadiation:
			dataType = DWDDescriptonData::D_Solar;
		break;
		case ColPrecipitation:
			dataType = DWDDescriptonData::D_Precipitation;
		break;
	}

	switch (role) {
		case Qt::DisplayRole :
			switch (index.column()) {
				case ColId :
					return dwdData.m_idStation;
				case ColDistance :
					return (double)(int)(100*dwdData.m_distance)/100;
				case ColCountry :
					return QString::fromLatin1(dwdData.m_country.c_str());
				case ColLatitude :
					return (double)(int)(100*dwdData.m_latitude)/100;
				case ColLongitude :
					return (double)(int)(100*dwdData.m_longitude)/100;
				case ColName :
					return QString::fromLatin1(dwdData.m_name.c_str());
				case ColMinDate :
					return convertIBKTimeToQDate(dwdData.m_minDate);
				case ColMaxDate :
					return convertIBKTimeToQDate(dwdData.m_maxDate);
		}
		break;

		case Qt::CheckStateRole : // we also want to apply all the checkboxes
			switch (index.column()) {
				case ColWind :
				case ColAirTemp :
				case ColRadiation :
				case ColPressure :
				case ColPrecipitation :
					Q_ASSERT(dataType != DWDDescriptonData::DWDDataType::NUM_D);
					if (!dwdData.m_data[dataType].m_isAvailable)
						return QVariant();
					else
						return dwdData.m_data[dataType].m_isChecked ? Qt::Checked : Qt::Unchecked;
			}
		case Qt::FontRole :
			// vars with INVALID valueRef -> grey italic
			//      with valid value -> black, bold
		break;

		case Qt::BackgroundColorRole :
			switch(index.column()) {
				case ColWind:
					if (!dwdData.m_data[dataType].m_isAvailable)
						return QVariant();
					else
						return QVariant(QColor(Qt::blue));
				case ColAirTemp:
					if (!dwdData.m_data[dataType].m_isAvailable)
						return QVariant();
					else
						return QVariant(QColor(Qt::red));
				case ColRadiation:
					if (!dwdData.m_data[dataType].m_isAvailable)
						return QVariant();
					else
						return QVariant(QColor(Qt::yellow));
			}
			// vars with INVALID valueRef -> grey italic
		break;

		// UserRole returns value reference
		case Qt::UserRole :
			return dataType;
		break;

		case Qt::TextAlignmentRole :
			switch (index.column()) {
				case ColWind :
				case ColAirTemp :
				case ColRadiation :
				case ColPressure :
				case ColPrecipitation :
				case ColId:
				case ColLatitude:
				case ColLongitude:
				case ColDistance:
				case ColMinDate:
				case ColMaxDate:
					return Qt::AlignCenter;
				case ColCountry:
				case ColName:
					return Qt::AlignVCenter;
			}
		break;
	}
	return QVariant();

}


QVariant DWDTableModel::headerData(int section, Qt::Orientation orientation, int role) const {

		if(section == -1)
			return QVariant();

		static QStringList headers = QStringList()

			<< tr("Station Id")
			<< tr("Distance\n[km]")
			<< tr("Longitude")
			<< tr("Latitude")
			<< tr("Name")
			<< tr("Country")
			<< tr("Minimum\nDate")
			<< tr("Maximum\nDate")
			<< tr("Temperature /\nHumidity")
			<< tr("Short-wave\nradiation")
			<< tr("Wind Speed")
			<< tr("Pressure")
			<< tr("Precipitation");

		if (orientation == Qt::Vertical)
			return QVariant();
		switch (role) {
			case Qt::DisplayRole :
				return headers[section];
		}
		return QVariant();
}

bool DWDTableModel::setData(const QModelIndex & index, const QVariant & value, int role) {

	if (!index.isValid())
		return false;

	int col = index.column();
	DWDDescriptonData::DWDDataType dataType;

	switch (col) {
		case ColWind:
			dataType = DWDDescriptonData::D_Wind;
		break;
		case ColAirTemp:
			dataType = DWDDescriptonData::D_TemperatureAndHumidity;
		break;
		case ColPressure:
			dataType = DWDDescriptonData::D_Pressure;
		break;
		case ColRadiation:
			dataType = DWDDescriptonData::D_Solar;
		break;
		case ColPrecipitation:
			dataType = DWDDescriptonData::D_Precipitation;
		break;
	}

	DWDDescriptonData &checkBox = (*m_descData)[index.row()];

	QModelIndex uncheckedIndex;

	if (role == Qt::CheckStateRole)	{
		if ((Qt::CheckState)value.toInt() == Qt::Checked) {
			//	user has checked item
			//  we have to uncheck all the data in the column if we check a different value
			for ( unsigned int i = 0; i < m_descData->size(); ++i ) {

				DWDDescriptonData &dwdData = (*m_descData)[i];
				if (dwdData.m_data[dataType].m_isChecked) { // we find where we need to update our view
					dwdData.m_data[dataType].m_isChecked = false; // we uncheck the checkbox
					Q_ASSERT(m_proxyModel!=nullptr);
					QModelIndex srcIndex = QAbstractTableModel::index(i,col);
					emit dataChanged(srcIndex, srcIndex); // we just update the whole column
					break;
				}
			}

			checkBox.m_data[dataType].m_isChecked = true;
		}
		else {
			checkBox.m_data[dataType].m_isChecked = false; //user has unchecked item

		}
		emit dataChanged(index, index);

		return true;
	}
	return false;

}

Qt::ItemFlags DWDTableModel::flags(const QModelIndex & index) const {

	if (!index.isValid()) // assert that index is valid
		return Qt::ItemFlags();

	if(index.column() < 8)
		return QAbstractItemModel::flags(index);

	DWDDescriptonData &checkBox = (*m_descData)[index.row()];
	DWDDescriptonData::DWDDataType dataType;

	switch (index.column()) {
		case ColWind:
			dataType = DWDDescriptonData::D_Wind;
		break;
		case ColAirTemp:
			dataType = DWDDescriptonData::D_TemperatureAndHumidity;
		break;
		case ColPressure:
			dataType = DWDDescriptonData::D_Pressure;
		break;
		case ColRadiation:
			dataType = DWDDescriptonData::D_Solar;
		break;
		case ColPrecipitation:
			dataType = DWDDescriptonData::D_Precipitation;
		break;
	}

	return checkBox.m_data[dataType].m_isAvailable ? QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable : QAbstractItemModel::flags(index) ;


}

void DWDTableModel::reset() {
	beginResetModel();
	endResetModel();
}
