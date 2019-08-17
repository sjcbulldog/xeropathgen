#pragma once

#include <RobotParams.h>
#include <QDialog>
#include "ui_PlotVariableSelector.h"

class PlotVariableSelector : public QDialog
{
	Q_OBJECT

public:
	PlotVariableSelector(xero::paths::RobotParams::DriveType type, QList<QString> selected, QWidget *parent = Q_NULLPTR);
	~PlotVariableSelector();

	QList<QString> getPlotVariables();

private:
	Ui::PlotVariableSelector ui;
};
