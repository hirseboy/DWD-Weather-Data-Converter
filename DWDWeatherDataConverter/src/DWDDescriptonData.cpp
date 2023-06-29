#include "DWDDescriptonData.h"

#include <IBK_FileReader.h>

#include <QMessageBox>

#include "QtExt_Directories.h"

QStringList DWDDescriptonData::downloadDescriptionFiles(bool isRecent){
	QString baseDirFTP = "ftp://opendata.dwd.de/climate_environment/CDC/observations_germany/climate/hourly/";

	QString type = "recent/";

	if(!isRecent)
		type = "historical/";

	QStringList sl;
	sl << baseDirFTP + "air_temperature/" + type + "TU_Stundenwerte_Beschreibung_Stationen.txt";
	sl << baseDirFTP + "solar/ST_Stundenwerte_Beschreibung_Stationen.txt";					//no difference between historical or recent
	sl << baseDirFTP + "wind/" + type +"FF_Stundenwerte_Beschreibung_Stationen.txt";
	sl << baseDirFTP + "pressure/" + type + "P0_Stundenwerte_Beschreibung_Stationen.txt";
	sl << baseDirFTP + "precipitation/" + type + "RR_Stundenwerte_Beschreibung_Stationen.txt";

	return sl;
}

void DWDDescriptonData::readAllDescriptions(std::vector<DWDDescriptonData> &dwdDescriptonData){

	IBK::Path filepath(QtExt::Directories().userDataDir().toStdString() + "/TU_Stundenwerte_Beschreibung_Stationen.txt");
	readDescription(filepath, dwdDescriptonData, D_TemperatureAndHumidity);

	filepath= IBK::Path (QtExt::Directories().userDataDir().toStdString() + "/P0_Stundenwerte_Beschreibung_Stationen.txt");
	readDescription(filepath, dwdDescriptonData, D_Pressure);

	filepath= IBK::Path (QtExt::Directories().userDataDir().toStdString() + "/ST_Stundenwerte_Beschreibung_Stationen.txt");
	readDescription(filepath, dwdDescriptonData, D_Solar);

	filepath= IBK::Path (QtExt::Directories().userDataDir().toStdString() + "/FF_Stundenwerte_Beschreibung_Stationen.txt");
	readDescription(filepath, dwdDescriptonData, D_Wind);

	filepath= IBK::Path (QtExt::Directories().userDataDir().toStdString() + "/RR_Stundenwerte_Beschreibung_Stationen.txt");
	readDescription(filepath, dwdDescriptonData, D_Precipitation);

	for ( DWDDescriptonData &dwdData : dwdDescriptonData )
		dwdData.calculateMinMaxDate(); // we calculate the minimum period
}

void DWDDescriptonData::calculateMinMaxDate() {

	m_minDate = m_startDate[0];
	m_maxDate = m_endDate[0];

	for ( unsigned int i = 1; i<4; ++i ) {

		if ( !m_data[i].m_isAvailable )
			continue;

		double secUntilStart = m_minDate.secondsUntil(m_startDate[i]);

		double secUntilEnd = m_maxDate.secondsUntil(m_endDate[i]);

		if ( secUntilStart>0 || !m_minDate.isValid() )
			m_minDate = m_startDate[i];

		if ( secUntilEnd<0 || !m_maxDate.isValid() )
			m_maxDate = m_endDate[i];
	}
}

QColor DWDDescriptonData::color(const DWDDataType & dt) {

	/*! Map with colors of data. */
	std::map<DWDDataType, QColor>	colorData;

	colorData[DWDDataType::D_Solar]				= QColor("#E9B44C");
	colorData[DWDDataType::D_Precipitation]		= QColor("#008000");
	colorData[DWDDataType::D_Pressure]			= QColor("#D45500");
	colorData[DWDDataType::D_TemperatureAndHumidity] = QColor("#AA0000");
	colorData[DWDDataType::D_Wind]				= QColor("#004E98");

	return colorData[dt];
}

