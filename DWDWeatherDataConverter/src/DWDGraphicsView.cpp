#include "DWDGraphicsView.h"

#include <QWheelEvent>

DWDGraphicsView::DWDGraphicsView(QWidget *parent) :
	QGraphicsView(parent)
{
	setRenderHint(QPainter::Antialiasing);
}

DWDGraphicsView::~DWDGraphicsView()
{

}

void DWDGraphicsView::wheelEvent(QWheelEvent * event) {
//	if (event->modifiers() & Qt::ControlModifier) {
//		// zoom
//		const ViewportAnchor anchor = transformationAnchor();
//		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
//		int angle = event->angleDelta().y();
//		qreal factor;
//		if (angle > 0) {
//			factor = 1.1;
//		} else {
//			factor = 0.9;
//		}
//		scale(factor, factor);
//		setTransformationAnchor(anchor);
//	} else {
//		QGraphicsView::wheelEvent(event);
//	}
}

void DWDGraphicsView::resizeEvent(QResizeEvent * event) {
	fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}
