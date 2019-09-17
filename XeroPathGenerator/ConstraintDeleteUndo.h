#pragma once

#include "UndoItem.h"
#include "ConstraintEditor.h"
#include <DistanceVelocityConstraint.h>

class ConstraintDeleteUndo : public UndoItem
{
public:
	ConstraintDeleteUndo(ConstraintEditor &editor, const xero::paths::DistanceVelocityConstraint& con);
	virtual ~ConstraintDeleteUndo();
	virtual QString toString();
	virtual void undo();

private:
	ConstraintEditor &editor_;

	//
	// The constraint that was deleted
	//
	xero::paths::DistanceVelocityConstraint deleted_;
};

