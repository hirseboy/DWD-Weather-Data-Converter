#ifndef DM_DataMapWidgetH
#define DM_DataMapWidgetH

#include <QWidget>

#include "DM_GraphicsView.h"
#include "DM_Scene.h"

namespace DM {

namespace Ui {
class DataMapWidget;
}

/*! Class with a tiny map viewer with map of Germany.
	Basis is a graphics scene with a graphics viewer,
	that loads a svg map. */
class DataMapWidget : public QWidget {
    Q_OBJECT

public:
	explicit DataMapWidget(QWidget *parent = nullptr);
	~DataMapWidget();

	/*! Pointer to Scene. */
	DM::Scene				*m_scene = nullptr;

	/*! Latitude of location. */
	double					m_latitude;

	/*! Longitude of location. */
	double					m_longitude;

	/*! Distance of location. */
	double					m_distance = 50.0;

	/*! Sets the coordinates in the line edit. */
	void setCoordinates(double latitude, double longitude);

	/*! Sets the current distance. */
	void setDistance(double distance);

signals:
	void updateDistances();

	void updatelocation(double latitude, double longitude);

private slots:
	/*! Is called in order to update Location. */
	void onUpdateLocation();

	void on_checkBoxAirTemp_toggled(bool checked);

	void on_checkBoxRadiation_toggled(bool checked);

	void on_checkBoxPressure_toggled(bool checked);

	void on_checkBoxWind_toggled(bool checked);

	void on_checkBoxPrecipitation_toggled(bool checked);

	void on_lineEditLongitude_editingFinishedSuccessfully();

	void on_lineEditLatitude_editingFinishedSuccessfully();

	void on_horizontalSliderDistance_valueChanged(int value);

private:
	Ui::DataMapWidget			*m_ui;

protected:
	void showEvent(QShowEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
};

} // namespace DM
#endif // DM_DataMapWidgetH
