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

protected:
	void resizeEvent(QResizeEvent *event);

public slots:
	/*! When a message from IBK::Message gets received. */
	void onMsgReceived(int type, QString msg);

signals:
	void resized();

private:
	Ui::DWDLogWidget	*m_ui;

	QPlainTextEdit		*m_textEdit;

};

#endif // DWDLOGWIDGET_H
