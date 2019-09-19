#pragma once

#include "UndoItem.h"

class WaypointAddUndo : public UndoItem
{
public:
	WaypointAddUndo();
	virtual ~WaypointAddUndo();

	virtual QString toString();
	virtual void undo();

private:
};

