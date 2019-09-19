#pragma once
#include "UndoItem.h"
#include <RobotPath.h>
#include <Pose2d.h>
#include <memory>

class PathFieldView;

class WaypointDeleteUndo : public UndoItem
{
public:
	WaypointDeleteUndo(PathFieldView& field, std::shared_ptr<xero::paths::RobotPath> path, size_t index, const xero::paths::Pose2d& pt);
	virtual ~WaypointDeleteUndo();

	virtual QString toString();
	virtual void undo();

private:
	PathFieldView& fields_;
	std::shared_ptr<xero::paths::RobotPath> path_;
	size_t index_;
	xero::paths::Pose2d waypoint_;
};

