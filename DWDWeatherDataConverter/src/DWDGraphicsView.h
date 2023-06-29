#ifndef DWDGRAPHICSVIEW_H
#define DWDGRAPHICSVIEW_H

#include <QGraphicsView>

namespace Ui {
class DWDGraphicsView;
}

class DWDGraphicsView : public QGraphicsView
{
	Q_OBJECT



public:
	explicit DWDGraphicsView(QWidget *parent = nullptr);
	~DWDGraphicsView();


	// QWidget interface
protected:
	void wheelEvent(QWheelEvent * event) override;
	void resizeEvent(QResizeEvent * event) override;
};

#endif // DWDGRAPHICSVIEW_H
