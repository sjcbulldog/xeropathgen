#pragma once

#include "UndoItem.h"


class PathFileTreeModel;

class AddPathUndo : public UndoItem
{
public:
	AddPathUndo(PathFileTreeModel& model, const std::string& group, const std::string& path);
	virtual ~AddPathUndo();

	virtual QString toString();
	virtual void undo();

private:
	PathFileTreeModel& model_;
	std::string group_;
	std::string path_;
};

