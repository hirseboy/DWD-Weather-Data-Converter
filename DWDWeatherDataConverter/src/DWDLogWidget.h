#ifndef DWDLOGWIDGET_H
#define DWDLOGWIDGET_H

#include <QDockWidget>
#include <QPlainTextEdit>

namespace Ui {
class DWDLogWidget;
}

class DWDLogWidget : public QDockWidget
{
    Q_OBJECT

public:
	explicit DWDLogWidget(QWidget *parent = nullptr);
	~DWDLogWidget();

public slots:
	/*! When a message from IBK::Message gets received. */
	void onMsgReceived(int type, QString msg);


private:
	Ui::DWDLogWidget	*m_ui;

	QPlainTextEdit		*m_textEdit;
};

#endif // DWDLOGWIDGET_H
