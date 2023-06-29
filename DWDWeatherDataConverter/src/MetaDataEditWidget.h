#ifndef METADATAEDITWIDGET_H
#define METADATAEDITWIDGET_H

#include <QWidget>
#include <CCM_ClimateDataLoader.h>

namespace Ui {
	class MetaDataEditWidget;
}

class MetaDataEditWidget : public QWidget {
	Q_OBJECT

public:
	explicit MetaDataEditWidget(QWidget *parent = 0);
	~MetaDataEditWidget();

	void updateUi();

	CCM::ClimateDataLoader  *m_ccm;

signals:
	/*! Emitted, when user has entered a new (valid) start year. */
	void startYearChanged(int);

private slots:
	void on_tableWidgetProperties_cellChanged(int row, int column);

	void on_plainTextEdit_textChanged();

private:
	Ui::MetaDataEditWidget *ui;
};

#endif // METADATAEDITWIDGET_H
