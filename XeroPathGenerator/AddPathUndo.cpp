#include "AddPathUndo.h"
#include "PathFileTreeModel.h"

AddPathUndo::AddPathUndo(PathFileTreeModel& model, const std::string& group, const std::string& path) : model_(model)
{
	path_ = path;
	group_ = group;
}

AddPathUndo::~AddPathUndo()
{
}

QString AddPathUndo::toString() 
{
	QString ret("AddPathUndo ");
	ret += group_.c_str();
	ret += " ";
	ret += path_.c_str();
	return ret;
}

void AddPathUndo::undo()
{
	model_.deletePath(group_, path_, false);
}
