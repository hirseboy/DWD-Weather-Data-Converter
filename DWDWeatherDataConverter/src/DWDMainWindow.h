#ifndef DWDMainWindowH
#define DWDMainWindowH

#include <QMainWindow>
#include <QWidget>
#include <QProcess>
#include <QElapsedTimer>
#include <QStandardItemModel>
#include <QFile>

#include <IBK_Time.h>
#include <IBK_Path.h>

#include <DM_DataMapWidget.h>

#include "DWDAboutDialog.h"
#include "DWDDescriptonData.h"
#include "DWDData.h"
#include "DWDTableModel.h"
#include "DWDConversions.h"
#include "DWDPlotZoomer.h"

#include "MetaDataEditWidget.h"

#include "QtExt_Directories.h"

#include <qwt_plot.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_zoomer.h>

namespace Ui {
class DWDMainWindow;
}

class QProgressDialog;
class QAbstractProxyModel;
class QTableWidgetItem;
class DWDDownloader;
class DWDMap;
class DWDSortFilterProxyModel;
class DWDProgressBar;
class DWDLogWidget;


class DWDMainWindow : public QMainWindow {
	Q_OBJECT

	/*! Export modes. */
	enum ExportMode {
		EM_EPW,
		EM_C6B,
		NUM_EM
	};

	/*! Tab modes. */
	enum Tab {
		T_Settings,
		T_Location,
		T_DataTable,
		T_Plots,
		NUM_T
	};

public:

	explicit DWDMainWindow(QWidget *parent = nullptr);
	~DWDMainWindow();

	/*! If each datatype has one local file checked, enable button */
	void updateDownloadButton();

	/*! Iterates over local files in m_downloadDir and writes to m_localFileList */
	void updateLocalFileList();

	/*! Download data from DWD server. */
	void extracted();

	/*! Load DWD Data from Server. */
	void loadDataFromDWDServer();

	/*! Align left axis of QWT Plot. */
	void alignLeftAxisQwtPlots();

	/*! Sets all GUI states. */
	void updateGuiState();

	/*! Downloads all dwd data. */
	bool downloadAndConvertDwdData(bool showPreview = true, bool exportEPW = false);

	void addToList(const QUrlInfo qUrlI);

	/*! Function that calculates all distances to the Reference location. */
	void calculateDistances();

	/*! Inits all plots. */
	void formatPlots(bool init = false);

	/*! Formats a qwt plot. */
	void formatQwtPlot(bool init, QwtPlot &plot, QDate startDate, QDate endDate, QString title, QString leftYAxisTitle, double yLeftMin, double yLeftMax, double yLeftStepSize,
					   bool hasRightAxis = false, QString rightYAxisTitle = "", double yRightMin = 0, double yRightMax = 100, double yRightStepSize = 0);

	/*! Add language action. */
	void addLanguageAction(const QString &langId, const QString &actionCaption) ;

protected:
	void resizeEvent(QResizeEvent *event) override;

private slots:
	void convertDwdData();

	/*! Triggered when a language menu entry was clicked. */
	void onActionSwitchLanguage();

	/*! Updates all distances. */
	void onUpdateDistances();

	/*! Update plot zoom. */
	void onUpdatePlotZooming(const QRectF &rect);

	/*! Updates Location data in top view. */
	void onUpdateLocation(DWDDescriptonData::DWDDataType dataType, const QString &location);

	void on_pushButtonDownload_clicked();

	void on_pushButtonMap_clicked();

	void setProgress(int min, int max, int val);

	void on_radioButtonHistorical_toggled(bool checked);

	void on_lineEditNameFilter_textChanged(const QString &filter);

	void on_pushButtonPreview_clicked();

	void on_comboBoxMode_currentIndexChanged(int index);

	void on_dateEditStart_dateChanged(const QDate &startDate);

	void on_dateEditEnd_dateChanged(const QDate &enddate);

	void on_toolButtonHelp_clicked();

	void on_toolButtonDownloadDir_clicked();

