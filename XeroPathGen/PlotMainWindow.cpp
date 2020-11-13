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
#include "PlotMainWindow.h"

using namespace xero::paths;

PlotMainWindow::PlotMainWindow(QWidget *parent) : QWidget(parent)
{
	layout_ = new QVBoxLayout(this);
	view_ = new PathPlotView();
	bar_ = new QScrollBar(this);
	layout_->addWidget(view_);
	layout_->addWidget(bar_);

	setLayout(layout_);

	bar_->setRange(0, 100);
	bar_->setValue(50);
	bar_->setOrientation(Qt::Horizontal);
	bar_->setVisible(true);
}

PlotMainWindow::~PlotMainWindow()
{
}

void PlotMainWindow::setPath(std::shared_ptr<RobotPath> path) {
	//
	// Setup the scroll bar range to be in milliseconds
	//
	path_ = path;
	updateScrollBar();
}

void PlotMainWindow::updateScrollBar()
{
	int pathmax;
	if (path_ == nullptr)
		pathmax = 0;
	else
		pathmax = static_cast<int>(path_->getTime() * 1000) + 1;

	bar_->setValue(0);
	bar_->setRange(0, pathmax);
	bar_->setSingleStep(20);		// TODO - this should come from the robot timestep
}

void PlotMainWindow::update()
{
	if (view_ != nullptr)
		view_->update();

	updateScrollBar();
}
