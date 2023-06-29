#include "MetaDataEditWidget.h"
#include "ui_MetaDataEditWidget.h"

/*! Utility function for conversion of std::string holding an Utf8 encoded string to QString. */
inline QString su( const std::string & target ) {
	return QString::fromUtf8( target.c_str() );
}

MetaDataEditWidget::MetaDataEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MetaDataEditWidget)
{
	ui->setupUi(this);
	layout()->setMargin(0);
	// setup edit table
	ui->tableWidgetProperties->setRowCount(9);
	ui->tableWidgetProperties->setColumnCount(1);
	ui->tableWidgetProperties->horizontalHeader()->setVisible(false);
	QStringList labels;
	labels << tr("City") << tr("Country") << tr("WMOCode") << tr("Source")
		   << tr("Elevation [m]") << tr("Longitude [Deg]")
		   << tr("Latitude [Deg]") << tr("Time zone") << tr("Start year");
	ui->tableWidgetProperties->setVerticalHeaderLabels(labels);
	ui->tableWidgetProperties->horizontalHeader()->setStretchLastSection(true);
	ui->tableWidgetProperties->verticalHeader()->setDefaultSectionSize(ui->tableWidgetProperties->verticalHeader()->minimumSectionSize());
	QFont f = font();
#ifdef Q_OS_MAC
	f.setPointSize(10);
#else
	f.setPointSize(9);
#endif
	setFont(f);
}


MetaDataEditWidget::~MetaDataEditWidget() {
	delete ui;
}


void MetaDataEditWidget::updateUi() {
	ui->tableWidgetProperties->blockSignals(true);
	ui->plainTextEdit->blockSignals(true);
	// transfer data to table widget
	QTableWidgetItem * item = new QTableWidgetItem(su(m_ccm->m_city));
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	ui->tableWidgetProperties->setItem(0,0, item);

	item = new QTableWidgetItem(su(m_ccm->m_country));
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	ui->tableWidgetProperties->setItem(1,0, item);

	item = new QTableWidgetItem(su(m_ccm->m_wmoCode));
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	ui->tableWidgetProperties->setItem(2,0, item);

	item = new QTableWidgetItem(su(m_ccm->m_source));
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	ui->tableWidgetProperties->setItem(3,0, item);

	item = new QTableWidgetItem(QString("%L1").arg(m_ccm->m_elevation));
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	ui->tableWidgetProperties->setItem(4,0, item);

	item = new QTableWidgetItem(QString("%L1").arg(m_ccm->m_longitudeInDegree));
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
	ui->tableWidgetProperties->setItem(5,0, item);

	item = new QTableWidgetItem(QString("%L1").arg(m_ccm->m_latitudeInDegree));
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	ui->tableWidgetProperties->setItem(6,0, item);

	QString timeZone = QString("%1").arg(m_ccm->m_timeZone);
	if (m_ccm->m_timeZone > 0)
		timeZone = "+" + timeZone;
	item = new QTableWidgetItem(timeZone);
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	ui->tableWidgetProperties->setItem(7,0, item);

	item = new QTableWidgetItem(QString("%1").arg(m_ccm->m_startYear));
	if (m_ccm->m_dataTimePoints.empty()) {
		item->setFlags( 0 );
		item->setTextColor(Qt::gray);
	}
	else {
		item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	}
	ui->tableWidgetProperties->setItem(8,0, item);


	ui->tableWidgetProperties->resizeRowsToContents();
	int lineHeight = ui->tableWidgetProperties->verticalHeader()->sectionSize(0);
	ui->tableWidgetProperties->setMaximumHeight(lineHeight*ui->tableWidgetProperties->rowCount()+2);
	ui->plainTextEdit->setPlainText( su(m_ccm->m_comment) );
	ui->plainTextEdit->blockSignals(false);
	ui->tableWidgetProperties->blockSignals(false);
}


void MetaDataEditWidget::on_tableWidgetProperties_cellChanged(int row, int column) {
	QTableWidgetItem * item = ui->tableWidgetProperties->item(row, column);
	if (row > 3 && row < 7) {
		double val;
		bool ok;
		QString valText = item->data(Qt::DisplayRole).toString();
		val = QLocale().toDouble( valText, &ok);
		if (!ok)
			val = valText.toDouble(&ok);
		if (ok) {
			switch (row) {
				case 4: m_ccm->m_elevation = val; break;
				case 5: m_ccm->m_longitudeInDegree = val; break;
				case 6: m_ccm->m_latitudeInDegree = val; break;
			}
		}
	}
	else if (row == 7) {
		int val;
		bool ok;
		val = QLocale().toInt( item->data(Qt::DisplayRole).toString(), &ok);
		if (ok && val >= -12 && val <= 12) {
			m_ccm->m_timeZone = val;
		}
	}
	else if (row == 8) {
		int val;
		bool ok;
		val = QLocale().toInt( item->data(Qt::DisplayRole).toString(), &ok);
		if (ok) {
			m_ccm->m_startYear = val;
			emit startYearChanged(val);
		}
	}
	else {
		switch (row) {
			case 0 : m_ccm->m_city = item->text().toUtf8().data(); break;
			case 1 : m_ccm->m_country = item->text().toUtf8().data(); break;
			case 2 : m_ccm->m_wmoCode = item->text().toUtf8().data(); break;
			case 3 : m_ccm->m_source = item->text().toUtf8().data(); break;
			default:;
		}
	}
}


void MetaDataEditWidget::on_plainTextEdit_textChanged() {
	m_ccm->m_comment = ui->plainTextEdit->toPlainText().toUtf8().data();
}
