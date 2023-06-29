#ifndef DWDDelegate_H
#define DWDDelegate_H

#include <QStyledItemDelegate>
#include <QModelIndex>

class DWDDelegate
		: public QStyledItemDelegate
{
	Q_OBJECT

public:
	DWDDelegate( QObject *parent = nullptr);
	~DWDDelegate() override;

	void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	bool editorEvent( QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index );

private:
	QModelIndex m_lastClickedIndex;
};

#endif // DWDDelegate_H
