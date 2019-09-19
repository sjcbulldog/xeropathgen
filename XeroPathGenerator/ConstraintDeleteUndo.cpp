#include "ConstraintDeleteUndo.h"

using namespace xero::paths;

ConstraintDeleteUndo::ConstraintDeleteUndo(ConstraintEditor& editor, int row, const DistanceVelocityConstraint& con) : editor_(editor), deleted_(con)
{
	row_ = row;
}

ConstraintDeleteUndo::~ConstraintDeleteUndo()
{
}

QString ConstraintDeleteUndo::toString()
{
	QString result("ConstraintDeleteUndo:");
	result += " " + QString::number(row_);
	result += " " + QString::number(deleted_.getAfter());
	result += " " + QString::number(deleted_.getBefore());
	result += " " + QString::number(deleted_.getVelocity());

	return result;
}

void ConstraintDeleteUndo::undo()
{
	editor_.addConstraint(row_, deleted_);
}