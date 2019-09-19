#pragma once

#include "UndoItem.h"
#include <Pose2d.h>
#include <RobotPath.h>

class PathFieldView;

class WaypointAddUndo : public UndoItem
{
public:
	WaypointAddUndo(PathFieldView& field, std::shared_ptr<xero::paths::RobotPath> path, size_t index);
	virtual ~WaypointAddUndo();

	virtual QString toString();
	virtual void undo();

private:
	PathFieldView& field_;
	std::shared_ptr<xero::paths::RobotPath> path_;
	size_t index_;
};

