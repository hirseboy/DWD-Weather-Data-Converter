#include "DWDAboutDialog.h"
#include "ui_DWDAboutDialog.h"

DWDAboutDialog::DWDAboutDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DWDAboutDialog)
{
	m_ui->setupUi(this);
	m_ui->label->setPixmap(QPixmap(":/splashscreen/DWDWeatherDataConverterSplash.png").scaledToHeight(500, Qt::SmoothTransformation));
}

DWDAboutDialog::~DWDAboutDialog() {
	delete m_ui;
}
