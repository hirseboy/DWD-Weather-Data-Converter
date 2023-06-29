#include "DWDData.h"

#include <QTimer>
#include <QDebug>
#include <QLabel>
#include <QApplication>
#include <QMessageBox>

#include <IBK_StringUtils.h>
#include <IBK_FileReader.h>
#include <IBK_physics.h>
#include <IBK_NotificationHandler.h>

#include <qftp.h>

#include <CCM_SolarRadiationModel.h>
#include <CCM_ClimateDataLoader.h>

#include "DWDConstants.h"
#include "DWDDescriptonData.h"


void DWDData::createData(IBK::NotificationHandler * notify, const std::map<IBK::Path,
						 std::set<DWDData::DataType>> &filenames, unsigned int intervalDuration) {

	FUNCID(createData);

	try {
	m_intervalDuration = intervalDuration;

	m_data.clear();

	unsigned int counter = 0;
	unsigned int size = filenames.size();
	for(std::map<IBK::Path, std::set<DWDData::DataType>>::const_iterator	it=filenames.begin();
		it!=filenames.end(); ++it){
		//check if file exist
		IBK::Path fileName = it->first;
		if(!fileName.exists())
			throw IBK::Exception(IBK::FormatString( "File '%1' does not exist. ").arg(fileName.filename()), FUNC_ID);

		//read the file
		IBK::FileReader fileReader(fileName, 40960);
		std::vector<std::string> lines;

		qDebug() << "Reading file " << QString::fromStdString(fileName.str());
		m_progressDlg->setLabelText(QString("Reading file '%1'").arg(QString::fromStdString(fileName.filename().str() ) ) );
		fileReader.readAll(fileName, lines, std::vector<std::string>{"\n"}, 0, notify);

		qDebug() << "Extracting data from " << QString::fromStdString(fileName.str());
		m_progressDlg->setLabelText(QString("Converting data of file '%1'").arg(QString::fromStdString(fileName.filename().str() ) ) );
		for(unsigned int i=1;i<lines.size(); ++i){
			addDataLine(lines[i], it->second);
			notify->notify((double)counter/size * (double)(i)/lines.size());
		}

		++counter;
	}
	} catch (IBK::Exception &ex) {
		throw IBK::Exception("Could not read DWD Data", FUNC_ID);
	}
}



void DWDData::addDataLine(std::string &line, const std::set<DataType> &dataType){
	std::vector<std::string> data;

	/*! Mind the time zone. In DWD Data the time zone is set to UTC+0 whereas in our exported epw file
		the time zone is set to UTC+1 thus we have to shift the time stamp for one hour (+1)
		-> DWD: 01012020 00:00 --> EPW: 01012020 01:00
	*/

	//explode line
	IBK::explode(line, data, ";", true);

	if(data.empty())
		return;

	std::set<DataType>::const_iterator it = dataType.begin();
	IBK::Time time, timeNextYear;

	//check startdate
	if(!m_startTime.isValid()){
		return;	//time is not valid
	}

	//check Enddate
	if(!m_endTime.isValid()){
		return;	//time is not valid
	}

	if(data.size()>2){
		try {
			unsigned int idx = 1;
			//station code
			if (dataType.find(DT_RadiationGlobal) != dataType.end() || dataType.find(DT_RadiationDiffuse) != dataType.end()
					|| dataType.find(DT_RadiationLongWave) != dataType.end() || dataType.find(DT_ZenithAngle) != dataType.end() )
				idx = 8;

			unsigned int stationId = IBK::string2val<unsigned int>(data[0]);		//not used yet

			//get timepoint
			std::string& s = data[idx];
			unsigned int year	= IBK::string2val<unsigned int>(s.substr(0,4));
			unsigned int month	= IBK::string2val<unsigned int>(s.substr(4,2))-1;
			unsigned int day	= IBK::string2val<unsigned int>(s.substr(6,2))-1;
			unsigned int hour	= IBK::string2val<unsigned int>(s.substr(8,2));
			time.set(year, month, day, hour*3600);
		} catch (IBK::Exception &ex) {

		}

	}
	else
		return;	//if no time point is given return

	double timepointStart = m_startTime.secondsUntil(time)/m_intervalDuration;
	double timepointEnd = m_endTime.secondsUntil(time)/m_intervalDuration;

	//shift all data because startpoint is later
	unsigned int newTimepointStart = static_cast<unsigned int>(timepointStart);
	unsigned int newTimepointEnd = static_cast<unsigned int>(timepointEnd);
	if(timepointStart<0 || timepointEnd > 0){
		return; // data should not be added
		//adjust start date
		double timeDiffSec = m_startTime.secondsUntil(time);
		int yearDiff = (int)(std::abs(timeDiffSec) / (8760*60*60)+1);
		m_startTime = IBK::Time(m_startTime.year()-yearDiff,0);
	}

	//start point is earlier or perfect
	if(m_data.size()<=newTimepointStart){
		unsigned int aaa =newTimepointStart-m_data.size()+1;
		m_data.insert(m_data.end(), aaa, IntervalData());
	}


	//add data to interval data
	while(it!= dataType.end()) {

		unsigned int col = getColumnDWD(*it);

		double unitFactor = 1;
		if(*it == DT_RadiationGlobal || *it == DT_RadiationDiffuse || *it == DT_RadiationLongWave)
			unitFactor = 1/0.36;

		if(col < data.size()){
			m_data[newTimepointStart].setVal(*it, IBK::string2val<double>(data[col])*unitFactor);
		}
		++it;
	}

	//get timepoint
}

