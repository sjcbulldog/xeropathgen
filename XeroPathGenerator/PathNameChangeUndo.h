#pragma once
#include "UndoItem.h"
#include "RobotPath.h"

class PathFileTreeModel;

class PathNameChangeUndo : public UndoItem
{
public:
	PathNameChangeUndo(PathFileTreeModel& model, const std::string &groupname, const std::string &newname, const std::string& oldname);
	virtual ~PathNameChangeUndo();

	virtual QString toString();
	virtual void undo();

private:
	PathFileTreeModel& model_;
	std::string group_;
	std::string newname_;
	std::string oldname_;
};

