#pragma once
#include "UndoItem.h"

class PathFileTreeModel;

class GroupNameChangeUndo : public UndoItem
{
public:
	GroupNameChangeUndo(PathFileTreeModel& model, const std::string& newname, const std::string& oldname);
	virtual ~GroupNameChangeUndo();

	virtual QString toString();
	virtual void undo();

private:
	PathFileTreeModel& model_;
	std::string newname_;
	std::string oldname_;
};

