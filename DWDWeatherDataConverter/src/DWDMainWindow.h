#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>

#include <QMainWindow>
#include <QWidget>
#include <QProcess>
#include <QElapsedTimer>
#include <QStandardItemModel>
#include <QFile>

#include <IBK_Time.h>
#include <IBK_Path.h>

#include <DM_MapDialog.h>

#include "DWDDescriptonData.h"
#include "DWDData.h"
#include "DWDTableModel.h"
#include "DWDConversions.h"
#include "MetaDataEditWidget.h"

#include "QtExt_Directories.h"

#include <qwt_plot.h>

namespace Ui {
class MainWindow;
}

class QProgressDialog;
class QAbstractProxyModel;
class QTableWidgetItem;
class DWDDownloader;
class DWDMap;
class DWDSortFilterProxyModel;
class DWDProgressBar;
class DWDLogWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	enum ExportMode {
		EM_EPW,
		EM_C6B,
		NUM_EM
	};


public:

	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	/*! If each datatype has one local file checked, enable button */
	void updateDownloadButton();

	/*! Iterates over local files in m_downloadDir and writes to m_localFileList */
	void updateLocalFileList();

	/*! Download data from DWD server. */
	void extracted();
	void loadDataFromDWDServer();

	/*! Sets all GUI states. */
	void setGUIState(bool guiState);

	/*! Downloads all dwd data. */
	void downloadData(bool showPreview = true, bool exportEPW = false);

	void addToList(const QUrlInfo qUrlI);

	/*! Function that calculates all distances to the Reference location. */
	void calculateDistances();

	/*! Inits all plots. */
	void formatPlots(bool init = false);

	/*! Formats a qwt plot. */
	void formatQwtPlot(bool init, QwtPlot &plot, QDate startDate, QDate endDate, QString title, QString leftYAxisTitle, double yLeftMin, double yLeftMax, double yLeftStepSize,
					   bool hasRightAxis = false, QString rightYAxisTitle = "", double yRightMin = 0, double yRightMax = 100, double yRightStepSize = 0);

protected:
	/*! Override resize event. */
	void resizeEvent(QResizeEvent* event) override;

private slots:
	void convertDwdData();

	/*! Updates all distances. */
	void onUpdateDistances();

	/*! Updates all distances. */
	void onLocationDistances(double latitude, double longitude);

	/*! Updates the Maximum height of the plots, when the main window is beeing resized. */
	void updateMaximumHeightOfPlots();

	void on_pushButtonDownload_clicked();

	void on_pushButtonMap_clicked();

	void setProgress(int min, int max, int val);

	void on_radioButtonHistorical_toggled(bool checked);

	void on_lineEditNameFilter_textChanged(const QString &filter);

	void on_horizontalSliderDistance_valueChanged(int value);

	void on_pushButtonPreview_clicked();

	void on_comboBoxMode_currentIndexChanged(int index);

	void on_dateEditStart_dateChanged(const QDate &date);

	void on_dateEditEnd_dateChanged(const QDate &date);

	void on_toolButtonHelp_clicked();

	void on_toolButtonDownloadDir_clicked();

private:

	/*! Updates the Ui. */
	void updateUi();

	Ui::MainWindow								*m_ui;

	/*! Pointer to download manager. */
	DWDDownloader								*m_manager = nullptr;

	/*! Pointer to Map Widget. */
	DM::MapDialog								*m_mapDialog = nullptr;

	/*! Description input of all existing DWD stations.
		\param key: station id
	*/
	std::vector<DWDDescriptonData>				m_descData;
	DWDData										m_dwdData;

	QStringList									m_filelist;

	bool										m_guiState = true;
	bool										m_generateEpwEnabled = false;

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
};

#endif // MAINWINDOW_H
