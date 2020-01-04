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

	ActionDB& getDB() {
		return db_;
	}

protected:
	void insertChildren(int indent, std::shared_ptr<Action> act, QTreeWidgetItem* parent);

private:
	void itemSelected(QTreeWidgetItem* current, QTreeWidgetItem* prev);

private:
	ActionDB &db_;
};
