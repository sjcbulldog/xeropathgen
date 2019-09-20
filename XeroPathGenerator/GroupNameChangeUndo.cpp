#include "GroupNameChangeUndo.h"
#include "PathFileTreeModel.h"

GroupNameChangeUndo::GroupNameChangeUndo(PathFileTreeModel& model, const std::string& newname, const std::string& oldname) : model_(model)
{
	newname_ = newname;
	oldname_ = oldname;
}

GroupNameChangeUndo::~GroupNameChangeUndo()
{
}

QString GroupNameChangeUndo::toString()
{
	QString ret("GroupNameChangeUndo");
	return ret;
}

void GroupNameChangeUndo::undo()
{
	model_.renameGroup(newname_, oldname_);
}