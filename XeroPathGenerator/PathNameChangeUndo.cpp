#include "PathNameChangeUndo.h"
#include "PathFileTreeModel.h"

using namespace xero::paths;

PathNameChangeUndo::PathNameChangeUndo(PathFileTreeModel& model, const std::string& groupname, const std::string& newname, const std::string& oldname) : model_(model)
{
	group_ = groupname;
	oldname_ = oldname;
	newname_ = newname;
}

PathNameChangeUndo::~PathNameChangeUndo()
{
}

QString PathNameChangeUndo::toString()
{
	QString ret("PathNameChangeUndo");
	return ret;
}

void PathNameChangeUndo::undo()
{
	model_.renamePath(group_, newname_, oldname_);
}