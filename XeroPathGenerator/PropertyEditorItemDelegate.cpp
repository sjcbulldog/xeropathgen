#include "PropertyEditorItemDelegate.h"
#include <QComboBox>
#include <QPainter>
#include <QDebug>

QWidget* PropertyEditorItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	auto prop = model_.getProperty(index);
	if (prop == nullptr || prop->getType() != EditableProperty::PropertyType::PTStringList)
		return QStyledItemDelegate::createEditor(parent, option, index);

	QComboBox* box = new QComboBox(parent);
	const QList<QString>& choices = prop->getChoices();
	for (const QString& str : choices)
		box->addItem(str);

	int which = box->findData(prop->getValue());
	if (which != -1)
		box->setCurrentIndex(which);

	return box;
}

void PropertyEditorItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);
}

QSize PropertyEditorItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize ret;
	int h = option.fontMetrics.lineSpacing();
	auto prop = model_.getProperty(index);
	QString txt;
	int total;

	if (index.column() == 0)
	{
		txt = prop->getName();
		total = option.fontMetrics.horizontalAdvance(txt);
		ret = QSize(total, h);
	}
	else if (index.column() == 1)
	{
		ret = QSize(80, h);
	}
	if (index.column() == 2)
	{
		txt = prop->getDescription();
		total = option.fontMetrics.horizontalAdvance(txt);

		int width = option.fontMetrics.horizontalAdvance("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		if (total > width)
			h = option.fontMetrics.lineSpacing() * 2;
		ret = QSize(width, h);
	}

	return ret;
}
