#include "DM_MapDialog.h"
#include "DM_Conversions.h"
#include "ui_DM_MapDialog.h"

#include "DM_Scene.h"


namespace DM {

MapDialog::MapDialog(QWidget *parent) :
    QDialog(parent),
	m_ui(new Ui::MapDialog)
{
	m_ui->setupUi(this);

	// init Scene
	m_scene = new Scene(this);
	m_ui->graphicsViewMap->setScene(m_scene);
	m_scene->setSceneRect(m_scene->sceneRect());

	m_ui->lineEditLatitude->setup(-90,90, "Latitude in Deg", true, true);
	m_ui->lineEditLongitude->setup(-180,180, "Longitude in Deg", true, true);

	connect(m_scene, &DM::Scene::updatedLocation, this, &DM::MapDialog::onUpdateLocation);
	// draw al data points
//	for(const Data &d : m_data)
//		m_scene->addDwdDataPoint(d.m_type, d.m_name, d.m_minDate, d.m_maxDate, d.m_latitude, d.m_longitude);

	m_ui->checkBoxAirTemp->setStyleSheet( QString("QCheckBox { color: %1; }").arg(colorFromDataType(DM::Data::DT_TemperatureAndHumidity).name()));
	m_ui->checkBoxPrecipitation->setStyleSheet( QString("QCheckBox { color: %1; }").arg(colorFromDataType(DM::Data::DT_Precipitation).name()));
	m_ui->checkBoxPressure->setStyleSheet( QString("QCheckBox { color: %1; }").arg(colorFromDataType(DM::Data::DT_Pressure).name()));
	m_ui->checkBoxRadiation->setStyleSheet( QString("QCheckBox { color: %1; }").arg(colorFromDataType(DM::Data::DT_Solar).name()));
	m_ui->checkBoxWind->setStyleSheet( QString("QCheckBox { color: %1; }").arg(colorFromDataType(DM::Data::DT_Wind).name()));
}

MapDialog::~MapDialog() {
	delete m_ui;
}

void MapDialog::setCoordinates(double latitude, double longitude) {
	m_ui->lineEditLatitude->setText(QString::fromStdString(std::to_string(latitude)));
	m_ui->lineEditLongitude->setText(QString::fromStdString(std::to_string(longitude)));

	m_longitude = longitude;
	m_latitude = latitude;
}

void MapDialog::setDistance(double distance) {
	m_ui->lineEditDistance->setValue(distance);
	m_distance = distance;

	double value = 5 + distance * 2. / 1.1;
	m_scene->m_locationItem->setRect(-value/2,-value/2,value,value);
}

void MapDialog::onUpdateLocation() {
	setCoordinates(m_scene->m_latitude, m_scene->m_longitude);

	emit updateDistances();
}


void MapDialog::on_checkBoxAirTemp_toggled(bool checked) {
	m_scene->setItemGroupVisiblity(Data::DT_TemperatureAndHumidity, checked);
}


void MapDialog::on_checkBoxRadiation_toggled(bool checked) {
	m_scene->setItemGroupVisiblity(Data::DT_Solar, checked);
}


void MapDialog::on_checkBoxPressure_toggled(bool checked) {
	m_scene->setItemGroupVisiblity(Data::DT_Pressure, checked);
}


void MapDialog::on_checkBoxWind_toggled(bool checked) {
	m_scene->setItemGroupVisiblity(Data::DT_Wind, checked);
}


void MapDialog::on_checkBoxPrecipitation_toggled(bool checked) {
	m_scene->setItemGroupVisiblity(Data::DT_Precipitation, checked);
}

void MapDialog::showEvent(QShowEvent * event) {
	// fit scene in view
	m_ui->graphicsViewMap->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

void MapDialog::resizeEvent(QResizeEvent *event) {
	m_ui->graphicsViewMap->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

void MapDialog::on_lineEditLongitude_editingFinishedSuccessfully() {
	double longi = m_ui->lineEditLongitude->value();
	m_longitude = longi;

	QPointF pos = convertCoordinatesToPos(m_scene->m_mapSvgItem->boundingRect(), m_latitude, longi);
	m_scene->m_locationItem->setPos(pos);

	emit updateDistances();
}


void MapDialog::on_lineEditLatitude_editingFinishedSuccessfully() {
	double lati = m_ui->lineEditLatitude->value();
	m_longitude = lati;

	QPointF pos = convertCoordinatesToPos(m_scene->m_mapSvgItem->boundingRect(), m_latitude, lati);
	m_scene->m_locationItem->setPos(pos);

	emit updateDistances();
}


void MapDialog::on_horizontalSliderDistance_valueChanged(int distance) {
	setDistance(distance);

	emit updateDistances();
}


void MapDialog::on_horizontalSliderSize_valueChanged(int value) {
	m_scene->m_dataPointSize = value;
}

} // namespace DM
