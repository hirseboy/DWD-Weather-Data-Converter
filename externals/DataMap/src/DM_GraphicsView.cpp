#include "DM_GraphicsView.h"
#include "qscrollbar.h"

#include <QWheelEvent>

namespace DM {
GraphicsView::GraphicsView(QWidget *parent) :
	QGraphicsView(parent)
{
	setRenderHint(QPainter::Antialiasing);
	// setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setCacheMode(QGraphicsView::CacheBackground);
}

GraphicsView::~GraphicsView() {

}

void GraphicsView::wheelEvent(QWheelEvent * event) {

	// zoom
	const ViewportAnchor anchor = transformationAnchor();
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	int angle = event->angleDelta().y();
	qreal factor;
	if (angle > 0) {
		factor = 1.1;
	} else {
		factor = 0.9;
	}
	scale(factor, factor);
	setTransformationAnchor(anchor);

	return;

	QGraphicsView::wheelEvent(event);
}

void GraphicsView::resizeEvent(QResizeEvent * event) {
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::MiddleButton) {
		m_lastPos = event->pos();
		m_isPanning = true;
		setCursor(Qt::ClosedHandCursor);
	}
	QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::MiddleButton) {
		m_isPanning = false;
		setCursor(Qt::ArrowCursor);
	}
	QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {
	if (m_isPanning) {
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - m_lastPos.x()));
		verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - m_lastPos.y()));
		m_lastPos = event->pos();
	}
	QGraphicsView::mouseMoveEvent(event);
}
}
