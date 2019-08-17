#pragma once

#include "PropertyEditorTreeModel.h"
#include <QStyledItemDelegate>

class PropertyEditorItemDelegate : public QStyledItemDelegate
{
public:
	PropertyEditorItemDelegate(PropertyEditorTreeModel& model, QObject *parent = nullptr) : QStyledItemDelegate(parent), model_(model)
	{
	}

protected:
	QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	PropertyEditorTreeModel& model_;
};

