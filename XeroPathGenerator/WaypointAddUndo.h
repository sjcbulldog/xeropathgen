#pragma once

#include "UndoItem.h"
#include <Pose2d.h>
#include <RobotPath.h>

class PathFieldView;

class WaypointAddUndo : public UndoItem
{
public:
	WaypointAddUndo(PathFieldView& field, const std::string &group, const std::string &path, size_t index);
	virtual ~WaypointAddUndo();

	virtual QString toString();
	virtual void undo();

private:
	PathFieldView& field_;
	std::string group_;
	std::string path_;
	size_t index_;
};

