#include "AddGroupUndo.h"
#include "PathFileTreeModel.h"

AddGroupUndo::AddGroupUndo(PathFileTreeModel& model, const std::string& group) : model_(model)
{
	group_ = group;
}

AddGroupUndo::~AddGroupUndo()
{
}

QString AddGroupUndo::toString()
{
	QString ret("AddGroupUndo ");
	ret += group_.c_str();

	return ret;
}

void AddGroupUndo::undo()
{
	model_.deleteGroup(group_, false);
}
