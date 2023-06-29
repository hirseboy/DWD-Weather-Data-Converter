#ifndef DWDDataH
#define DWDDataH

#include <map>
#include <vector>
#include <set>

#include <QString>
#include <QProgressDialog>

#include <qurlinfo.h>

#include <IBK_Time.h>
#include <IBK_Path.h>
#include <IBK_NotificationHandler.h>

#include <CCM_ClimateDataLoader.h>

#include "QtExt_Directories.h"

class DWDProgressBar;

class DWDData : public QObject
{
	Q_OBJECT

public:

	enum DataType{
		DT_AirTemperature,
		DT_RelativeHumidity,
		DT_RadiationLongWave,
		DT_RadiationDiffuse,
		DT_RadiationGlobal,
		DT_ZenithAngle,
		DT_SunElevation,
		DT_WindSpeed,
		DT_WindDirection,
		DT_Pressure,
		DT_Precipitation,
		NUM_DT
	};

	// TODO SWITCH TO STATIC ARRAY

	struct IntervalData{

		void setVal(const DataType &dt, double val){
			switch (dt) {
				case DT_AirTemperature:						m_airTemp = val;		break;
				case DT_RelativeHumidity:					m_relHum= val;			break;
				case DT_RadiationDiffuse:					m_diffRad= val;			break;
				case DT_RadiationGlobal:					m_globalRad= val;		break;
				case DT_RadiationLongWave:					m_counterRad= val;		break;
				case DT_ZenithAngle:						m_zenithAngle= val;		break;
				case DT_WindDirection:						m_windDirection= val;	break;
				case DT_WindSpeed:							m_windSpeed= val;		break;
				case DT_Pressure:							m_pressure= val;		break;
				case DT_Precipitation:						m_precipitation= val;	break;

			}
		}

		double data(const DataType &dt) {
			switch (dt) {
				case DT_AirTemperature:						return m_airTemp;		break;
				case DT_RelativeHumidity:					return m_relHum;		break;
				case DT_RadiationDiffuse:					return m_diffRad;		break;
				case DT_RadiationGlobal:					return m_globalRad;		break;
				case DT_RadiationLongWave:					return m_counterRad;	break;
				case DT_ZenithAngle:						return m_zenithAngle;	break;
				case DT_WindDirection:						return m_windDirection;	break;
				case DT_WindSpeed:							return m_windSpeed;		break;
				case DT_Pressure:							return m_pressure;		break;
				case DT_Precipitation:						return m_precipitation;	break;

			}
		}

		double				m_airTemp		= -999;		///< Air temperature in C
		double				m_relHum		= -999;		///< Relative humidity in %
		double				m_counterRad	= -999;		///< Long wave counter radiation in W/m2
		double				m_diffRad		= -999;		///< Diffuse radiation W/m2
		double				m_globalRad		= -999;		///< Globale radiation W/m2
		double				m_zenithAngle	= -999;		///< ZenithAngle in Deg
		double				m_sunElevation	= -999;		///< Sun elavation angle in Deg
		double				m_windSpeed		= -999;		///< Wind speed in m/s
		double				m_windDirection	= -999;		///< Wind direction in Deg
		double				m_pressure		= -999;		///< Pressure in Pa
		double				m_precipitation	= -999;		///< Precipitation in mm


	};


	/*! Reads all given dwd files and creates the data. */
	void createData(IBK::NotificationHandler * notify, const std::map<IBK::Path, std::set<DWDData::DataType>> &filenames, unsigned int intervalDuration = 3600);

	/*! Add a data line from dwd file.	*/
	void addDataLine(std::string &line, const std::set<DataType> &dataType);

	/*! Create a tsv-file for one year. */
	void writeTSV(unsigned int year);

	/*! Create an epw-file for one year. */
	void exportEPW(CCM::ClimateDataLoader &loader, double latitudeDeg, double longitudeDeg, IBK::Path &exportPath);

	/*! Creat an c6b-file with continous data. */
	void exportC6B(CCM::ClimateDataLoader &loader, double latitudeDeg, double longitudeDeg, IBK::Path &exportPath);

	/*! Returns the complete url as QString needed for download. */
	QString urlFilename(const DataType &type, const QString &numberString, const std::string &dateString,
						bool isRecent=true, const QString &filename="") const;

	/*! Returns the filename of the downloaded zip-archive */
	QString filename(const DWDData::DataType &type, const QString &numberString, const std::string &dateString, bool isRecent=true) const;

	/*! Returns interval data. */
	std::vector<IntervalData>	data() const;



	IBK::Time						m_startTime;                ///> Start time for the first interval data
	IBK::Time						m_endTime;					///> End time for the first interval data
	unsigned int					m_intervalDuration;         ///> Interval duration in sec
	std::vector<IntervalData>		m_data;                     ///> Vector with interval data
	IBK::Path						m_filenames[NUM_DT];        ///> Filename
	std::vector<QUrlInfo>			m_urls;                     ///> urls in ftp directory
	QProgressDialog					*m_progressDlg = nullptr;   ///> pointer to label
	QString							dataString[6];


signals:
	void progress(int min, int max, int val);

public slots:
	void findFile(const QUrlInfo&);

private:
	/*! Returns the column for a given data type in the different dwd weather files. */
	unsigned int getColumnDWD(const DataType &dt);
};

#endif // DWDDataH
