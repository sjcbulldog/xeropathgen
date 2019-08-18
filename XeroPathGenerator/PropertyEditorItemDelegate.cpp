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
	int h = option.fontMetrics.height() + 2;
	auto prop = model_.getProperty(index);
	QString text = prop->getDescription();

	int width = option.fontMetrics.horizontalAdvance("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
	int total = option.fontMetrics.horizontalAdvance(text);
	if (total > width)
		h = option.fontMetrics.height() * 2 + 2;
	return QSize(width, h);
}
