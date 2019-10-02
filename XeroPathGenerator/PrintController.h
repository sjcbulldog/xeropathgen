#pragma once

#include <RobotPath.h>
#include <PathGroup.h>
#include <QPrinter>
#include <QPainter>

class PathFieldView;
class PathPlotView;

class PrintController
{
public:
	PrintController(PathFieldView& field, PathPlotView& plot);
	virtual ~PrintController();

	void printField(QPrinter& printer, std::shared_ptr<xero::paths::RobotPath> path);
	void printFieldTrajectory(QPrinter& printer, std::shared_ptr<xero::paths::RobotPath> path);
	void printGroup(QPrinter& printer, std::shared_ptr<xero::paths::PathGroup> group);

private:
	void mapToPaperRect(QPainter& paint, const QRect& loc, const QSize& size);

private:
	PathFieldView& field_;
	PathPlotView& plot_;
};

