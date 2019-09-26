#include "DeleteGroupUndo.h"
#include "PathFileTreeModel.h"

using namespace xero::paths;

DeleteGroupUndo::DeleteGroupUndo(PathFileTreeModel& model, int row, std::shared_ptr<PathGroup> group) : model_(model)
{
	row_ = row;
	group_ = group;
}

DeleteGroupUndo::~DeleteGroupUndo()
{
}

QString DeleteGroupUndo::toString()
{
	QString ret("DeleteGroupUndo ");
	ret += group_->getName().c_str();
	return ret;
}

void DeleteGroupUndo::undo()
{
	model_.insertPathGroup(row_, group_);
}