void DWDData::writeTSV(unsigned int year){
	//first find the start timepoint
	if(m_data.empty())
		return; // no data :(

	// we go through all data and compose a tsv table
	for(unsigned int i=0; i<m_data.size(); ++i){
		// time
		// data
	}
}

void DWDData::exportEPW(CCM::ClimateDataLoader &loader, double latitudeDeg, double longitudeDeg, IBK::Path &exportPath) {
	FUNCID(exportEPW);

	/*! Mind the time zone. In DWD Data the time zone is set to UTC+0 whereas in our exported epw file
		the time zone is set to UTC+1 thus we have to shift the time stamp for one hour (+1)
		-> DWD: 01012020 00:00 --> EPW: 01012020 01:00
	*/

	//all data is invalid initialized
	loader.initDataWithDefault();
	loader.m_latitudeInDegree = latitudeDeg;
	loader.m_longitudeInDegree = longitudeDeg;

	CCM::SolarRadiationModel solMod;
	solMod.m_climateDataLoader = loader;

	// remove existing data
	//	for (int i=0; i<CCM::ClimateDataLoader::NumClimateComponents; ++i) {
	//		loader.m_data[i] = std::vector<double>(8760, -99);
	//	}

	int idx = m_startTime.secondsUntil(IBK::Time(m_startTime.year(), 0))/m_intervalDuration;
	int hourCount = m_startTime.secondsUntil(m_endTime)/m_intervalDuration;

	Q_ASSERT(hourCount < 8761);

	for (int i=0; i<hourCount;++i, ++idx) {
		//		if(idx > m_data.size() || m_data.empty())
		//			break;
		//		if(idx < 0)
		//			continue;
		///TODO Fehlerbetrachtung muss dann woanders gemacht werden
		IntervalData intVal = m_data[i];
		loader.m_data[CCM::ClimateDataLoader::Temperature][i] = (intVal.m_airTemp == -999 ? 0 : intVal.m_airTemp);
		loader.m_data[CCM::ClimateDataLoader::RelativeHumidity][i] = intVal.m_relHum < 0 ? 0 : intVal.m_relHum;

		loader.m_data[CCM::ClimateDataLoader::WindVelocity][i] = intVal.m_windSpeed;
		loader.m_data[CCM::ClimateDataLoader::WindDirection][i] = intVal.m_windDirection;

		loader.m_data[CCM::ClimateDataLoader::AirPressure][i] = intVal.m_pressure;

		double dirH = intVal.m_globalRad - intVal.m_diffRad;
		double dirN;
		solMod.convertHorizontalToNormalRadiation(m_intervalDuration*(idx-0.5), dirH, dirN);

		loader.m_data[CCM::ClimateDataLoader::DirectRadiationNormal][i] = dirN;
		loader.m_data[CCM::ClimateDataLoader::DiffuseRadiationHorizontal][i] = intVal.m_diffRad;
		loader.m_data[CCM::ClimateDataLoader::LongWaveCounterRadiation][i] = intVal.m_counterRad;
		loader.m_data[CCM::ClimateDataLoader::Rain][i] = intVal.m_precipitation;

#if 0
		loader.m_data[CCM::ClimateDataLoader::WindVelocity][i] = std::max<double>( 90.0 - intVal.m_zenithAngle, 0);
		loader.m_data[CCM::ClimateDataLoader::LongWaveCounterRadiation][i] = dirH/ ( std::cos(intVal.m_zenithAngle*IBK::DEG2RAD)< 1e-04 ? 1 : std::cos(intVal.m_zenithAngle*IBK::DEG2RAD) );
		loader.m_data[CCM::ClimateDataLoader::WindDirection][i] = std::max<double>(solMod.m_sunPositionModel.m_elevation/IBK::DEG2RAD, 0);
#endif
	}

	try {
		loader.writeClimateDataEPW(exportPath);
	} catch (IBK::Exception &ex) {
		throw IBK::Exception("Could not write climate file (epw)", FUNC_ID);
	}
}

