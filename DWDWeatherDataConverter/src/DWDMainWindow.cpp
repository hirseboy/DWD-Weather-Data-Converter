#include "DWDMainWindow.h"
#include "ui_DWDMainWindow.h"

#include "qaction.h"

#include <stdio.h>

#include "DM_Conversions.h"
#include "DM_Data.h"

#include <IBK_NotificationHandler.h>
#include <IBK_messages.h>
#include <IBK_FileReader.h>
#include <IBK_physics.h>
#include <IBK_MessageHandler.h>
#include <IBK_Exception.h>

#include <QtExt_Directories.h>
#include <QtExt_ValidatingLineEdit.h>

#include <QString>
#include <QSlider>
#include <QCheckBox>
#include <QStandardItemModel>
#include <QAbstractTableModel>
#include <QAbstractProxyModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QPicture>
#include <QProgressDialog>
#include <QTableWidgetItem>
#include <QResizeEvent>
#include <QTimer>
#include <QThread>
#include <QMessageBox>
#include <QItemDelegate>
#include <QtNumeric>
#include <QDate>
#include <QDebug>
#include <QHBoxLayout>

#include <qwt_plot_curve.h>
#include <qwt_series_data.h>
#include <qwt_date_scale_draw.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_grid.h>
#include <qwt_date_scale_engine.h>
#include <qwt_scale_div.h>
#include "qwt_scale_widget.h"
#include "qwt_plot_zoomer.h"

#include <qftp.h>

#include <JlCompress.h>

#include "DWDDownloader.h"
#include "DWDData.h"
#include "DWDSortFilterProxyModel.h"
#include "DWDLogWidget.h"
#include "DWDMessageHandler.h"
#include "DWDDelegate.h"
#include "DWDSettings.h"
#include "DWDConversions.h"

#include "DWDConstants.h"
#include "DWDTimePlotPicker.h"
#include "DWDUtilities.h"


class ProgressNotify : public IBK::NotificationHandler {
public:
	ProgressNotify(QProgressDialog *bar):
		m_dlg(bar)
	{
		m_dlg->setMaximum(100);
		m_dlg->show();
		m_dlg->setMinimumWidth(300);
	}

	/*! Reimplement this function in derived child 'notification' objects
		to provide whatever notification operation you want to perform.
	*/
	virtual void notify() {
		FUNCID(notify);
		if(m_dlg->value() == m_value)
			return;
		m_dlg->setValue(m_value);
		// qDebug() << m_dlg->value() << " | " << m_dlg->maximum();

		qApp->processEvents();
		if(m_dlg->wasCanceled()){
			throw IBK::Exception("Operation canceled by user", FUNC_ID);
		}
	}

	/*! Reimplement this function in derived child 'notification' objects
		to provide whatever notification operation you want to perform.

		The default implementation calls notify().
	*/
	virtual void notify(double p) {
		m_value = 100 * p;
		notify();
	}

	QProgressDialog			*m_dlg;
	int						m_value;
};

DWDMainWindow::DWDMainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::DWDMainWindow),
	m_progressDlg(nullptr),
	m_dwdTableModel(new DWDTableModel(this)),
	m_proxyModel(new DWDSortFilterProxyModel(this))
{
	m_ui->setupUi(this);

	m_metaDataWidget = m_ui->widgetMetaData;
	m_metaDataWidget->m_ccm = &m_ccm;

	m_ccm.m_latitudeInDegree = 51.03;
	m_ccm.m_longitudeInDegree = 13.7;
	m_model = new QStandardItemModel();

	m_ui->lineEditDistance->setup(0,1000, "Distance in km", true, true);
	//	m_ui->lineEditLatitude->setup(-90,90, "Latitude in Deg", true, true);
	//	m_ui->lineEditLongitude->setup(-180,180, "Longitude in Deg", true, true);
	//	m_ui->lineEditLatitude->setReadOnly(true);
	//	m_ui->lineEditLongitude->setReadOnly(true);
	//	m_ui->lineEditLatitude->setText(QString::number(m_ccm.m_latitudeInDegree));
	//	m_ui->lineEditLongitude->setText(QString::number(m_ccm.m_longitudeInDegree));

	m_ui->dateEditStart->blockSignals(true);
	m_ui->dateEditEnd->blockSignals(true);

	QDate startDate = QDate(2020,1,1);
	QDate endDate = QDate(2021,1,1);

	m_ui->dateEditEnd->setDate(endDate);
	m_ui->dateEditStart->setDate(startDate);

	m_dwdData.m_endTime.set(endDate.year(), endDate.month()-1, endDate.day()-1, 0 );
	m_proxyModel->setFilterMaximumDate(endDate);

	m_dwdData.m_startTime.set(startDate.year(), startDate.month()-1, startDate.day()-1, 0 );
	m_proxyModel->setFilterMinimumDate(startDate);

	m_ui->dateEditStart->setMinimumDate(QDate(1950,1,1));
	m_ui->dateEditStart->setMaximumDate(QDate::currentDate());

	m_ui->dateEditEnd->setMinimumDate(QDate(1950,1,1));
	m_ui->dateEditEnd->setMaximumDate(QDate::currentDate());

	m_ui->dateEditStart->blockSignals(false);
	m_ui->dateEditEnd->blockSignals(false);

	QTableView * v = m_ui->tableView;
	v->verticalHeader()->setDefaultSectionSize(25);
	v->verticalHeader()->setVisible(false);
	v->horizontalHeader()->setMinimumSectionSize(19);
	v->setSelectionBehavior(QAbstractItemView::SelectRows);
	v->setSelectionMode(QAbstractItemView::NoSelection);
	v->setAlternatingRowColors(true);
	v->setSortingEnabled(false);
	v->sortByColumn(DWDTableModel::ColDistance, Qt::AscendingOrder);
	// smaller font for entire table
	QFont f;
#ifdef Q_OS_LINUX
	f.setPointSizeF(f.pointSizeF()*0.8);
#endif // Q_OS_WIN
	v->setFont(f);
	v->horizontalHeader()->setFont(f); // Note: on Linux/Mac this won't work until Qt 5.11.1 - this was a bug between Qt 4.8...5.11.1


	// Init combo box for program mode
	m_ui->comboBoxMode->addItem("epw", EM_EPW);
	m_ui->comboBoxMode->addItem("c6b", EM_C6B);
	m_ui->comboBoxMode->setCurrentIndex(EM_EPW);
	m_mode = EM_EPW;

	// m_ui->splitter->installEventFilter(this);
	connect( &m_dwdData, &DWDData::progress, this, &DWDMainWindow::setProgress );
	// connect( m_dwdTableModel, &DWDTableModel::dataChanged, this, &MainWindow::updateDownloadButton);
	// double scaleFactor = this->devicePixelRatioF();
	//resize(scaleFactor*1900, scaleFactor*1000);

	// Add the Dockwidget
	m_logWidget = new DWDLogWidget;
	this->addDockWidget(Qt::BottomDockWidgetArea, m_logWidget);
	m_logWidget->hide();

	// Also connect all IBK::Messages to Log Widget
	DWDMessageHandler * msgHandler = dynamic_cast<DWDMessageHandler *>(IBK::MessageHandlerRegistry::instance().messageHandler() );
	connect(msgHandler, &DWDMessageHandler::msgReceived, m_logWidget, &DWDLogWidget::onMsgReceived);


	// Init Map Widget
	m_mapWidget = m_ui->widgetLocation;
	connect(m_mapWidget, &DM::DataMapWidget::updateDistances, this, &DWDMainWindow::onUpdateDistances);
	//	m_mapWidget->m_latitude = m_ccm.m_latitudeInDegree;
	//	m_mapWidget->m_longitude = m_ccm.m_longitudeInDegree;

	double distance = 50;
	on_horizontalSliderDistance_valueChanged(distance);
	m_mapWidget->setCoordinates(m_ccm.m_latitudeInDegree, m_ccm.m_longitudeInDegree);
	m_mapWidget->setDistance(distance);

	// Initialize download directory
	m_downloadDir = IBK::Path(QtExt::Directories().userDataDir().toStdString());

	// *** Populate language menu ***
	addLanguageAction("en", "English");
	addLanguageAction("de", "Deutsch");

	// updat UI
	updateUi();

	m_plotPickerTemp = new DWDTimePlotPicker( m_ui->plotTemp->canvas(), "°C" );
	m_plotPickerPressure = new DWDTimePlotPicker( m_ui->plotPres->canvas(), "kPa" );
	m_plotPickerRad = new DWDTimePlotPicker( m_ui->plotRad->canvas(), "W/m2" );
	m_plotPickerRain = new DWDTimePlotPicker( m_ui->plotRain->canvas(), "mm" );
	m_plotPickerRelHum = new DWDTimePlotPicker( m_ui->plotRelHum->canvas(), "%" );
	m_plotPickerLongWave = new DWDTimePlotPicker( m_ui->plotRadLongWave->canvas(), "W/m2" );
	m_plotPickerWind = new DWDTimePlotPicker( m_ui->plotWind->canvas(), "m/s" );

	m_plotZoomerTemp = new DWDPlotZoomer( m_ui->plotTemp->canvas() );
	m_plotZoomerTemp->setAxis( QwtPlot::xBottom, QwtPlot::yRight);

	m_plotZoomerPressure = new DWDPlotZoomer( m_ui->plotPres->canvas() );
	m_plotZoomerPressure->setAxis( QwtPlot::xBottom, QwtPlot::yRight);

	m_plotZoomerRad = new DWDPlotZoomer( m_ui->plotRad->canvas() );
	m_plotZoomerRad->setAxis( QwtPlot::xBottom, QwtPlot::yRight);

	m_plotZoomerRain = new DWDPlotZoomer( m_ui->plotRain->canvas() );
	m_plotZoomerRain->setAxis( QwtPlot::xBottom, QwtPlot::yRight);

	m_plotZoomerRelHum = new DWDPlotZoomer( m_ui->plotRelHum->canvas() );
	m_plotZoomerRelHum->setAxis( QwtPlot::xBottom, QwtPlot::yRight);

	m_plotZoomerLongWave = new DWDPlotZoomer( m_ui->plotRadLongWave->canvas() );
	m_plotZoomerLongWave->setAxis( QwtPlot::xBottom, QwtPlot::yRight);

	m_plotZoomerWind = new DWDPlotZoomer( m_ui->plotWind->canvas() );
	m_plotZoomerWind->setAxis( QwtPlot::xBottom, QwtPlot::yRight);

	// connect plot zoomer
	connect(m_plotZoomerTemp, &QwtPlotZoomer::zoomed, this, &DWDMainWindow::onUpdatePlotZooming);
	connect(m_plotZoomerPressure, &QwtPlotZoomer::zoomed, this, &DWDMainWindow::onUpdatePlotZooming);
	connect(m_plotZoomerRad, &QwtPlotZoomer::zoomed, this, &DWDMainWindow::onUpdatePlotZooming);
	connect(m_plotZoomerRelHum, &QwtPlotZoomer::zoomed, this, &DWDMainWindow::onUpdatePlotZooming);
	connect(m_plotZoomerRain, &QwtPlotZoomer::zoomed, this, &DWDMainWindow::onUpdatePlotZooming);
	connect(m_plotZoomerLongWave, &QwtPlotZoomer::zoomed, this, &DWDMainWindow::onUpdatePlotZooming);
	connect(m_plotZoomerWind, &QwtPlotZoomer::zoomed, this, &DWDMainWindow::onUpdatePlotZooming);

	connect(m_ui->horizontalSliderDistance, &QSlider::valueChanged, this, &DWDMainWindow::on_horizontalSliderDistance_valueChanged);

	// connect Table model to get updates when clicked
	connect(m_dwdTableModel, &DWDTableModel::updateLocation, this, &DWDMainWindow::onUpdateLocation);

	QColor color;
	color = DM::colorFromDataType(DM::Data::DT_TemperatureAndHumidity);
	m_ui->labelTemperature->setStyleSheet(QString("QLabel { color : %1; }").arg(color.name()));
	color = DM::colorFromDataType(DM::Data::DT_Precipitation);
	m_ui->labelPrecipitation->setStyleSheet(QString("QLabel { color : %1; }").arg(color.name()));
	color = DM::colorFromDataType(DM::Data::DT_Pressure);
	m_ui->labelPressure->setStyleSheet(QString("QLabel { color : %1; }").arg(color.name()));
	color = DM::colorFromDataType(DM::Data::DT_Solar);
	m_ui->labelRadiation->setStyleSheet(QString("QLabel { color : %1; }").arg(color.name()));
	color = DM::colorFromDataType(DM::Data::DT_Wind);
	m_ui->labelWind->setStyleSheet(QString("QLabel { color : %1; }").arg(color.name()));

	QFont font;
	font.setItalic(true);

	m_ui->labelTextPrecipitation->setFont(font);
	m_ui->labelTextPressure->setFont(font);
	m_ui->labelTextRadiation->setFont(font);
	m_ui->labelTextTemperature->setFont(font);
	m_ui->labelTextWind->setFont(font);

	m_ui->tableView->update();
	m_ui->tabWidget->setCurrentIndex(T_Settings);

	m_ui->graphicsViewMap->setScene(m_mapWidget->m_scene);
	onUpdateDistances();

//	m_ui->plotLayoutPreview->setSizeConstraint(QLayout::SetFixedSize);

	// *** Add Checkboxes ***

	generateCheckBox(tr("Temperature"), m_ui->plotTemp, DWDDescriptonData::color(DWDDescriptonData::D_TemperatureAndHumidity), PT_Temperature);
	generateCheckBox(tr("Relative humidity"), m_ui->plotRelHum, QColor("#7F2AFF"), PT_RelativeHumidity);
	generateCheckBox(tr("Short-wave radiation"), m_ui->plotRad, DWDDescriptonData::color(DWDDescriptonData::D_Solar), PT_ShortWaveRadiation);
	generateCheckBox(tr("Long-wave radiation"), m_ui->plotRadLongWave, QColor("#871ca4"), PT_LongWaveRadiation);
	generateCheckBox(tr("Pressure"), m_ui->plotPres, DWDDescriptonData::color(DWDDescriptonData::D_Pressure), PT_Pressure);
	generateCheckBox(tr("Precipiation"), m_ui->plotRain, DWDDescriptonData::color(DWDDescriptonData::D_Precipitation), PT_Precipitation);
	generateCheckBox(tr("Wind"), m_ui->plotWind, DWDDescriptonData::color(DWDDescriptonData::D_Wind), PT_WindSpeed);

	// init all plots
	formatPlots(true);
	m_ui->plotLayoutPreview->setSizeConstraint(QLayout::SetFixedSize);

	m_ui->plotTemp->setMinimumHeight(50);
	m_ui->plotPres->setMinimumHeight(50);
	m_ui->plotRad->setMinimumHeight(50);
	m_ui->plotRadLongWave->setMinimumHeight(50);
	m_ui->plotRain->setMinimumHeight(50);
	m_ui->plotWind->setMinimumHeight(50);
	m_ui->plotRelHum->setMinimumHeight(50);
}


