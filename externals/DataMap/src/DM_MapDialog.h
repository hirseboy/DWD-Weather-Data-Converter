#ifndef DM_MapDialogH
#define DM_MapDialogH

#include <QDialog>

#include "DM_GraphicsView.h"
#include "DM_Scene.h"

namespace DM {

namespace Ui {
class MapDialog;
}

class MapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapDialog(QWidget *parent = nullptr);
    ~MapDialog();

	/*! Pointer to Scene. */
	DM::Scene				*m_scene = nullptr;


private slots:
	void on_checkBoxAirTemp_toggled(bool checked);

	void on_checkBoxRadiation_toggled(bool checked);

	void on_checkBoxPressure_toggled(bool checked);

	void on_checkBoxWind_toggled(bool checked);

	void on_checkBoxPrecipitation_toggled(bool checked);

private:
	Ui::MapDialog			*m_ui;

};

} // namespace DM
#endif // DM_MapDialogH