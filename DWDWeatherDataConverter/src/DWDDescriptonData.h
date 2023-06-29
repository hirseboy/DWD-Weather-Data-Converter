#ifndef DWDDataDescriptionH
#define DWDDataDescriptionH

#include <IBK_Time.h>
#include <IBK_Path.h>

#include <vector>

class QStringList;

#include <QDate>
#include <QColor>

class DWDDescriptonData
{
public:

	DWDDescriptonData()
	{
		for (unsigned int i=0; i<NUM_D; ++i)
			m_data[i] = DWDDataManager(false, false, false);

	};

	/*! Download 4 decription files from dwd ftp server. */
	QStringList downloadDescriptionFiles(bool isRecent=true);

	struct DWDDataManager {

		DWDDataManager(){}


		DWDDataManager(bool isAvailable, bool isChecked, bool isLocal) :
			m_isAvailable(isAvailable),
			m_isChecked(isChecked),
			m_isLocal(isLocal)
		{}

		bool			m_isAvailable;
		bool			m_isChecked;
		bool			m_isLocal;
	};


	enum DWDDataType{
		D_TemperatureAndHumidity,
		D_Solar,
		D_Wind,
		D_Pressure,
		D_Precipitation,
		NUM_D
	};


	/*! Read predefined (download dwd) description files. */
	void readAllDescriptions(std::vector<DWDDescriptonData> &dwdDescriptonData);

	/*! Calculate Min and Max Date from all 4 Boundary Conditions ( AirTemp, Pressure, Solar Rad, wind) */
	void calculateMinMaxDate();

	static QColor color(const DWDDataType &dt);

	/*! Station id. */
	unsigned int				m_idStation;

	/*! Start date of data type. */
	IBK::Time					m_startDate[NUM_D];

	/*! End date of data type. */
	IBK::Time					m_endDate[NUM_D];

	/*! Min Date */
	IBK::Time					m_minDate;

	/*! Max Date */
	IBK::Time					m_maxDate;

	/*! End date string.*/
	std::string					m_endDateString;

	/*! Start date string.*/
	std::string					m_startDateString;

	/*! Distance to reference location */
	double						m_distance;

	/*! Station height in m. */
	double						m_height;

	/*! Station latitude in Deg. */
	double						m_latitude;

	/*! Station longitude in Deg. */
	double						m_longitude;

	/*! Station name. */
	std::string					m_name;

	/*! Station country. */
	std::string					m_country;

	/*! data container. */
	DWDDataManager				m_data[NUM_D];

	/*! show database entry in table */
	bool						m_isVisible;


private:
	void readDescription(const IBK::Path &filepath, std::vector<DWDDescriptonData> & dwdDescriptonData, const DWDDataType &dataType);

	/*

 Stations_id von_datum bis_datum Stationshoehe geoBreite geoLaenge Stationsname Bundesland
----------- --------- --------- ------------- --------- --------- ----------------------------------------- ----------
00003 19500401 20110331            202     50.7827    6.0941 Aachen                                   Nordrhein-Westfalen
00044 20070401 20210314             44     52.9336    8.2370 Großenkneten                             Niedersachsen

	*/
};

#endif // DWDDataDescriptionH
