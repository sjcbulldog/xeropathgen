#include "DeletePathUndo.h"
#include "PathFileTreeModel.h"

DeletePathUndo::DeletePathUndo(PathFileTreeModel& model, const std::string& group, int index, std::shared_ptr<xero::paths::RobotPath> path) : model_(model)
{
	path_ = path;
	index_ = index;
	group_ = group;
}

DeletePathUndo::~DeletePathUndo()
{
}

QString DeletePathUndo::toString()
{
	QString ret("DeletePathUndo");
	return ret;
}

void DeletePathUndo::undo()
{
	model_.addPath(group_, index_, path_);
}