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

#include "PathPlotView.h"
#include <RobotPath.h>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollBar>

class PlotMainWindow : public QWidget
{
	Q_OBJECT

public:
	PlotMainWindow(QWidget *parent = Q_NULLPTR);
	~PlotMainWindow();

	void setPath(std::shared_ptr<xero::paths::RobotPath> path);

	PathPlotView* plotview() {
		return view_;
	}

	QScrollBar* bar() {
		return bar_;
	}

	double getTime() const {
		return bar_->value() / 1000.0;
	}

	void update();

private:
	void updateScrollBar();

private:
	QVBoxLayout* layout_;
	QScrollBar* bar_;
	PathPlotView* view_;
	std::shared_ptr<xero::paths::RobotPath> path_;
};
