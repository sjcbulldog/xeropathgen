#include "ActionView.h"
#include <QString>

QStringList labels =
{
	"Start",
	"End",
	"Cancel",
	"ID",
	"Parent",
	"Text"
};

ActionView::ActionView(ActionDB &db, QWidget *parent)	: QTreeWidget(parent), db_(db)
{
	setColumnCount(4);
	setHeaderLabels(labels);
}

ActionView::~ActionView()
{
}

void ActionView::updateContents()
{
	clear();

	for (auto act : db_.getActions())
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(this);
		item->setText(0, QString::number(act->getStartTime()));
		if (act->isDone())
			item->setText(1, QString::number(act->getEndTime()));
		else if (act->isCanceled())
			item->setText(2, QString::number(act->getCancelTime()));

		item->setText(3, QString::number(act->getID()));
		if (act->getParent() != nullptr)
			item->setText(4, QString::number(act->getParent()->getID()));
		item->setText(5, act->getText());

		if (act->hasChildren())
		{
			insertChildren(4, act, item);
		}
	}
}

void ActionView::insertChildren(int indent, std::shared_ptr<Action> act, QTreeWidgetItem *parent)
{
	for (auto child : act->getChildren())
	{
		QString txt;

		for (int i = 0; i < indent; i++)
			txt += ' ';

		txt += child->getText();
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(5, txt);
		parent->addChild(item);

		if (child->hasChildren())
			insertChildren(indent + 4, child, item);
	}
}