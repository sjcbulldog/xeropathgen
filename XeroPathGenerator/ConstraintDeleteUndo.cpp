#include "ConstraintDeleteUndo.h"

using namespace xero::paths;

ConstraintDeleteUndo::ConstraintDeleteUndo(ConstraintEditor& editor, const DistanceVelocityConstraint& con) : editor_(editor), deleted_(con)
{
}

ConstraintDeleteUndo::~ConstraintDeleteUndo()
{
}

QString ConstraintDeleteUndo::toString()
{
	QString result("ConstraintDeleteUndo:");
	result += " " + QString::number(deleted_.getAfter());
	result += " " + QString::number(deleted_.getBefore());
	result += " " + QString::number(deleted_.getVelocity());

	return result;
}

void ConstraintDeleteUndo::undo()
{
	editor_.addConstraint(deleted_);
}