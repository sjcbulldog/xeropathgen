#pragma once

#include "Action.h"
#include "ActionDB.h"
#include <QTreeWidget>
#include <list>
#include <memory>

class ActionView : public QTreeWidget
{
	Q_OBJECT

public:
	ActionView(ActionDB &db, QWidget *parent = Q_NULLPTR);
	~ActionView();

	void updateContents();

protected:
	void insertChildren(int indent, std::shared_ptr<Action> act, QTreeWidgetItem* parent);

private:
	ActionDB &db_;
};
