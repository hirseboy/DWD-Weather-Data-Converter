#include "DWDLogWidget.h"
#include "ui_DWDLogWidget.h"

#include <QTime>
#include <QScrollBar>

#include <IBK_messages.h>

DWDLogWidget::DWDLogWidget(QWidget *parent) :
    QDockWidget(parent),
	m_ui(new Ui::DWDLogWidget)
{
	m_ui->setupUi(this);
	m_textEdit = m_ui->plainTextEdit;
}

DWDLogWidget::~DWDLogWidget()
{
	delete m_ui;
}

void DWDLogWidget::onMsgReceived(int type, QString msgString) {
	// avoid empty lines between messages
	if (msgString.endsWith('\n'))
		msgString.chop(1);
	if (msgString.endsWith("\r\n"))
		msgString.chop(2);

	// Define Colors
	QColor logProgressText							= "#c0c0c0";
	QColor logErrorText								= "#ff2222";
	QColor logWarningText							= "#f0dc00";
	QColor logDebugText								= "#39b1d9";

	QString html = QString("<span style=\"white-s	pace:pre; color:%3\">[%2] %1</span>").arg(msgString).arg(QDateTime::currentDateTime().toString());
	// color lines according to message type
	switch (type) {
		case IBK::MSG_PROGRESS :
		case IBK::MSG_CONTINUED :
			html = html.arg(logProgressText.name());
			break;
		case IBK::MSG_WARNING :
			html = html.arg(logWarningText.name());
			break;
		case IBK::MSG_ERROR :
			html = html.arg(logErrorText.name());
			break;
		case IBK::MSG_DEBUG :
			html = html.arg(logDebugText.name());
			break;
	}

	// get position of scroll bar
	int pos = m_textEdit->verticalScrollBar()->value();
	int maxpos = m_textEdit->verticalScrollBar()->maximum();
	m_textEdit->appendHtml(html);
	// if we were at the end, go again to the end
	if (pos == maxpos)
		m_textEdit->verticalScrollBar()->setValue( m_textEdit->verticalScrollBar()->maximum() );
}
