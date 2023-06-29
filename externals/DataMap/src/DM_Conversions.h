#ifndef DM_ConversionsH
#define DM_ConversionsH

#include "DM_Scene.h"
#include "DM_Data.h"


namespace DM {

/*! Converts coordinates to a position at the scene. */
static QPointF convertCoordinatesToPos(QRectF rect, const double &lat, const double &lon) {
	double height = rect.height();
	double width = rect.width();

	double			MIN_LON = 5.866345;
	double			MAX_LON = 15.042245;
	double			MIN_LAT = 47.2705;
	double			MAX_LAT = 55.036572;
	double			DIFF = 0.4;

	QPointF topLeftPoint = rect.topLeft();


	double diffLon = -0.1 + DIFF - 2*DIFF * (lat - MIN_LAT)/(MAX_LAT - MIN_LAT);
    double diffLat = lat * (lat - MIN_LON)/(MAX_LON - MIN_LON);

	MIN_LON = MIN_LON + diffLon;
	MAX_LON = MAX_LON - diffLon;

	QPointF pos;
	pos.setX( topLeftPoint.x() + width  + width  * (MAX_LON - lon) / ( MIN_LON - MAX_LON ) );
	pos.setY( topLeftPoint.y() + height - height * (lat - MIN_LAT) / ( MAX_LAT - MIN_LAT ) );

	return pos;
}

/*! Converts point in scnene to coordinates. */
static void convertPosToCoordinates(const QPointF pos, const double &mapHeight, const double &mapWidth,
									double &lat, double &lon) {

	double			MIN_LON = 5.866345;
	double			MAX_LON = 15.042245;
	double			MIN_LAT = 47.2705;
	double			MAX_LAT = 55.036572;

	lat = MIN_LAT + ( mapHeight - pos.y() ) / mapHeight * ( MAX_LAT - MIN_LAT );
	lon = MAX_LON - ( mapWidth  - pos.x() ) / mapWidth  * ( MAX_LON - MIN_LON );
}

static QColor colorFromDataType(const Data::DataType &dt) {

	QColor color;
	/*! Map with colors of data. */
	switch (dt) {
		case Data::DT_TemperatureAndHumidity:
			color = QColor("#AA0000");
		break;
		case Data::DT_Precipitation:
			color = QColor("#008000");
		break;
		case Data::DT_Solar:
			color = QColor("#E9B44C");
		break;
		case Data::DT_Wind:
			color = QColor("#004E98");
		break;
		case Data::DT_Pressure:
			color = QColor("#D45500");
		case Data::NUM_DT:
		break;
	}

//	colorData[DWDDataType::D_Solar]				= QColor("#E9B44C");
//	colorData[DWDDataType::D_Precipitation]		= QColor("#008000");
//	colorData[DWDDataType::D_Pressure]			= QColor("#D45500");
//	colorData[DWDDataType::D_TemperatureAndHumidity] = QColor("#AA0000");
//	colorData[DWDDataType::D_Wind]				= QColor("#004E98");

	color.setAlpha(240);
	return color;
}


}

#endif // DM_ConversionsH
