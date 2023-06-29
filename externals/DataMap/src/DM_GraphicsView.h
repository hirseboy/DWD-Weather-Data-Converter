#ifndef DM_GraphicsViewH
#define DM_GraphicsViewH

#include <QGraphicsView>


namespace DM {

/*! Inherited Graphics View to view map with data points. */
class GraphicsView : public QGraphicsView
{
	Q_OBJECT
public:

	explicit GraphicsView(QWidget *parent = nullptr);

	~GraphicsView();

protected:
	void wheelEvent(QWheelEvent * event) override;
	void resizeEvent(QResizeEvent * event) override;

	bool m_isPanning = false;
	QPoint m_lastPos;


	// QWidget interface
protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
};

}
#endif // DWDGRAPHICSVIEW_H
