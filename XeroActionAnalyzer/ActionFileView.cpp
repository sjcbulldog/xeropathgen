#include "ActionFileView.h"

ActionFileView::ActionFileView(QWidget *parent)	: QSplitter(parent)
{
	setOrientation(Qt::Vertical);
	action_view_ = new ActionView(db_, this);
	this->addWidget(action_view_);

	(void)connect(action_view_, &QTreeWidget::currentItemChanged, this, &ActionFileView::itemSelected);

	scroll_ = new QScrollArea(this);
	scroll_->setWidgetResizable(true);
	this->addWidget(scroll_);

	timeline_ = new ActionTimelineView(db_, scroll_);
	scroll_->setWidget(timeline_);

	QList<int> sizes;
	sizes.push_back(height() / 2);
	sizes.push_back(height() / 2);
	setSizes(sizes);
}

ActionFileView::~ActionFileView()
{
}


void ActionFileView::itemSelected(QTreeWidgetItem* current, QTreeWidgetItem* prev)
{
	if (current != nullptr)
	{
		QString idtxt = current->text(3);
		int id = idtxt.toInt();
		timeline_->updateContents(id);
	}
	else
	{
		timeline_->updateContents(-1);
	}
}