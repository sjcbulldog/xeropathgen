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

#include "RobotPath.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QComboBox>

class TrajectoryViewWindow : public QDialog
{
	Q_OBJECT

public:
	TrajectoryViewWindow(QWidget *parent = Q_NULLPTR);
	~TrajectoryViewWindow();

	void setPath(std::shared_ptr<xero::paths::RobotPath> path);
	bool isShowingPath(std::shared_ptr<xero::paths::RobotPath> path) {
		return path == path_;
	}
	void setCurrentTime(double t);

	void update();

private:
	void updateTrajectory(const QString &text);

private:
	QVBoxLayout* layout_;
	QComboBox* box_;
	QTableWidget* table_;
	std::shared_ptr<xero::paths::RobotPath> path_;
	std::string which_;
};