void DWDData::exportC6B(CCM::ClimateDataLoader &loader, double latitudeDeg, double longitudeDeg, IBK::Path &exportPath) {
	FUNCID(exportEPW);

	/*! Mind the time zone. In DWD Data the time zone is set to UTC+0 whereas in our exported epw file
		the time zone is set to UTC+1 thus we have to shift the time stamp for one hour (+1)
		-> DWD: 01012020 00:00 --> EPW: 01012020 01:00
	*/

	//all data is invalid initialized
	loader.initDataWithDefault();
	loader.m_latitudeInDegree = latitudeDeg;
	loader.m_longitudeInDegree = longitudeDeg;

	CCM::SolarRadiationModel solMod;
	solMod.m_climateDataLoader = loader;

	// remove existing data
	//	for (int i=0; i<CCM::ClimateDataLoader::NumClimateComponents; ++i) {
	//		loader.m_data[i] = std::vector<double>(8760, -99);
	//	}

	unsigned int idx = m_startTime.seconds();
	unsigned int hourCount = m_startTime.secondsUntil(m_endTime)/m_intervalDuration;

	std::vector<double> dataTimePoints(hourCount);
	// Init all timesteps of climateloader
	for (unsigned int i=0; i<hourCount; ++i) {
		dataTimePoints[i] = idx + 3600.0 * i; // current time in seconds
	}
	loader.m_dataTimePoints = dataTimePoints;
	loader.initDataWithDefault();

	for (unsigned int i=0; i<hourCount;++i, ++idx) {
		/// TODO Fehlerbetrachtung muss dann woanders gemacht werden
		IntervalData intVal = m_data[i];
		loader.m_data[CCM::ClimateDataLoader::Temperature][i] = (intVal.m_airTemp == -999 ? 0 : intVal.m_airTemp);
		loader.m_data[CCM::ClimateDataLoader::RelativeHumidity][i] = intVal.m_relHum < 0 ? 0 : intVal.m_relHum;

		loader.m_data[CCM::ClimateDataLoader::WindVelocity][i] = intVal.m_windSpeed;
		loader.m_data[CCM::ClimateDataLoader::WindDirection][i] = intVal.m_windDirection;

		loader.m_data[CCM::ClimateDataLoader::AirPressure][i] = intVal.m_pressure;

		double dirH = intVal.m_globalRad - intVal.m_diffRad;
		double dirN;
		solMod.convertHorizontalToNormalRadiation(m_intervalDuration*(idx-0.5), dirH, dirN);

		loader.m_data[CCM::ClimateDataLoader::DirectRadiationNormal][i] = dirN;
		loader.m_data[CCM::ClimateDataLoader::DiffuseRadiationHorizontal][i] = intVal.m_diffRad;
		loader.m_data[CCM::ClimateDataLoader::LongWaveCounterRadiation][i] = intVal.m_counterRad;
		loader.m_data[CCM::ClimateDataLoader::Rain][i] = intVal.m_precipitation;

#if 0
		loader.m_data[CCM::ClimateDataLoader::WindVelocity][i] = std::max<double>( 90.0 - intVal.m_zenithAngle, 0);
		loader.m_data[CCM::ClimateDataLoader::LongWaveCounterRadiation][i] = dirH/ ( std::cos(intVal.m_zenithAngle*IBK::DEG2RAD)< 1e-04 ? 1 : std::cos(intVal.m_zenithAngle*IBK::DEG2RAD) );
		loader.m_data[CCM::ClimateDataLoader::WindDirection][i] = std::max<double>(solMod.m_sunPositionModel.m_elevation/IBK::DEG2RAD, 0);
#endif
	}

	try {
		loader.writeClimateDataIBK(exportPath);
	} catch (IBK::Exception &ex) {
		throw IBK::Exception("Could not write climate file (c6b)", FUNC_ID);
	}
}