DWDMainWindow::~DWDMainWindow() {
	delete m_ui;
}

void DWDMainWindow::updateDownloadButton() {
	// This function checks if every category has 1 selected entry that is available locally, and if so, enables the download button

	std::vector<int> dataInRows(DWDDescriptonData::NUM_D,-1);

	//find selected and local elements
	for ( unsigned int i = 0; i<m_descData.size(); ++i ) {	// iterate over stations
		for ( unsigned int j = 0; j<DWDDescriptonData::NUM_D; ++j ) {	// iterate over categories/types
			DWDDescriptonData &dwdData = m_descData[i];
			if (dwdData.m_data[j].m_isChecked && dwdData.m_data[j].m_isLocal) {
				return; // all fine, we have at least one valid data
			}
		}
	}

	m_enableClimateFileGeneration = false;
	updateGuiState(); //update ui
}

void DWDMainWindow::updateLocalFileList() {
	// iterate over local files and write to m_localFileList
	m_localFileList.clear();
	std::string prefix = "stundenwerte_";
	QDirIterator it(m_downloadDir.c_str(), QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	while (it.hasNext()) {
		QString localFilePath = it.next();
		std::string localFileName = localFilePath.toStdString().substr(m_downloadDir.str().size()+1);
		if (localFileName.substr(0,prefix.size()) == prefix) {
			m_localFileList.push_back(localFileName);
		}
	}

	// reset all flags in case the folder changed or files were deleted
	for(unsigned int i=0; i<m_descData.size(); ++i) {
		for(unsigned int j=0; j<=4; ++j) {
			m_descData[i].m_data[j].m_isLocal = false;
		}
	}

	for(unsigned int i=0; i<m_localFileList.size(); ++i) {
		try {
			unsigned int stationId = std::stoi(m_localFileList[i].substr(std::string("stundenwerte_FF_").length(),5));
			std::string type = m_localFileList[i].substr(std::string("stundenwerte_").length(),2);
			int typeId = -1;
			if (type == "TU") typeId = 0;
			if (type == "ST") typeId = 1;
			if (type == "FF") typeId = 2;
			if (type == "P0") typeId = 3;
			if (type == "RR") typeId = 4;

			for(unsigned int j=0; j<m_descData.size(); ++j) {
				if (m_descData[j].m_idStation == stationId) {
					m_descData[j].m_data[typeId].m_isLocal = true;
					break;
				}

			}
		} catch (IBK::Exception &ex) {continue;}
	}
	updateDownloadButton();
}

void DWDMainWindow::loadDataFromDWDServer(){

	//download all files
	DWDDescriptonData  descData;
	// get download links for data
	QStringList urls = descData.downloadDescriptionFiles(false);

	bool doDownload = false;
	for (QString &url : urls) {
		unsigned int idx = url.lastIndexOf("/") + 1;
		QString fileName = url.mid(idx);

		QString path = QString("%1/%2")
				.arg(QString::fromStdString(m_downloadDir.str()), fileName);
		QFile file(path);

		if (file.exists()) {
			QFileInfo info(file);
			QDateTime modifiedDate = info.lastModified();
			unsigned int days = modifiedDate.daysTo(QDateTime::currentDateTime());

			// update!!!
			if (days > CACHE_PERIOD) {
				doDownload = true;
				break;
			}
		}
		else {
			doDownload = true;
			break;
		}
	}

	if (doDownload) {
		progressDialog()->show();
		IBK::IBK_Message("Downloading weather location meta data from DWD Server.", IBK::MSG_PROGRESS);

		// initiate download manager
		m_manager = new DWDDownloader(this);
		m_manager->setFilepath(m_downloadDir);

		m_manager->m_urls = urls;
		m_manager->m_progressDlg = progressDialog(); // bisschen quatsch
		connect( m_manager, &DWDDownloader::finished, this, &DWDMainWindow::convertDwdData );

		m_manager->execute(); // simply registers network requests
	}
	else {
		IBK::IBK_Message("Using cached local meta data from DWD.", IBK::MSG_PROGRESS);
		convertDwdData();

	}

	// initialize lineEdit with userDataDir on StartUp
	m_ui->lineEditDownloads->setText(QString::fromStdString(m_downloadDir.str()));
}

void DWDMainWindow::alignLeftAxisQwtPlots() {
	// reformat the now initialized plots to align left axes
	int maxAxisWidth = std::max({
									m_ui->plotPres->axisWidget(QwtPlot::yLeft)->width(),
									m_ui->plotRain->axisWidget(QwtPlot::yLeft)->width(),
									m_ui->plotRelHum->axisWidget(QwtPlot::yLeft)->width(),
									m_ui->plotTemp->axisWidget(QwtPlot::yLeft)->width(),
									m_ui->plotWind->axisWidget(QwtPlot::yLeft)->width(),
									m_ui->plotRad->axisWidget(QwtPlot::yLeft)->width(),
									m_ui->plotRadLongWave->axisWidget(QwtPlot::yLeft)->width()
								});

	m_ui->plotPres->setContentsMargins(maxAxisWidth-m_ui->plotPres->axisWidget(QwtPlot::yLeft)->width(),0,0,0);
	m_ui->plotRain->setContentsMargins(maxAxisWidth-m_ui->plotRain->axisWidget(QwtPlot::yLeft)->width(),0,0,0);
	m_ui->plotRelHum->setContentsMargins(maxAxisWidth-m_ui->plotRelHum->axisWidget(QwtPlot::yLeft)->width(),0,0,0);
	m_ui->plotTemp->setContentsMargins(maxAxisWidth-m_ui->plotTemp->axisWidget(QwtPlot::yLeft)->width(),0,0,0);
	m_ui->plotWind->setContentsMargins(maxAxisWidth-m_ui->plotWind->axisWidget(QwtPlot::yLeft)->width(),0,0,0);
	m_ui->plotRad->setContentsMargins(maxAxisWidth-m_ui->plotRad->axisWidget(QwtPlot::yLeft)->width(),0,0,0);
	m_ui->plotRadLongWave->setContentsMargins(maxAxisWidth-m_ui->plotRadLongWave->axisWidget(QwtPlot::yLeft)->width(),0,0,0);
}

void DWDMainWindow::updateGuiState() {
	m_ui->tableView->setEnabled(m_guiState);
	m_ui->groupBoxLocation->setEnabled(m_guiState);
	m_ui->groupBoxTime->setEnabled(m_guiState);
	m_ui->groupBoxDir->setEnabled(m_guiState);
	m_ui->pushButtonPreview->setEnabled(m_guiState);
	m_ui->pushButtonMap->setEnabled(m_guiState);
	m_ui->pushButtonDownload->setEnabled(m_guiState && m_enableClimateFileGeneration);
}

bool DWDMainWindow::downloadAndConvertDwdData(bool showPreview, bool exportEPW) {
	FUNCID(MainWindow::downloadData);

	progressDialog()->show();
	qApp->processEvents();

	if ( exportEPW ) {
		if(!m_exportPath.isValid()) {
			progressDialog()->hide();
			QMessageBox::warning(this, "Select EPW file path.", "Please select a file path first for the export of the EPW - file.");
			return false;
		}
	}

	//check longitude and latitude
	//	if(m_ui->lineEditLatitude->text().isEmpty()){
	//		QMessageBox::critical(this, QString(), "Latitude is empty");
	//		setGUIState(true);
	//		return false;
	//	}
	//	if(m_ui->lineEditLongitude->text().isEmpty()){
	//		QMessageBox::critical(this, QString(), "Longitude is empty");
	//		setGUIState(true);
	//		return false;
	//	}

	m_ui->plotRelHum->setEnabled(false);
	m_ui->plotPres->setEnabled(false);
	m_ui->plotRain->setEnabled(false);
	m_ui->plotWind->setEnabled(false);
	m_ui->plotRad->setEnabled(false);
	m_ui->plotRadLongWave->setEnabled(false);
	m_ui->plotTemp->setEnabled(false);

	std::vector<int> dataInRows(DWDDescriptonData::NUM_D,-1);
	//find selected elements
	for ( unsigned int i = 0; i<m_descData.size(); ++i ) {	// iterate over stations
		for ( unsigned int j = 0; j<DWDDescriptonData::NUM_D; ++j ) {	// iterate over categories/types
			DWDDescriptonData &dwdData = m_descData[i];

			bool isChecked = dwdData.m_data[j].m_isChecked;

			if (isChecked) {
				dataInRows[j] = i;

				// Set current location name
				m_currentLocation[j] = QString::fromLatin1(dwdData.m_name.c_str());

				switch (j) {
				case DWDDescriptonData::D_TemperatureAndHumidity:
					m_ui->plotRelHum->setEnabled(isChecked);
					m_ui->plotTemp->setEnabled(isChecked);
					break;
				case DWDDescriptonData::D_Solar:
					m_ui->plotRad->setEnabled(isChecked);
					m_ui->plotRadLongWave->setEnabled(isChecked);
					break;
				case DWDDescriptonData::D_Pressure:
					m_ui->plotPres->setEnabled(isChecked);
					break;
				case DWDDescriptonData::D_Wind:
					m_ui->plotWind->setEnabled(isChecked);
					break;
				case DWDDescriptonData::D_Precipitation:
					m_ui->plotRain->setEnabled(isChecked);
					break;
				}
			}
		}
	}


	if(dataInRows == std::vector<int>(DWDDescriptonData::NUM_D,-1)) {
		progressDialog()->hide();
		QMessageBox::warning(this, "Download Error", tr("Please select at least one climate data entry in data table (e.g. temperature, radiation, ...)!"));

		// update ui
		m_guiState = true;
		updateGuiState();

		return false;
	}

	progressDialog()->setLabelText("Downloading DWD Data.");
	qApp->processEvents();

	std::vector<QString> filenames(DWDDescriptonData::NUM_D); //hold filenames for download
	std::vector<DWDData::DataType>	types{	DWDData::DT_AirTemperature, DWDData::DT_RadiationDiffuse,
				DWDData::DT_WindDirection, DWDData::DT_Pressure, DWDData::DT_Precipitation};

	// create downloadDir if necessary
	if (!QDir(m_downloadDir.c_str()).exists())
		QDir().mkdir(m_downloadDir.c_str());

	m_manager = new DWDDownloader(this);
	m_manager->setFilepath(m_downloadDir);
	m_manager->m_progressDlg = progressDialog();
	m_manager->m_urls.clear();

	//connect( m_manager, &DWDDownloader::finished, this, &MainWindow::convertDwdData );
	//download the data (zip)

	for(unsigned int i=0; i<DWDDescriptonData::NUM_D; ++i){
		if(dataInRows[i] != -1){	// if this column contains any selected entries

			// set auxiliary "type" string of current file to compare against local files
			std::string type = "";
			switch(i){
			case 0:	type = "TU"; break;
			case 1: type = "ST"; break;
			case 2:	type = "FF"; break;
			case 3:	type = "P0"; break;
			case 4:	type = "RR"; break;
			}

			DWDData dwdData;
			std::string dateString;

			int idx = dataInRows[i];

			unsigned int stationId = QString::number(m_descData[idx].m_idStation).rightJustified(5,'0').toUInt();
			// find our descData

			// test if requested file is present in localFileList
			std::string localFileName;
			std::string currentRequest = (QString::fromStdString(type + "_") + QString::number(stationId).rightJustified(5,'0')).toStdString();
			bool localFilePresent = false;
			for(unsigned int i=0; i<m_localFileList.size(); ++i){
				if (m_localFileList[i].substr(std::string("stundenwerte_").length(),std::string("FF_01234").length()) == currentRequest) {

					QFileInfo info(QString::fromStdString(m_localFileList[i]));
					QDateTime modifiedDate = info.lastModified();
					unsigned int days = modifiedDate.daysTo(QDateTime::currentDateTime());

					// update!!!
					if (days < CACHE_PERIOD) {
						localFilePresent = true;
						localFileName = m_localFileList[i];
						break;
					}
				}
			}
			if (localFilePresent) {
				filenames[i] = QString::fromStdString(localFileName).mid(0, localFileName.length()-4);
				IBK::IBK_Message("Found cached local file '" + localFileName +"'", IBK::MSG_PROGRESS);
			} else {

				dateString = "_" + m_descData[idx].m_startDateString + "_" + m_descData[idx].m_endDateString;

				bool isRecent = false;
				QString filename = ""; //only needed by historical

				//only find urls for historical and NO solar data (this is in dataInRows on position 1
				if(!isRecent){
					QFtp *ftp = new QFtp;

					connect(ftp, &QFtp::listInfo, &dwdData, &DWDData::findFile );

					ftp->connectToHost("opendata.dwd.de", 21);
					ftp->login("anonymous", "anonymous@opendata.dwd.de");
					ftp->cd("climate_environment");
					ftp->cd("CDC");
					ftp->cd("observations_germany");
					ftp->cd("climate");
					ftp->cd("hourly");


					switch(i){
					case 0:	ftp->cd("air_temperature"); break;
					case 2:	ftp->cd("wind"); break;
					case 3:	ftp->cd("pressure"); break;
					case 4:	ftp->cd("precipitation"); break;
					}
					ftp->cd("historical");
					ftp->list();

					while(ftp->hasPendingCommands() || ftp->currentCommand()!=QFtp::None)
						qApp->processEvents();

					//				while( dwdData.m_urls.empty() )
					//					qApp->processEvents();

					//now search through all urls for the right file
					for(unsigned int j=0; j<dwdData.m_urls.size(); ++j){
						filename = dwdData.m_urls[j].name();
						//stundenwerte_ST_00183_row.zip
						if((filename.midRef(QString("stundenwerte_ST_").length(), 5)).toUInt() == stationId){
							qDebug() << "Gefunden: " << filename;
							break;
						}
					}
					dwdData.m_urls.clear();
					qDebug() << "\nclear\n";
				}

				m_manager->m_urls << dwdData.urlFilename(types[i], QString::number(m_descData[idx].m_idStation).rightJustified(5,'0'), dateString, isRecent, filename);
				qDebug() << m_manager->m_urls.back();

				if (isRecent || i==1)
					filenames[i] = dwdData.filename(types[i], QString::number(m_descData[idx].m_idStation).rightJustified(5,'0'),dateString, isRecent);
				else
					filenames[i] = filename.mid(0, filename.length()-4);
			}
		}
	}

	IBK::IBK_Message("Start downloading Weather Data...", IBK::MSG_PROGRESS);

	try {
		if(!m_manager->m_urls.empty()) {
			m_manager->execute();

			while ( m_manager->m_isRunning ) {
				qApp->processEvents();
			}
		}
	}
	catch (IBK::Exception &ex) {
		for (unsigned int i = 0; i < filenames.size(); ++i) {
			std::remove(filenames[i].toStdString().c_str());
		}
		throw ex;
	}


	//Check if all downloaded files are valid
	//create a vector with valid files
	std::vector<IBK::Path>	validFiles(DWDDescriptonData::NUM_D);
	for(unsigned int i=0; i<DWDDescriptonData::NUM_D; ++i){
		if(dataInRows[i] == -1)
			continue;
		IBK::Path checkfile(m_downloadDir.str() + "/" + filenames[i].toStdString() + ".zip");

		if(!checkfile.exists()){
			QString cat;
			switch (types[i]) {
			case DWDData::DT_AirTemperature:	cat = "Temperature and relative Humidity";		break;
			case DWDData::DT_RadiationDiffuse:	cat = "Radiation"						;		break;
			case DWDData::DT_WindDirection:		cat = "Wind";									break;
			case DWDData::DT_Pressure:			cat = "Pressure";								break;
			case DWDData::DT_Precipitation:		cat = "Precipitation";							break;
			case DWDData::DT_RelativeHumidity:
			case DWDData::DT_RadiationLongWave:
			case DWDData::DT_RadiationGlobal:
			case DWDData::DT_ZenithAngle:
			case DWDData::DT_SunElevation:
			case DWDData::DT_WindSpeed:
			case DWDData::NUM_DT:
				break;
			}
			QMessageBox::warning(this, QString(), QString("Download of file '%1' was not successful. Category: '%2'").arg(filenames[i]+".zip").arg(cat));
			progressDialog()->hide();
			return false;
		}
		else
			validFiles[i] = checkfile;
	}

	//open the zip
	//find file with name 'produkt_....'
	//create the file path names and according data types for reading
	std::vector<IBK::Path>	checkedFileNames(DWDDescriptonData::NUM_D);
	std::map<IBK::Path, std::set<DWDData::DataType>> filenamesForReading;
	for (unsigned int i=0; i<filenames.size(); ++i) {
		if ( filenames[i].isEmpty() )
			continue;

		QStringList filesInArchive = JlCompress::getFileList(validFiles[i].str().c_str());
		QStringList filesExtracted;

		QString textFile;

		for ( QString fileName : filesInArchive ) {
			if ( "produkt" == fileName.mid(0,7) ){ // we find the right file
				// we found the file
				textFile = fileName;
				// we extract the file
				QString fileExtracted = JlCompress::extractFile( validFiles[i].str().c_str(), fileName, QString(m_downloadDir.c_str()) + "/extractedFiles/" + textFile);
				filesExtracted << fileExtracted;
				checkedFileNames[i] = IBK::Path(m_downloadDir.str() + "/extractedFiles/" + textFile.toStdString());
				// was the exraction successful
				if ( fileExtracted.isEmpty() )
					QMessageBox::warning(this, QString(), QString("File %1 could not be extracted").arg(textFile));
			}
		}
	}

	// there might be new local files after downloading
	updateLocalFileList();

	//create extract folder
	for(unsigned int i=0; i<DWDDescriptonData::NUM_D; ++i){
		if(checkedFileNames[i] == IBK::Path())
			continue; // skip empty states
		switch (i) {
		case DWDDescriptonData::D_TemperatureAndHumidity:
			filenamesForReading[checkedFileNames[i]] = std::set<DWDData::DataType>{DWDData::DT_AirTemperature, DWDData::DT_RelativeHumidity}; break;
		case DWDDescriptonData::D_Solar:
			filenamesForReading[checkedFileNames[i]] = std::set<DWDData::DataType>{DWDData::DT_RadiationDiffuse,DWDData::DT_RadiationGlobal, DWDData::DT_RadiationLongWave, DWDData::DT_ZenithAngle}; break;
		case DWDDescriptonData::D_Wind:
			filenamesForReading[checkedFileNames[i]] = std::set<DWDData::DataType>{DWDData::DT_WindDirection,DWDData::DT_WindSpeed}; break;
		case DWDDescriptonData::D_Pressure:
			filenamesForReading[checkedFileNames[i]] = std::set<DWDData::DataType>{DWDData::DT_Pressure}; break;
		case DWDDescriptonData::D_Precipitation:
			filenamesForReading[checkedFileNames[i]] = std::set<DWDData::DataType>{DWDData::DT_Precipitation}; break;
		}
	}

	ProgressNotify progressNotify(progressDialog());
	try {
		if (!exportEPW) {
			//read data
			m_dwdData.m_startTime = DWDConversions::convertQDate2IBKTime(m_ui->dateEditStart->date());
			m_dwdData.m_progressDlg = progressDialog();
			m_dwdData.createData(&progressNotify, filenamesForReading);
		}
	} catch(IBK::Exception &ex) {
		throw IBK::Exception(IBK::FormatString("%1\nCould not convert DWD Data.").arg(ex.what()), FUNC_ID);
	}

	if (m_dwdData.m_data.empty()) {
		progressDialog()->hide();
		throw IBK::Exception("Empty climate data. Cannot generate climate data for "
							 "selected time-period and location. May be due to mismatching "
							 "meta-data and measurement data from DWD server.", FUNC_ID);
	}

	//copy all data in range and create an epw
	double latiDeg = m_mapWidget->m_latitude;
	double longiDeg = m_mapWidget->m_longitude;

	///TODO take coordinates from radiation if exists --> zenith angle
	//check data
	for(unsigned int i=0; i<m_dwdData.m_data.size(); ++i){
		DWDData::IntervalData &intVal = m_dwdData.m_data[i];

		if( i == 0 ){
			if(intVal.m_airTemp < -50)
				intVal.m_airTemp = 0;
			if(intVal.m_relHum < 0 || intVal.m_relHum > 100)
				intVal.m_relHum = 50;
			if(intVal.m_counterRad < 0 || intVal.m_counterRad > 1000)
				intVal.m_counterRad = 280;
			if(intVal.m_windDirection < 0 || intVal.m_windDirection > 360)
				intVal.m_windDirection = 0;
			if(intVal.m_windSpeed< 0 || intVal.m_windSpeed> 20)
				intVal.m_windSpeed = 4;
			if(intVal.m_pressure < 0 || intVal.m_pressure > 2000)
				intVal.m_pressure = 1013;
			if(intVal.m_precipitation < 0 || intVal.m_precipitation > 2000)
				intVal.m_precipitation = 0;
		}
		else{
			//take air temperature of last timepoint
			if(intVal.m_airTemp < -50)
				intVal.m_airTemp = m_dwdData.m_data[i-1].m_airTemp;
			if(intVal.m_relHum < 0 || intVal.m_relHum > 100)
				intVal.m_relHum = m_dwdData.m_data[i-1].m_relHum;
			if(intVal.m_counterRad < 0 || intVal.m_counterRad > 1000)
				intVal.m_counterRad = m_dwdData.m_data[i-1].m_counterRad;
			if(intVal.m_windDirection < 0 || intVal.m_windDirection > 360)
				intVal.m_windDirection = m_dwdData.m_data[i-1].m_windDirection;
			if(intVal.m_windSpeed< 0 || intVal.m_windSpeed> 20)
				intVal.m_windSpeed = m_dwdData.m_data[i-1].m_windSpeed;
			if(intVal.m_pressure< 0 || intVal.m_pressure> 2000)
				intVal.m_pressure = m_dwdData.m_data[i-1].m_pressure;
			if(intVal.m_precipitation< 0 || intVal.m_precipitation> 2000)
				intVal.m_precipitation = m_dwdData.m_data[i-1].m_precipitation;
		}
		//only radiation
		if(i<=24){
			if(intVal.m_diffRad < 0 || intVal.m_diffRad > 1200)
				intVal.m_diffRad = 0;
			if(intVal.m_globalRad < 0 || intVal.m_globalRad > 1400)
				intVal.m_globalRad = intVal.m_diffRad;			//global must be diffuse radiation if normal radiation is zero
		}
		else{
			if(intVal.m_diffRad < 0 || intVal.m_diffRad > 1200)
				intVal.m_diffRad = m_dwdData.m_data[i-24].m_diffRad;
			if(intVal.m_globalRad < 0 || intVal.m_globalRad > 1400 || intVal.m_globalRad < intVal.m_diffRad)
				intVal.m_globalRad = std::max(m_dwdData.m_data[i-24].m_globalRad, intVal.m_diffRad);
		}
	}

	if ( showPreview ) {

		m_ui->plotPres->detachItems();
		m_ui->plotRain->detachItems();
		m_ui->plotRelHum->detachItems();
		m_ui->plotTemp->detachItems();
		m_ui->plotWind->detachItems();
		m_ui->plotRad->detachItems();
		m_ui->plotRadLongWave->detachItems();

		// create a new curve to be shown in the plot and set some properties
		QwtPlotCurve *curveTemp = new QwtPlotCurve();
		QwtPlotCurve *curveRelHum = new QwtPlotCurve();
		QwtPlotCurve *curveRad = new QwtPlotCurve();
		QwtPlotCurve *curveLongWaveRad = new QwtPlotCurve();
		QwtPlotCurve *curveRadDiff = new QwtPlotCurve();
		QwtPlotCurve *curveWind = new QwtPlotCurve();
		QwtPlotCurve *curvePressure = new QwtPlotCurve();
		QwtPlotCurve *curvePrecipitation = new QwtPlotCurve();

		QColor penColor = DWDDescriptonData::color(DWDDescriptonData::D_TemperatureAndHumidity);
		curveTemp->setPen( penColor, 1 ); // color and thickness in pixels
		curveTemp->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // use antialiasing

		penColor = QColor("#7F2AFF");
		curveRelHum->setPen( penColor, 1 ); // color and thickness in pixels
		curveRelHum->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // use antialiasing

		curveRad->setPen( DWDDescriptonData::color(DWDDescriptonData::D_Solar), 1 ); // color and thickness in pixels
		curveRad->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // use antialiasing

		curveLongWaveRad->setPen( QColor("#871ca4"), 1 ); // color and thickness in pixels
		curveLongWaveRad->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // use antialiasing

		curveRadDiff->setPen( DWDDescriptonData::color(DWDDescriptonData::D_Solar).darker(), 1 ); // color and thickness in pixels
		curveRadDiff->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // use antialiasing

		curveWind->setPen( DWDDescriptonData::color(DWDDescriptonData::D_Wind), 1 ); // color and thickness in pixels
		curveWind->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // use antialiasing

		curvePressure->setPen( DWDDescriptonData::color(DWDDescriptonData::D_Pressure), 1 ); // color and thickness in pixels
		curvePressure->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // use antialiasing

		curvePrecipitation->setPen( DWDDescriptonData::color(DWDDescriptonData::D_Precipitation), 1 ); // color and thickness in pixels
		curvePrecipitation->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // use antialiasing

		// data points
		QPolygonF pointsTemp, pointsRelHum, pointsRad, pointsRadDiff, pointsWind, pointsPressure, pointsPrecipitation, pointsLongWaveRadiation;

		QDate startDate(m_ui->dateEditStart->date());
		QDate endDate(m_ui->dateEditEnd->date());

		QDateTime start(startDate, QTime(0,0,0,0), Qt::UTC);
		QDateTime end(endDate, QTime(0,0,0,0), Qt::UTC);

		progressDialog()->setLabelText("Updating plot charts");
		// Updating plots
		for ( size_t i=0; i<m_dwdData.m_data.size(); ++i ) {
			size_t time = i*3600*1000;

			long long timeStep = (size_t)start.toMSecsSinceEpoch() /*+ (size_t)60*24*3600*1000*/ + time;
			// qDebug() << time << "\t" << timeStep;

			DWDData::IntervalData intVal = m_dwdData.m_data[i];

			pointsTemp << QPointF(timeStep , intVal.m_airTemp );
			pointsRelHum << QPointF(timeStep , intVal.m_relHum );
			double elevationAngle = 90.0 - intVal.m_zenithAngle;
			double radValue = (intVal.m_globalRad - intVal.m_diffRad) / std::sin(IBK::PI - elevationAngle*IBK::DEG2RAD);
			pointsRad << QPointF(timeStep, std::max(0.0, std::min(1200.0, radValue)));
			pointsRadDiff << QPointF(timeStep, intVal.m_diffRad );
			pointsLongWaveRadiation << QPointF(timeStep, intVal.m_counterRad );
			pointsWind << QPointF(timeStep, intVal.m_windSpeed );
			pointsPressure << QPointF(timeStep, intVal.m_pressure/1000  ); // in kPa
			pointsPrecipitation << QPointF(timeStep, intVal.m_precipitation );

			progressNotify.notify((double)(i+1)/m_dwdData.m_data.size() );
		}

		// give some points to the curve
		if (m_ui->plotTemp->isEnabled())
			curveTemp->setSamples(pointsTemp);
		if (m_ui->plotRelHum->isEnabled())
			curveRelHum->setSamples(pointsRelHum);
		if (m_ui->plotRad->isEnabled())
			curveRad->setSamples(pointsRad);
		if (m_ui->plotRad->isEnabled())
			curveRadDiff->setSamples(pointsRadDiff);
		if (m_ui->plotRadLongWave->isEnabled())
			curveLongWaveRad->setSamples(pointsLongWaveRadiation);
		if (m_ui->plotWind->isEnabled())
			curveWind->setSamples(pointsWind);
		if (m_ui->plotPres->isEnabled())
			curvePressure->setSamples(pointsPressure);
		if (m_ui->plotRain->isEnabled())
			curvePrecipitation->setSamples(pointsPrecipitation);

		// set the curve in the plot
		curveRelHum->attach(m_ui->plotRelHum);
		curveTemp->attach(m_ui->plotTemp);
		curveRad->attach( m_ui->plotRad );
		curveRadDiff->attach( m_ui->plotRad );
		curveLongWaveRad->attach( m_ui->plotRadLongWave );
		curvePressure->attach( m_ui->plotPres );
		curvePrecipitation->attach( m_ui->plotRain );
		curveWind->attach( m_ui->plotWind );

		m_ui->plotRelHum->replot();
		m_ui->plotPres->replot();
		m_ui->plotRain->replot();
		m_ui->plotRad->replot();
		m_ui->plotRadLongWave->replot();
		m_ui->plotTemp->replot();
		m_ui->plotWind->replot();

		m_ui->plotRelHum->show();
		m_ui->plotPres->show();
		m_ui->plotRain->show();
		m_ui->plotRad->show();
		m_ui->plotRadLongWave->show();
		m_ui->plotWind->show();
		m_ui->plotTemp->show();

		m_plotZoomerTemp->setZoomBase();
		m_plotZoomerLongWave->setZoomBase();
		m_plotZoomerPressure->setZoomBase();
		m_plotZoomerWind->setZoomBase();
		m_plotZoomerRain->setZoomBase();
		m_plotZoomerRelHum->setZoomBase();
		m_plotZoomerRad->setZoomBase();


	}

	m_validData = true;
	updateUi();

	if (exportEPW) {
		progressDialog()->hide();
		switch (m_mode) {

		case EM_EPW: {
			QDate start = m_ui->dateEditStart->date();
			QDate end = m_ui->dateEditStart->date();
			if (start.daysTo(end) > 365) {
				QMessageBox::warning(this, tr("Error in climate file creation"), tr("Only yearly data can be exported to an epw-file."));
				return false;
			}
			m_dwdData.exportEPW(m_ccm, latiDeg, longiDeg, m_exportPath);
		} break;
		case EM_C6B: m_dwdData.exportC6B(m_ccm, latiDeg, longiDeg, m_exportPath); break;
		case NUM_EM: break;
		}

		QMessageBox::information(this, QString("Climate file export"), tr("Climate file generation succesfully done.\n%1").arg(QString::fromStdString(m_exportPath.str())));
	}

	return true;
}



void DWDMainWindow::convertDwdData() {
	// read all decription files
	DWDDescriptonData descData;
	m_descData.clear();
	IBK::IBK_Message("Converting weather data.", IBK::MSG_PROGRESS);

	descData.readAllDescriptions(m_descData);
	DM::Scene *s = m_mapWidget->m_scene;
	// s->addDwdDataPoint(DM::Data::DT_TemperatureAndHumidity, QString(), IBK::Time(), IBK::Time(), 55.036579, 8.389285);


	try {
		// Convert data to our map dialog
		for(DWDDescriptonData &d : m_descData) {
			for(unsigned int i=0; i<DWDDescriptonData::NUM_D; ++i) {
				if(!d.m_data[i].m_isAvailable)
					continue;

				DM::Scene *s = m_mapWidget->m_scene;
				s->addDwdDataPoint(&m_mapWidget->m_distance, &m_dwdData.m_startTime, &m_dwdData.m_endTime,
								   (DM::Data::DataType)i, d.m_idStation, d.m_name.c_str(), d.m_minDate, d.m_maxDate,
								   d.m_latitude, d.m_longitude);
			}
		}
	}
	catch (IBK::Exception &ex) {
		IBK::IBK_Message("Error converting data.", IBK::MSG_ERROR);
	}

	calculateDistances();

	s->m_dataGroup[DM::Data::DT_Precipitation]->setZValue(1);
	s->m_dataGroup[DM::Data::DT_Pressure]->setZValue(3);
	s->m_dataGroup[DM::Data::DT_Solar]->setZValue(10);
	s->m_dataGroup[DM::Data::DT_TemperatureAndHumidity]->setZValue(5);
	s->m_dataGroup[DM::Data::DT_Wind]->setZValue(2);

	// we give the data to our table model
	m_dwdTableModel->m_descData = &m_descData;

	m_proxyModel->setSourceModel( m_dwdTableModel );

	m_ui->tableView->setModel(m_proxyModel); // we connect the table model to our view

	m_ui->tableView->setSortingEnabled(true);
	m_ui->tableView->sortByColumn(1, Qt::AscendingOrder);

	m_dwdTableModel->m_proxyModel = m_proxyModel;
	//m_ui->tableView->resizeColumnsToContents();

	m_ui->tableView->setItemDelegateForColumn(DWDTableModel::ColPressure, new DWDDelegate);
	m_ui->tableView->setItemDelegateForColumn(DWDTableModel::ColRadiation, new DWDDelegate);
	m_ui->tableView->setItemDelegateForColumn(DWDTableModel::ColAirTemp, new DWDDelegate);
	m_ui->tableView->setItemDelegateForColumn(DWDTableModel::ColPrecipitation, new DWDDelegate);
	m_ui->tableView->setItemDelegateForColumn(DWDTableModel::ColWind, new DWDDelegate);

	QHeaderView *headerView = m_ui->tableView->horizontalHeader();
//	headerView->setSectionResizeMode(DWDTableModel::ColPrecipitation, QHeaderView::Fixed);
//	headerView->setSectionResizeMode(DWDTableModel::ColRadiation, QHeaderView::Fixed);
//	headerView->setSectionResizeMode(DWDTableModel::ColRadiation, QHeaderView::Fixed);
//	headerView->setSectionResizeMode(DWDTableModel::ColWind, QHeaderView::Fixed);
//	headerView->setSectionResizeMode(DWDTableModel::ColPressure, QHeaderView::Fixed);

	m_ui->tableView->reset();

	double width = 100;
	m_ui->tableView->setColumnWidth(DWDTableModel::ColPressure, width);
	m_ui->tableView->setColumnWidth(DWDTableModel::ColAirTemp, width);
	m_ui->tableView->setColumnWidth(DWDTableModel::ColPrecipitation, width);
	m_ui->tableView->setColumnWidth(DWDTableModel::ColRadiation, width);
	m_ui->tableView->setColumnWidth(DWDTableModel::ColWind, width);

	double defWidth = 70;
	m_ui->tableView->setColumnWidth(DWDTableModel::ColId, defWidth);
	m_ui->tableView->setColumnWidth(DWDTableModel::ColDistance, defWidth);
	m_ui->tableView->setColumnWidth(DWDTableModel::ColLongitude, defWidth);
	m_ui->tableView->setColumnWidth(DWDTableModel::ColLatitude, defWidth);
	m_ui->tableView->setColumnWidth(DWDTableModel::ColHeight, defWidth);

	m_ui->tableView->setColumnWidth(DWDTableModel::ColMinDate, defWidth);
	m_ui->tableView->setColumnWidth(DWDTableModel::ColMaxDate, defWidth);

	m_ui->tableView->setColumnWidth(DWDTableModel::ColCountry, 80);
	m_ui->tableView->setColumnWidth(DWDTableModel::ColDistance, 100);

	headerView->setSectionResizeMode(DWDTableModel::ColName, QHeaderView::Stretch);

	// initialize local file list on startup
	// this can only be done after m_descData is filled, so the isLocal flags can be set correctly
	updateLocalFileList();
}

void DWDMainWindow::onActionSwitchLanguage() {
	QAction * a = (QAction *)sender();
	QString langId = a->data().toString();
	DWDSettings::instance().m_langId = langId;
	QMessageBox::information(this, tr("Languange changed"), tr("Please restart the software to activate the new language!"));
}

void DWDMainWindow::onUpdateDistances() {
	if (m_validData) {

		for (unsigned int i = 0; i < DWDDescriptonData::NUM_D; ++i)
			m_currentLocation[(DWDDescriptonData::DWDDataType)i] = "-";

		m_validData = false;
		updateUi();
		formatPlots(true);

		// Unckecks currently selected data
		m_dwdTableModel->uncheckData();
	}

	m_ui->lineEditLatitude->setText(QString("%1").arg(m_mapWidget->m_latitude, 0 ,'g', 3));
	m_ui->lineEditLongitude->setText(QString("%1").arg(m_mapWidget->m_longitude, 0 ,'g', 3));

	m_ui->horizontalSliderDistance->setValue(m_mapWidget->m_distance);

	calculateDistances();
}

void DWDMainWindow::onUpdatePlotZooming(const QRectF &rect) {
	QRectF rectOld = m_plotZoomerLongWave->zoomRect();
	QRectF newRect(QPointF(rect.bottomLeft().x(), rectOld.bottomLeft().y()),
				   QPointF(rect.topRight().x(),   rectOld.topRight().y()));

	m_plotZoomerLongWave->zoom(newRect);

	rectOld = m_plotZoomerPressure->zoomRect();
	newRect = QRectF(QPointF(rect.bottomLeft().x(), rectOld.bottomLeft().y()),
					 QPointF(rect.topRight().x(),   rectOld.topRight().y()));

	m_plotZoomerPressure->zoom(newRect);

	rectOld = m_plotZoomerRad->zoomRect();
	newRect = QRectF(QPointF(rect.bottomLeft().x(), rectOld.bottomLeft().y()),
					 QPointF(rect.topRight().x(),   rectOld.topRight().y()));

	m_plotZoomerRad->zoom(newRect);

	rectOld = m_plotZoomerRelHum->zoomRect();
	newRect = QRectF(QPointF(rect.bottomLeft().x(), rectOld.bottomLeft().y()),
					 QPointF(rect.topRight().x(),   rectOld.topRight().y()));

	m_plotZoomerRelHum->zoom(newRect);

	rectOld = m_plotZoomerRain->zoomRect();
	newRect = QRectF(QPointF(rect.bottomLeft().x(), rectOld.bottomLeft().y()),
					 QPointF(rect.topRight().x(),   rectOld.topRight().y()));

	m_plotZoomerRain->zoom(newRect);

	rectOld = m_plotZoomerTemp->zoomRect();
	newRect = QRectF(QPointF(rect.bottomLeft().x(), rectOld.bottomLeft().y()),
					 QPointF(rect.topRight().x(),   rectOld.topRight().y()));

	m_plotZoomerTemp->zoom(newRect);

	rectOld = m_plotZoomerWind->zoomRect();
	newRect = QRectF(QPointF(rect.bottomLeft().x(), rectOld.bottomLeft().y()),
					 QPointF(rect.topRight().x(),   rectOld.topRight().y()));

	m_plotZoomerWind->zoom(newRect);
}

void DWDMainWindow::onUpdateLocation(DWDDescriptonData::DWDDataType dataType, const QString &location) {
	switch (dataType) {
	case DWDDescriptonData::D_TemperatureAndHumidity:
		m_ui->labelTextTemperature->setText(location);
		break;
	case DWDDescriptonData::D_Solar:
		m_ui->labelTextRadiation->setText(location);
		break;
	case DWDDescriptonData::D_Wind:
		m_ui->labelTextWind->setText(location);
		break;
	case DWDDescriptonData::D_Pressure:
		m_ui->labelTextPressure->setText(location);
		break;
	case DWDDescriptonData::D_Precipitation:
		m_ui->labelTextPrecipitation->setText(location);
		break;
	case DWDDescriptonData::NUM_D:
		break;
	}
}

/// TODO
/// Herunterladen der Beschreibungsdateien
/// Lesen der Stationsbeschreibungsdateien
///
/* Stations_id von_datum bis_datum Stationshoehe geoBreite geoLaenge Stationsname Bundesland
----------- --------- --------- ------------- --------- --------- ----------------------------------------- ----------
012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
00003 19500401 20110331            202     50.7827    6.0941 Aachen                                   Nordrhein-Westfalen
00044 20070401 20210314             44     52.9336    8.2370 Großenkneten                             Niedersachsen
*/
/// sind für die ausgewählte datei daten vorhanden
/// Intervalfestlegung
/// Temperatur Feuchte
/// Strahlung
/// Wind, etc...
///
/// Daten runterladen
/// Fehlerprüfung
///		Fehlwerte korrigieren
/// fertigstellen für CCM (epw) oder ccd (nicht jahresscheiben)


void DWDMainWindow::on_pushButtonDownload_clicked(){

	try {
		QString extension;
		switch (m_mode) {
		case EM_EPW: extension = ".epw"; break;
		case EM_C6B: extension = ".c6b"; break;
		case NUM_EM: break;
		}

		// request file name
		QString filename = QFileDialog::getSaveFileName(
					this,
					tr("Save Climate File"),
					"",
					tr("Weather file (*%1);;All files (*.*)").arg(extension) );

		if (filename.isEmpty()) {
			QMessageBox::warning(this, tr("Export-Error"), tr("Please Select a valid file name for the exporting weather-data (%1).").arg(extension));
			return;
		}

		if (!filename.endsWith(extension))
			filename.append(extension);

		m_exportPath = IBK::Path(filename.toStdString());
		downloadAndConvertDwdData(true, true);
		formatPlots();
	}
	catch (IBK::Exception &ex) {
		progressDialog()->hide();

		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setText(tr("Could not download and save climate data.\nSee detailed errors."));
		messageBox.setDetailedText(QString("%1").arg(ex.what()));
		messageBox.exec();
	}
}

void DWDMainWindow::addToList(const QUrlInfo qUrlI){
	m_filelist << qUrlI.name();
}

void DWDMainWindow::calculateDistances() {

	double lat1 = m_mapWidget->m_latitude;
	double lon1 = m_mapWidget->m_longitude;

	// we calculate for each description the distance to the reference location
	for ( DWDDescriptonData &data : m_descData ) {

		// Latitude & Longitutde
		double lat2 = data.m_latitude;
		double lon2 = data.m_longitude;

		// Calculate distance between two coordinates
		// SOURCE: https://stackoverflow.com/questions/27928/calculate-distance-between-two-latitude-longitude-points-haversine-formula
		double a = 0.5 - cos((lat2-lat1)*IBK::DEG2RAD)/2 + cos(lat1*IBK::DEG2RAD) * cos(lat2*IBK::DEG2RAD) * (1-cos((lon2-lon1)*IBK::DEG2RAD))/2;

		// calc distance
		double dist = 12742 * std::asin(sqrt(a)); //2*R*asin
		data.m_distance = dist;

		for (unsigned int i=0; i<DWDDescriptonData::DWDDataType::NUM_D; ++i) {
			if (!data.m_data[i].m_isAvailable)
				continue;

			std::pair<DM::Data::DataType, unsigned int> key((DM::Data::DataType)i, data.m_idStation);

			DM::DataItem *dataItem = m_mapWidget->m_scene->m_idToDataItem.at(key);

			Q_ASSERT(dataItem != nullptr);
			const_cast<DM::Data &>(dataItem->data()).m_currentDistance = dist;
		}
	}

	double maxDistance = m_ui->lineEditDistance->text().toDouble();
	m_proxyModel->setFilterMaximumDistance(maxDistance);
	m_proxyModel->setFilterKeyColumn(1);

	// dwdTableModel->reset();
	m_ui->tableView->sortByColumn(DWDTableModel::ColDistance, Qt::AscendingOrder);
}

void DWDMainWindow::formatPlots(bool init) {

	if (init) {
		m_ui->plotTemp->detachItems();
		m_ui->plotPres->detachItems();
		m_ui->plotRad->detachItems();
		m_ui->plotRadLongWave->detachItems();
		m_ui->plotRain->detachItems();
		m_ui->plotWind->detachItems();
		m_ui->plotRelHum->detachItems();

		m_ui->plotTemp->setEnabled(false);
		m_ui->plotPres->setEnabled(false);
		m_ui->plotRad->setEnabled(false);
		m_ui->plotRadLongWave->setEnabled(false);
		m_ui->plotRain->setEnabled(false);
		m_ui->plotWind->setEnabled(false);
		m_ui->plotRelHum->setEnabled(false);
	}

	formatQwtPlot(init, *m_ui->plotTemp, m_ui->dateEditStart->date(), m_ui->dateEditEnd->date(), "Air Temperature", "C", -20, 40, 20, false);
	formatQwtPlot(init, *m_ui->plotPres, m_ui->dateEditStart->date(), m_ui->dateEditEnd->date(), "Pressure", "kPa", 0, 1.4, 0.2, false);
	formatQwtPlot(init, *m_ui->plotRad, m_ui->dateEditStart->date(), m_ui->dateEditEnd->date(), "Shortwave radiation", "W/m2", 0, 1400, 400, false);
	formatQwtPlot(init, *m_ui->plotRadLongWave, m_ui->dateEditStart->date(), m_ui->dateEditEnd->date(), "Longwave radiation", "W/m2", 0, 500, 100, false);
	formatQwtPlot(init, *m_ui->plotRain, m_ui->dateEditStart->date(), m_ui->dateEditEnd->date(), "Precipitation", "mm", 0, 50, 10, false);
	formatQwtPlot(init, *m_ui->plotWind, m_ui->dateEditStart->date(), m_ui->dateEditEnd->date(), "Wind speed", "m/s", 0, 40, 10, false);
	formatQwtPlot(init, *m_ui->plotRelHum, m_ui->dateEditStart->date(), m_ui->dateEditEnd->date(), "Relative Humidity", "%", 0, 100, 20, false);

	QString description;
	if (m_ui->plotTemp->isEnabled()) {
		description += "Temperatur: " + m_currentLocation[DWDDescriptonData::D_TemperatureAndHumidity] + " ";
		m_ui->plotTemp->setTitle(QString("%1 - %2")
								 .arg(m_currentLocation[DWDDescriptonData::D_TemperatureAndHumidity])
				.arg(m_ui->plotTemp->title().text()));
	}

	if (m_ui->plotRelHum->isEnabled()) {
		description += "Relative Humidity: " + m_currentLocation[DWDDescriptonData::D_TemperatureAndHumidity] + " ";
		m_ui->plotRelHum->setTitle(QString("%1 - %2")
								   .arg(m_currentLocation[DWDDescriptonData::D_TemperatureAndHumidity])
				.arg(m_ui->plotRelHum->title().text()));
	}

	if (m_ui->plotPres->isEnabled()) {
		description += "Pressure: " + m_currentLocation[DWDDescriptonData::D_Pressure] + " ";
		m_ui->plotPres->setTitle(QString("%1 - %2")
								 .arg(m_currentLocation[DWDDescriptonData::D_Pressure])
				.arg(m_ui->plotPres->title().text()));
	}

	if (m_ui->plotRad->isEnabled()) {
		description += "Short wave radiation: " + m_currentLocation[DWDDescriptonData::D_Solar] + " ";
		m_ui->plotRad->setTitle(QString("%1 - %2")
								.arg(m_currentLocation[DWDDescriptonData::D_Solar])
				.arg(m_ui->plotRad->title().text()));
	}

	if (m_ui->plotRadLongWave->isEnabled()) {
		description += "Long wave radiation: " + m_currentLocation[DWDDescriptonData::D_Solar] + " ";
		m_ui->plotRadLongWave->setTitle(QString("%1 - %2")
										.arg(m_currentLocation[DWDDescriptonData::D_Solar])
				.arg(m_ui->plotRadLongWave->title().text()));
	}

	if (m_ui->plotRain->isEnabled()) {
		description += "Precipitation: " + m_currentLocation[DWDDescriptonData::D_Precipitation] + " ";
		m_ui->plotRain->setTitle(QString("%1 - %2")
								 .arg(m_currentLocation[DWDDescriptonData::D_Precipitation])
				.arg(m_ui->plotRain->title().text()));
	}

	if (m_ui->plotWind->isEnabled()) {
		description += "Wind: " + m_currentLocation[DWDDescriptonData::D_Wind] + " ";
		m_ui->plotWind->setTitle(QString("%1 - %2")
								 .arg(m_currentLocation[DWDDescriptonData::D_Wind])
				.arg(m_ui->plotWind->title().text()));
	}

	m_ui->plotTemp->setHidden(!m_ui->plotTemp->isEnabled());
	m_ui->plotRelHum->setHidden(!m_ui->plotRelHum->isEnabled());
	m_ui->plotPres->setHidden(!m_ui->plotPres->isEnabled());
	m_ui->plotRad->setHidden(!m_ui->plotRad->isEnabled());
	m_ui->plotRadLongWave->setHidden(!m_ui->plotRadLongWave->isEnabled());
	m_ui->plotWind->setHidden(!m_ui->plotWind->isEnabled());
	m_ui->plotRain->setHidden(!m_ui->plotRain->isEnabled());

	// ToDo Completly improve this!!!!
	m_checkBox[PT_Temperature]->setChecked(m_ui->plotTemp->isEnabled());
	m_checkBox[PT_RelativeHumidity]->setChecked(m_ui->plotRelHum->isEnabled());
	m_checkBox[PT_LongWaveRadiation]->setChecked(m_ui->plotRadLongWave->isEnabled());
	m_checkBox[PT_ShortWaveRadiation]->setChecked(m_ui->plotRad->isEnabled());
	m_checkBox[PT_WindSpeed]->setChecked(m_ui->plotWind->isEnabled());
	m_checkBox[PT_Pressure]->setChecked(m_ui->plotPres->isEnabled());
	m_checkBox[PT_Precipitation]->setChecked(m_ui->plotRain->isEnabled());

	m_metaDataWidget->m_ccm->m_comment = description.toStdString();
	m_metaDataWidget->updateUi();
}

void DWDMainWindow::formatQwtPlot(bool init, QwtPlot &plot, QDate startDate, QDate endDate, QString title, QString leftYAxisTitle, double yLeftMin, double yLeftMax, double yLeftStepSize,
								  bool hasRightAxis, QString rightYAxisTitle, double yRightMin, double yRightMax, double yRightStepSize) {


	// initialize start and end date
	QDateTime start(startDate, QTime(0,0,0,0), Qt::UTC);
	QDateTime end(endDate, QTime(0,0,0,0), Qt::UTC);

	// we set also the time spec
	start.setTimeSpec(Qt::UTC);
	end.setTimeSpec(Qt::UTC);

	// initialize all major ticks in grid and Axis
	QList<double> majorTicks;
	majorTicks.push_back(start.toMSecsSinceEpoch() );

	unsigned int days = start.daysTo(end);

	// assume an average month has 30 days
	unsigned int months = days / 30;

	for(unsigned int i=1; i<months/2; ++i)
		majorTicks.push_back(QwtDate::toDouble(start.addMonths(2*i) ) );

	// Init Scale Divider
	QwtScaleDiv *scaleDiv = new QwtScaleDiv(QwtDate::toDouble(start), QwtDate::toDouble(end), QList<double>(), QList<double>(), majorTicks);

	// inti plot title
	QFont font;
	font.setPointSize(7);
	QwtText qwtTitle;
	qwtTitle.setFont(font);
	qwtTitle.setText(title);

	// Set plot title
	plot.setTitle(qwtTitle);

	// Scale all y axises
	plot.setAxisScale(QwtPlot::yLeft, yLeftMin, yLeftMax, yLeftStepSize);
	plot.setAxisFont(QwtPlot::yLeft, font);

	// Init QWT Text
	QwtText axisTitle;
	axisTitle.setFont(font);

	// left Axis title
	axisTitle.setText(leftYAxisTitle);
	plot.setAxisTitle(QwtPlot::yLeft, axisTitle);

	// do we have a right y axis?
	if(hasRightAxis) {
		plot.setAxisFont(QwtPlot::yRight, font);
		plot.enableAxis(QwtPlot::yRight, true);
		plot.setAxisScale(QwtPlot::yRight, yRightMin, yRightMax, yRightStepSize);

		// right Axis title
		axisTitle.setText(rightYAxisTitle);
		plot.setAxisTitle(QwtPlot::yRight, axisTitle);
		plot.setTitle(title);
	}

	// Bottom axis
	plot.setAxisFont(QwtPlot::xBottom, font);

	// Canvas Background
	plot.setCanvasBackground(Qt::white);

	// Init Scale draw engine
	QwtDateScaleDraw *scaleDrawTemp = new QwtDateScaleDraw(Qt::UTC);
	scaleDrawTemp->setDateFormat(QwtDate::Year, "yyyy");
	scaleDrawTemp->setDateFormat(QwtDate::Month, "MMM yy");
	scaleDrawTemp->setDateFormat(QwtDate::Week, "dd.MM.");
	scaleDrawTemp->setDateFormat(QwtDate::Day, "dd.MM.");
	scaleDrawTemp->setDateFormat(QwtDate::Hour, "hh:mm\ndd.MM.");
	scaleDrawTemp->setDateFormat(QwtDate::Minute, "hh:mm");
	scaleDrawTemp->setDateFormat(QwtDate::Second, "hh:mm:ss");
	scaleDrawTemp->setDateFormat(QwtDate::Millisecond, "hh:mm:ss");

	QwtDateScaleEngine *scaleEngine = new QwtDateScaleEngine(Qt::UTC);

	plot.enableAxis(QwtPlot::xBottom, !init && plot.isEnabled());
	// Set scale draw engine
	plot.setAxisScaleDraw(QwtPlot::xBottom, scaleDrawTemp);
	plot.setAxisScaleEngine(QwtPlot::xBottom, scaleEngine);
	//plot.setMinimumWidth(350);


	// Init Grid
	QwtPlotGrid *grid = new QwtPlotGrid;
	grid->setXDiv(*scaleDiv);
	grid->enableXMin(true);
	grid->enableYMin(true);
	grid->enableX(true);
	grid->enableY(true);
	grid->setVisible(true);
	grid->setMajorPen(QPen(Qt::gray, 0, Qt::DotLine));
	grid->setMinorPen(QPen(Qt::lightGray, 0 , Qt::DotLine));
	grid->attach(&plot);

	plot.replot();
}

void DWDMainWindow::addLanguageAction(const QString &langId, const QString &actionCaption) {
	FUNCID(DWDMainWindow::addLanguageAction);
	QString languageFilename = QtExt::Directories::translationsFilePath(langId);
	if (langId == "en" || QFile(languageFilename).exists()) {
		QAction * a = new QAction(actionCaption, this);
		a->setData(langId);
		a->setIcon( QIcon( QString(":/gfx/languages/%1.png").arg(langId)) );
		a->setIconVisibleInMenu(true);
		connect(a, SIGNAL(triggered()),
				this, SLOT(onActionSwitchLanguage()));
		m_ui->menuLanguage->insertAction(nullptr, a);
	}
	else {
		IBK::IBK_Message( IBK::FormatString("Language file '%1' missing.").arg(languageFilename.toStdString()),
						  IBK::MSG_WARNING, FUNC_ID);
	}
}

void DWDMainWindow::resizeEvent(QResizeEvent *event) {
	m_ui->graphicsViewMap->fitInView(m_mapWidget->m_scene->sceneRect(), Qt::KeepAspectRatio);
}


void DWDMainWindow::on_pushButtonMap_clicked() {
	double latitude = m_mapWidget->m_latitude;
	double longitude = m_mapWidget->m_longitude;
	unsigned int distance = m_ui->horizontalSliderDistance->value();

	//	unsigned int year = 2020;
	//DWDMap::getLocation(m_descData, latitude, longitude, year, distance, this);
	m_mapWidget->setCoordinates(latitude, longitude);
	m_mapWidget->setDistance(distance);
	// m_mapDialog->showMaximized();

	QRectF rect = m_mapWidget->m_scene->m_mapSvgItem->boundingRect();
	QPointF pos = DM::convertCoordinatesToPos(rect, latitude, longitude);

	m_mapWidget->m_scene->m_locationItem->setPos(pos.x(), pos.y());

	//	m_ui->lineEditLatitude->setText(QString::number(m_mapWidget->m_latitude) );
	//	m_ui->lineEditLongitude->setText(QString::number(m_mapWidget->m_longitude) );
	m_ui->horizontalSliderDistance->setValue(m_mapWidget->m_distance);

	m_ccm.m_latitudeInDegree = m_mapWidget->m_latitude;
	m_ccm.m_longitudeInDegree = m_mapWidget->m_longitude;

	m_ui->widgetMetaData->updateUi();

	calculateDistances();

	//	QModelIndex top = m_dwdTableModel->index(0, 2);
	//	QModelIndex bottom = m_dwdTableModel->index(m_descData.size(), 2);
	//	emit m_dwdTableModel->dataChanged(top, bottom); // we just update the whole column

	m_dwdTableModel->reset();

	// Unckecks currently selected data
	m_dwdTableModel->uncheckData();

	m_ui->tabWidget->setCurrentIndex(T_Location);
}


void DWDMainWindow::setProgress(int min, int max, int val) {
	//	FUNCID(setProgress);

	progressDialog()->setMaximum(val);

	m_dwdTableModel->reset();
}

void DWDMainWindow::on_radioButtonHistorical_toggled(bool /*checked*/) {
	loadDataFromDWDServer();

	m_ui->tableView->reset();
}


void DWDMainWindow::on_lineEditNameFilter_textChanged(const QString &filter) {
	m_proxyModel->setFilterRegExp(filter);
	m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	m_proxyModel->setFilterKeyColumn(4);
}


void DWDMainWindow::on_pushButtonPreview_clicked() {
	FUNCID(DWDMainWindow::on_pushButtonPreview_clicked);

	try {
		// Try to download and convert data
		bool successful = downloadAndConvertDwdData(true, false);

		// update formatting
		if (!successful)
			return; // When no data has been selected by user such as temperature, rel. Humidity, ...
		// Info MessageBox is then thrown

		// update plots;
		formatPlots();

		// Show plot charts
		m_ui->tabWidget->setCurrentIndex(T_Plots);

	}
	catch (IBK::Exception &ex) {
		progressDialog()->hide();

		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setText(tr("Could not download and convert climate data. See detailed errors."));
		messageBox.setDetailedText(QString("%1").arg(ex.what()));
		messageBox.exec();

		updateGuiState();
		return;
	}
}


void DWDMainWindow::on_toolButtonDownloadDir_clicked() {
	// request directory
	QString directory = QFileDialog::getExistingDirectory (
				this,
				tr("Set Downloads Directory"),
				m_downloadDir.c_str(),
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (directory.isEmpty()) return;

	m_downloadDir = IBK::Path(directory.toStdString());

	m_ui->lineEditDownloads->setText(directory);

	updateLocalFileList();
}


void DWDMainWindow::on_comboBoxMode_currentIndexChanged(int index) {
	m_mode = (ExportMode)m_ui->comboBoxMode->currentData().toUInt();

	m_ui->dateEditEnd->setEnabled(m_mode == ExportMode::EM_C6B);

	switch (m_mode) {

	case EM_EPW:
		// In EPW only yearly data can be produced
		m_ui->dateEditEnd->setDate(m_ui->dateEditStart->date().addMonths(12));
		break;
	case EM_C6B:

		break;
	case NUM_EM:
		break;
	}
}

void DWDMainWindow::on_dateEditStart_dateChanged(const QDate &startDate) {
	if (m_validData) {
		QMessageBox::Button btn = QMessageBox::question(this, tr("Data reset"),
														tr("Downloaded DWD data will be reseted.\n"
														   "Do you want to continue?"));

		if (btn == QMessageBox::No) {
			updateUi(); // reset to old data
			return;
		}
	}

	m_ccm.m_startYear = startDate.year();
	m_ui->widgetMetaData->updateUi();

	m_dwdData.m_startTime = DWDConversions::convertQDate2IBKTime(startDate);
	QDate endDate(DWDConversions::convertIBKTime2QDate(m_dwdData.m_endTime));

	if (m_mode == ExportMode::EM_EPW) {
		endDate = startDate.addMonths(12);
	}
	else if (startDate > endDate) {
		endDate = startDate.addDays(1);
		QMessageBox::warning(this, tr("Error in start date"), tr("Period should be at least one day."));
	}

	m_dwdData.m_endTime = DWDConversions::convertQDate2IBKTime(endDate);

	for (unsigned int i = 0; i < DWDDescriptonData::NUM_D; ++i)
		m_currentLocation[(DWDDescriptonData::DWDDataType)i] = "-";

	m_validData = false;
	updateUi();
	formatPlots(true);

	// Unckecks currently selected data
	m_dwdTableModel->uncheckData();
}


void DWDMainWindow::on_dateEditEnd_dateChanged(const QDate &enddate) {
	if (m_validData) {
		QMessageBox::Button btn = QMessageBox::question(this, tr("Data reset"),
														tr("Downloaded DWD data will be reseted.\n"
														   "Do you want to continue?"));

		if (btn == QMessageBox::No) {
			updateUi(); // reset to old data
			return;
		}
	}

	m_validData = false;

	m_dwdData.m_endTime.set(enddate.year(), enddate.month()-1, enddate.day()-1, 0 );
	QDate startDate(DWDConversions::convertIBKTime2QDate(m_dwdData.m_startTime));

	if (startDate > enddate)
		m_dwdData.m_startTime = DWDConversions::convertQDate2IBKTime(enddate.addDays(-1));

	updateUi();

	for (unsigned int i = 0; i < DWDDescriptonData::NUM_D; ++i)
		m_currentLocation[(DWDDescriptonData::DWDDataType)i] = "-";

	formatPlots(true);

	// Unckecks currently selected data
	m_dwdTableModel->uncheckData();
}


void DWDMainWindow::on_toolButtonHelp_clicked() {
	QMessageBox::information(this, tr("Climate data modes"), tr("In EPW Mode it is only possible to produce yearly data and exported "
																"weather files. So only a start date can be specified. Whereas in C6B "
																"Mode continuous weather data can be produced and exported in a weather file."));
}

void DWDMainWindow::updateUi() {

	m_ui->pushButtonDownload->setEnabled(m_validData);
	alignLeftAxisQwtPlots();

	m_ui->dateEditStart->blockSignals(true);
	m_ui->dateEditEnd->blockSignals(true);

	m_ui->dateEditStart->setDate(DWDConversions::convertIBKTime2QDate(m_dwdData.m_startTime));
	m_ui->dateEditEnd->setDate(DWDConversions::convertIBKTime2QDate(m_dwdData.m_endTime));

	m_proxyModel->setFilterMinimumDate(DWDConversions::convertIBKTime2QDate(m_dwdData.m_startTime));
	m_proxyModel->setFilterMaximumDate(DWDConversions::convertIBKTime2QDate(m_dwdData.m_endTime));

	m_ui->dateEditStart->blockSignals(false);
	m_ui->dateEditEnd->blockSignals(false);

}

void DWDMainWindow::generateCheckBox(const QString &str, const QwtPlot *plot, const QColor &color, PlotType pt) {
	// Connect the checkbox's stateChanged signal to a lambda function
	QHBoxLayout *layout = m_ui->horizontalLayoutCheckBoxes;

	QCheckBox *cb = new QCheckBox();
	cb->setChecked(true);
	cb->setText(str);
	cb->setStyleSheet(QString("QCheckBox {Color: %1;}").arg(color.name()));

	layout->addWidget(cb);

	// Connect the checkbox's stateChanged signal to a lambda function
	connect(cb, &QCheckBox::toggled, [plot](bool checked){
		const_cast<QwtPlot*>(plot)->setHidden(!checked);
	});

	m_checkBox[pt] = cb;
}


QProgressDialog *DWDMainWindow::progressDialog() {
	if(m_progressDlg == nullptr) {
		m_progressDlg = new QProgressDialog(this);
		m_progressDlg->setMinimumWidth(800);
		m_progressDlg->setModal(true);
		m_progressDlg->setValue(0);
		m_progressDlg->setMaximum(0);
		m_progressDlg->hide();
	}

	return m_progressDlg;
}

void DWDMainWindow::closeEvent(QCloseEvent *event) {
	// save user config and recent file list
	DWDSettings::instance().write(saveGeometry(), saveState());
	event->accept();
}


void DWDMainWindow::on_actionAbout_triggered() {
	// ToDo Dialog
	if (m_aboutDialog == nullptr)
		m_aboutDialog = new DWDAboutDialog(this);

	m_aboutDialog->exec();
}


void DWDMainWindow::on_actionAbout_Qt_triggered() {
	QMessageBox::aboutQt(this, tr("About Qt..."));
}


void DWDMainWindow::on_actionClose_triggered() {
	close();
}


void DWDMainWindow::on_actionc6b_triggered() {
	m_ui->comboBoxMode->setCurrentIndex(EM_C6B);
}


void DWDMainWindow::on_actionEPW_triggered() {
	m_ui->comboBoxMode->setCurrentIndex(EM_EPW);
}


void DWDMainWindow::on_horizontalSliderDistance_valueChanged(int value) {
	m_ui->lineEditDistance->setText(QString::number(value) );
	m_mapWidget->setDistance(value);

	m_proxyModel->setFilterMaximumDistance(value);
	m_proxyModel->setFilterKeyColumn(1);
}


void DWDMainWindow::on_tabWidget_currentChanged(int index) {
	m_ui->graphicsViewMap->fitInView(m_mapWidget->m_scene->sceneRect(), Qt::KeepAspectRatio);
//	if (index == T_Plots)
//		m_ui->plotLayoutPreview->setSizeConstraint(QLayout::SetDefaultConstraint);
}


void DWDMainWindow::on_checkBoxPres_toggled(bool checked) {
	m_ui->plotPres->setHidden(!checked);
}


void DWDMainWindow::on_actiondark_triggered() {

	QFile styleDark(":/style/style.qss");
	styleDark.open(QFile::ReadOnly);
	qApp->setStyleSheet(QLatin1String(styleDark.readAll()));

}


void DWDMainWindow::on_actionwhite_triggered() {
	qApp->setStyleSheet("");
}


void DWDMainWindow::on_actionSaveAs_triggered() {
	if (!m_validData) {
		QMessageBox::information(this, tr("Save climate file"), tr("Please download climate data first in order to save it to a climate file."));
		return;
	}

	on_pushButtonDownload_clicked();
}


void DWDMainWindow::on_actionShowLogWidget_triggered(bool showLog) {
	m_logWidget->setHidden(!showLog);
}

