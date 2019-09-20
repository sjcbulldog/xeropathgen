#pragma once
#include "UndoItem.h"
#include <RobotPath.h>
#include <Pose2d.h>

class XeroPathGenerator;

class WaypointChangedUndo :	public UndoItem
{
public:
	WaypointChangedUndo(XeroPathGenerator& gen, const std::string &group, const std::string &path, size_t index, const xero::paths::Pose2d& pt);
	virtual ~WaypointChangedUndo();

	virtual QString toString();
	virtual void undo();

private:
	XeroPathGenerator& gen_;
	std::string group_;
	std::string path_;
	size_t index_;
	xero::paths::Pose2d waypoint_;
};