	void on_actionAbout_triggered();

	void on_actionAbout_Qt_triggered();

	void on_actionClose_triggered();

	void on_actionc6b_triggered();

	void on_actionEPW_triggered();

	void on_actionShow_log_widget_triggered();

	void on_horizontalSliderDistance_valueChanged(int value);

	void on_tabWidget_currentChanged(int index);

	void on_checkBoxPres_toggled(bool checked);

	void on_actiondark_triggered();

	void on_actionwhite_triggered();

	void on_actionSaveAs_triggered();

private:

	/*! Updates the Ui. */
	void updateUi();

	/*! Generate check boxes. */
	void generateCheckBox(const QString &str, const QwtPlot *plot, const QColor &color);

	/*! Pointer to progress Dialog. */
	QProgressDialog *progressDialog();

	Ui::DWDMainWindow							*m_ui;

	/*! Pointer to download manager. */
	DWDDownloader								*m_manager = nullptr;

	/*! Pointer to Map Widget. */
	DM::DataMapWidget							*m_mapWidget = nullptr;

	/*! Pointer to Map Widget. */
	DWDAboutDialog								*m_aboutDialog = nullptr;

	/*! Description input of all existing DWD stations.	*/
	std::vector<DWDDescriptonData>				m_descData;
	DWDData										m_dwdData;

	QStringList									m_filelist;

	bool										m_guiState = true;
	bool										m_enableClimateFileGeneration = false;

	DWDMap										*m_dwdMap;

	QStandardItemModel							*m_model;
	QProgressDialog								*m_progressDlg;

	/*! Table model instance for dwd data. */
	DWDTableModel								*m_dwdTableModel = nullptr;
	DWDSortFilterProxyModel						*m_proxyModel = nullptr;
	QAbstractProxyModel							*m_abstractProxyModel = nullptr;

	IBK::Path									m_exportPath;

	DWDLogWidget								*m_logWidget = nullptr;


	IBK::Path									m_downloadDir;

	/*! List of local files, that have been already downloaded. */
	std::vector<std::string>					m_localFileList;

	/*! Current working mode such as EPW and C6b. */
	ExportMode									m_mode;

	/*! Meta Data Widget. */
	MetaDataEditWidget							*m_metaDataWidget = nullptr;

	/*! Climate data loader. */
	CCM::ClimateDataLoader						m_ccm;

	/*! Indicates if there is currently valid data. */
	bool										m_validData = false;

	/*! Current selected location. */
	QString										m_currentLocation[DWDDescriptonData::NUM_D];

	/*! Deletes currently downloaded files. When download was canceled by user.
		So, no corrupted cached files will be read. */
	bool										m_deleteCachedFiles = false;

	/*! Plot picker. */
	QwtPlotPicker								*m_plotPickerTemp = nullptr;
	QwtPlotPicker								*m_plotPickerRelHum = nullptr;
	QwtPlotPicker								*m_plotPickerPressure = nullptr;
	QwtPlotPicker								*m_plotPickerRad = nullptr;
	QwtPlotPicker								*m_plotPickerRain = nullptr;
	QwtPlotPicker								*m_plotPickerLongWave = nullptr;
	QwtPlotPicker								*m_plotPickerWind = nullptr;

	/*! Plot zoomer. */
	DWDPlotZoomer								*m_plotZoomerTemp = nullptr;
	DWDPlotZoomer								*m_plotZoomerRelHum = nullptr;
	DWDPlotZoomer								*m_plotZoomerPressure = nullptr;
	DWDPlotZoomer								*m_plotZoomerRad = nullptr;
	DWDPlotZoomer								*m_plotZoomerRain = nullptr;
	DWDPlotZoomer								*m_plotZoomerLongWave = nullptr;
	DWDPlotZoomer								*m_plotZoomerWind = nullptr;

	/*! Close event. */
	void closeEvent(QCloseEvent *event) override;

	// QWidget interface
};

#endif // MAINWINDOW_H