void DWDDescriptonData::readDescription(const IBK::Path &filepath, std::vector<DWDDescriptonData> &dwdDescriptonData, const DWDDataType &dataType){
//	IBK::Path filepath(QtExt::Directories::userDataDir().toStdString() + "filename.txt");

	IBK::FileReader fileReader(filepath);

	//check if file is valid
	if(!fileReader.valid()){
		QMessageBox::warning(nullptr, QString(), QString("File '%1' is not valid").arg(QString::fromStdString(filepath.absolutePath().c_str())));
		return;
	}
	std::vector<std::string> lines;
	//fill lines vector
	fileReader.readAll(filepath, lines,std::vector<std::string>{"\n"});


	// --------------------------------------------------
	// we want to find the right line

//	std::ifstream f("FILE.txt");
//	std::string s;

//	for (int i = 1; i <= LINE; i++)
//			std::getline(f, s);

	// --------------------------------------------------

	//parse each line
	for (unsigned int i=2; i<lines.size(); ++i ) {
		const std::string & line =lines[i];
		if(line.empty())
			continue;

		unsigned int id = IBK::string2val<unsigned int>(line.substr(0,5));

		unsigned int index = 0;
		bool foundIndex = false;
		// we have to go through our vector if we already have specified our station ID
		for ( unsigned int i=0; i<dwdDescriptonData.size(); ++i ) {
			if ( dwdDescriptonData[i].m_idStation == id ) {
				index = i;
				foundIndex = true;
				break;
			}
		}

		if (!foundIndex) {
			dwdDescriptonData.push_back(DWDDescriptonData());
			index = dwdDescriptonData.size() - 1; // mind that size is always latest index + 1
		}

		DWDDescriptonData &dwd = dwdDescriptonData[index];

		try {
			//extract all informations
			dwd.m_idStation = id;
			dwd.m_data[dataType] = DWDDataManager(true, false, false);

			// Example for an entry:
			// =======================================================================================================================
			// 00071 20091201 20191231            759     48.2156    8.9784 Albstadt-Badkap                          Baden-Württemberg
			// =======================================================================================================================


			dwd.m_startDate[dataType].set( IBK::string2val<unsigned int>(line.substr(6,4)),
										   IBK::string2val<unsigned int>(line.substr(10,2))-1,
										   IBK::string2val<unsigned int>(line.substr(12,2))-1,0);
			dwd.m_endDate[dataType].set( IBK::string2val<unsigned int>(line.substr(15,4)),
										 IBK::string2val<unsigned int>(line.substr(19,2))-1,
										 IBK::string2val<unsigned int>(line.substr(21,2))-1,0);

			dwd.m_startDateString = line.substr(6,8);
			dwd.m_endDateString = line.substr(15,8);
			dwd.m_height = IBK::string2val<double>(line.substr(24,38-24));
			dwd.m_latitude = IBK::string2val<double>(line.substr(39,50-39));
			dwd.m_longitude = IBK::string2val<double>(line.substr(51,60-51));
			dwd.m_name = IBK::trim_copy(line.substr(61,100-61));
			dwd.m_country = IBK::trim_copy(line.substr(101,500));


		}  catch (IBK::Exception &ex) {
			QMessageBox::warning(nullptr, QString(), QString("Got an exception while reading lines. In line %1\n").arg(i)
								 + QString("%1\n").arg(dwd.m_idStation)
								 + QString("%1\n").arg(QString::fromStdString(dwd.m_startDate[dataType].toDateTimeFormat()))
								 + QString("%1\n").arg(QString::fromStdString(dwd.m_endDate[dataType].toDateTimeFormat()))
								 + QString("%1\n").arg(dwd.m_height)
								 + QString("%1\n").arg(dwd.m_latitude)
								 + QString("%1\n").arg(dwd.m_longitude)
								 + QString("%1\n").arg(QString::fromStdString(dwd.m_name))
								 + QString("%1\n").arg(QString::fromStdString(dwd.m_country))
								 + QString("%1\n").arg(QString::fromStdString(line))
								 );
		}
	}

}
