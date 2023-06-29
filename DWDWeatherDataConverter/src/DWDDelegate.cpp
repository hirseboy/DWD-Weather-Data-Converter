#include "DWDDelegate.h"

#include <QApplication>
#include <QStyleOptionButton>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>

#include "DWDDescriptonData.h"


DWDDelegate::DWDDelegate( QObject *parent )
	: QStyledItemDelegate( parent )
{
}

DWDDelegate::~DWDDelegate()
{
}

void DWDDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{

	QStyleOptionButton opt;// Setting parameters
	Qt::ItemFlags flags = index.flags();
	Qt::CheckState state = (Qt::CheckState)index.data( Qt::CheckStateRole ).toInt();
	bool isSelectable = flags.testFlag(Qt::ItemIsUserCheckable);

	if( option.state & QStyle::State_Selected )//we need this to show selection
		painter->fillRect( option.rect, option.palette.highlight() );

	if (!isSelectable)
		return;

	opt.state = QStyle::State_Enabled; // CheckBox enabled
	if ( option.state & QStyle::State_MouseOver )
		opt.state |= QStyle::State_MouseOver; // Mouse over cell
	if ( state == Qt::Checked )
		opt.state |= QStyle::State_On;
	else
		opt.state |= QStyle::State_Off;

	DWDDescriptonData::DWDDataType dataType = (DWDDescriptonData::DWDDataType)index.data(Qt::UserRole).toInt();

	QColor color = DWDDescriptonData::color(dataType);
	if(state == Qt::Checked)//we need this to show selection
		painter->fillRect( option.rect, QBrush(color) );
	else {
		color.setAlpha(50);
		painter->fillRect( option.rect, QBrush(color) );
	}

	// Check box rect
	opt.rect = QApplication::style()->subElementRect( QStyle::SE_CheckBoxIndicator, &opt, nullptr );
	const int x = option.rect.center().x() - opt.rect.width() / 2;
	const int y = option.rect.center().y() - opt.rect.height() / 2;
	opt.rect.moveTo( x, y );

	// Mandatory: drawing check box
	QApplication::style()->drawControl( QStyle::CE_CheckBox, &opt, painter );
}

bool DWDDelegate::editorEvent( QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index ) {

	switch ( event->type() )
	{
	case QEvent::MouseButtonPress:
		m_lastClickedIndex = index;
		break;

	case QEvent::MouseButtonRelease:
	{
		if ( index != m_lastClickedIndex )
			break;
		QMouseEvent *e = static_cast< QMouseEvent * >( event );
		if ( e->button() != Qt::LeftButton )
			break;
		m_lastClickedIndex = QModelIndex();

		QStyleOptionButton opt;
		opt.rect = QApplication::style()->subElementRect( QStyle::SE_CheckBoxIndicator, &opt, NULL );
		const int x = option.rect.center().x() - opt.rect.width() / 2;
		const int y = option.rect.center().y() - opt.rect.height() / 2;
		opt.rect.moveTo( x, y );

		if ( opt.rect.contains( e->pos() ) )
		{
			// TODO: process click on checkbox logic
			Qt::CheckState state = (Qt::CheckState)index.data( Qt::CheckStateRole ).toInt();
			switch ( state )
			{
			case Qt::Unchecked:
				state = Qt::Checked;
				break;
			case Qt::Checked:
				state = Qt::Unchecked;
				break;
			}

			// qDebug() << state;
			model->setData( index, state, Qt::CheckStateRole );

		}
		return true;
	}

	default:
		break;
	}


	return QAbstractItemDelegate::editorEvent( event, model, option, index );
}

