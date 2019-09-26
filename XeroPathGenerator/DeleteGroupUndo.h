#pragma once
#include "UndoItem.h"
#include "PathGroup.h"

class PathFileTreeModel;

class DeleteGroupUndo :	public UndoItem
{
public:
	DeleteGroupUndo(PathFileTreeModel& model, int row, std::shared_ptr<xero::paths::PathGroup> group);
	virtual ~DeleteGroupUndo();

	virtual QString toString();
	virtual void undo();

private:
	PathFileTreeModel& model_;
	int row_;
	std::shared_ptr<xero::paths::PathGroup> group_;
};