QString DWDData::urlFilename(const DWDData::DataType &type, const QString &numberString, const std::string &dateString,
							 bool isRecent,const QString &filename) const {
	QString rec = "_akt", rec2 = "";
	QString dateStringNew = "";
	if(!isRecent){
		rec = "_hist";
		dateStringNew = QString::fromStdString(dateString);
	}

	QString base = "ftp://opendata.dwd.de/climate_environment/CDC/observations_germany/climate/hourly/";

	if(!(type == DT_RadiationDiffuse || type == DT_RadiationGlobal || type == DT_RadiationLongWave))
		rec2 = isRecent ? "recent/" : "historical/";

	if( isRecent || (type == DT_RadiationDiffuse || type == DT_RadiationGlobal || type == DT_RadiationLongWave || type == DT_Precipitation)) {

		switch (type) {
		case DT_AirTemperature:
		case DT_RelativeHumidity:
			return base + "air_temperature/" + rec2 + "stundenwerte_TU_" + numberString + dateStringNew + rec + ".zip";
			// recent --> stundenwerte_TU_00142_akt.zip
			// historical --> stundenwerte_TU_00078_20041101_20201231_hist.zip
			// neu hinzu "_20041101_20201231" >> dateString

		case DT_Pressure:
			return base + "pressure/" + rec2 + "stundenwerte_P0_" + numberString + dateStringNew + rec + ".zip";

		case DT_Precipitation:
			return base + "precipitation/" + rec2 +  filename;

		case DT_WindSpeed:
		case DT_WindDirection:
			return base + "wind/" + rec2 + "stundenwerte_FF_" + numberString + dateStringNew + rec + ".zip";

		case DT_RadiationDiffuse:
		case DT_RadiationGlobal:
		case DT_RadiationLongWave:
		case DT_ZenithAngle:
			return base + "solar/" + "stundenwerte_ST_" + numberString + "_row.zip";
		}

	} else {

		QString baseSearch;

		switch (type) {
		case DT_AirTemperature:
		case DT_RelativeHumidity:
			baseSearch = base + "air_temperature/";
			// recent --> stundenwerte_TU_00142_akt.zip
			// historical --> stundenwerte_TU_00078_20041101_20201231_hist.zip
			// neu hinzu "_20041101_20201231" >> dateString

			break;
		case DT_Pressure:
			baseSearch = base + "pressure/";

			break;
		case DT_WindSpeed:
		case DT_WindDirection:
			baseSearch = base + "wind/";
			break;
			//			case DT_RadiationDiffuse:
			//			case DT_RadiationGlobal:
			//			case DT_RadiationLongWave:
			//			case DT_ZenithAngle:
			//				baseSearch = base + "solar/";
		}
		return baseSearch + rec2 + filename;

	}
}

QString DWDData::filename(const DWDData::DataType &type, const QString &numberString, const std::string &dateString, bool isRecent) const{
	QString rec = "_akt";
	QString dateStringNew = "";
	if(!isRecent && !(type == DT_RadiationDiffuse || type == DT_RadiationGlobal || type == DT_RadiationLongWave)){
		rec = "_hist";
		dateStringNew = QString::fromStdString(dateString);
	}

	switch (type) {
	case DT_AirTemperature:
	case DT_RelativeHumidity:
		return "stundenwerte_TU_" + numberString + dateStringNew + rec;

	case DT_Pressure:
		return "stundenwerte_P0_" + numberString + dateStringNew + rec;

	case DT_WindSpeed:
	case DT_WindDirection:
		return "stundenwerte_FF_" + numberString + dateStringNew + rec;

	case DT_Precipitation:
		return "stundenwerte_RR_" + numberString + dateStringNew + rec;

	case DT_RadiationDiffuse:
	case DT_RadiationGlobal:
	case DT_RadiationLongWave:
	case DT_ZenithAngle:
		return "stundenwerte_ST_" + numberString + "_row";
	}
}

void DWDData::findFile(const QUrlInfo &url) {
	//qDebug() << url.name();
	m_urls.push_back(url);
}

unsigned int DWDData::getColumnDWD(const DataType &dt){
	switch (dt) {
	case DT_AirTemperature:					return 3;
	case DT_RelativeHumidity:				return 4;
	case DT_RadiationLongWave:				return 3;
	case DT_RadiationDiffuse:				return 4;
	case DT_RadiationGlobal:				return 5;
	case DT_ZenithAngle:					return 7;
	case DT_WindSpeed:						return 3;
	case DT_WindDirection:					return 4;
	case DT_Pressure:						return 4;
	case DT_Precipitation:					return 3;
	}
	return 0;
}
