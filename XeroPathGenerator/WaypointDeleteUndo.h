#pragma once
#include "UndoItem.h"
#include <RobotPath.h>
#include <Pose2d.h>
#include <memory>

class PathFieldView;

class WaypointDeleteUndo : public UndoItem
{
public:
	WaypointDeleteUndo(PathFieldView& field, const std::string& group, const std::string& path, size_t index, const xero::paths::Pose2d& pt);
	virtual ~WaypointDeleteUndo();

	virtual QString toString();
	virtual void undo();

private:
	PathFieldView& fields_;
	std::string group_;
	std::string path_;
	size_t index_;
	xero::paths::Pose2d waypoint_;
};

