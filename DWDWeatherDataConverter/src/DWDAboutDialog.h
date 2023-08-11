#ifndef DWDABOUTDIALOG_H
#define DWDABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class DWDAboutDialog;
}

class DWDAboutDialog : public QDialog {
	Q_OBJECT

public:
	explicit DWDAboutDialog(QWidget *parent = nullptr);
	~DWDAboutDialog();

private:
	Ui::DWDAboutDialog *m_ui;
};

#endif // DWDABOUTDIALOG_H
