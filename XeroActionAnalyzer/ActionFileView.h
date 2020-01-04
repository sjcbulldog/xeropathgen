#pragma once

#include "ActionDB.h"
#include "ActionView.h"
#include "ActionTimelineView.h"
#include <QScrollArea>
#include <QSplitter>

class ActionFileView : public QSplitter
{
	Q_OBJECT

public:
	ActionFileView(QWidget *parent);
	~ActionFileView();

	ActionDB& getDB() {
		return db_;
	}

	void itemSelected(QTreeWidgetItem* current, QTreeWidgetItem* prev);

	void updateContents() {
		action_view_->updateContents();
		timeline_->updateContents(-1);
	}

private:
	ActionView* action_view_;
	ActionTimelineView* timeline_;
	QScrollArea* scroll_;
	ActionDB db_;
};
