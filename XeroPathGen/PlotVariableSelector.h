//
// Copyright 2019 Jack W. Griffin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//
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
