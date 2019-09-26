#pragma once
#include "UndoItem.h"
#include <RobotPath.h>

class PathFileTreeModel;

class DeletePathUndo :public UndoItem
{
public:
	DeletePathUndo(PathFileTreeModel& model, const std::string &group, int index, std::shared_ptr<xero::paths::RobotPath> path);
	virtual ~DeletePathUndo();

	virtual QString toString();
	virtual void undo();

private:
	PathFileTreeModel& model_;
	int index_;
	std::shared_ptr<xero::paths::RobotPath> path_;
	std::string group_;
};

