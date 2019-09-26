#pragma once
#include "UndoItem.h"

class PathFileTreeModel;

class AddGroupUndo : public UndoItem
{
public:
	AddGroupUndo(PathFileTreeModel& model, const std::string& group);
	virtual ~AddGroupUndo();

	virtual QString toString();
	virtual void undo();

private:
	PathFileTreeModel& model_;
	std::string group_;
};

