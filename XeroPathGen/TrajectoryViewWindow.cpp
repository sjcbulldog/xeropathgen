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
#include "TrajectoryViewWindow.h"

using namespace xero::paths;

TrajectoryViewWindow::TrajectoryViewWindow(QWidget *parent) : QDialog(parent)
{
	setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());

	layout_ = new QVBoxLayout();
	setLayout(layout_);
	box_ = new QComboBox(this);
	layout_->addWidget(box_);
	table_ = new QTableWidget(this);
	layout_->addWidget(table_);

	box_->setEditable(false);
	(void)connect(box_, &QComboBox::currentTextChanged, this, &TrajectoryViewWindow::updateTrajectory);
}

TrajectoryViewWindow::~TrajectoryViewWindow()
{
}

void TrajectoryViewWindow::updateTrajectory(const QString& text)
{
	which_ = text.toStdString();
	auto traj = path_->getTrajectory(which_);

	table_->clearContents();
	table_->setColumnCount(8);
	table_->setRowCount(0);

	QStringList hdlist;
	hdlist << "Time" << "X" << "Y" << "Heading" << "Position" << "Velocity" << "Accel" << "Jerk";
	table_->setHorizontalHeaderLabels(hdlist);

	if (traj != nullptr)
	{
		table_->setRowCount(static_cast<int>(traj->size()));

		QTableWidgetItem* item;
		int row = 0;
		for (const auto& pt : (*traj))
		{
			item = new QTableWidgetItem(QString::number(pt.time(), 'f', 2));
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			table_->setItem(row, 0, item);

			item = new QTableWidgetItem(QString::number(pt.x(), 'f', 1));
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			table_->setItem(row, 1, item);

			item = new QTableWidgetItem(QString::number(pt.y(), 'f', 1));
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			table_->setItem(row, 2, item);

			item = new QTableWidgetItem(QString::number(pt.rotation().toDegrees(), 'f', 1));
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			table_->setItem(row, 3, item);

			item = new QTableWidgetItem(QString::number(pt.position(), 'f', 1));
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			table_->setItem(row, 4, item);

			item = new QTableWidgetItem(QString::number(pt.velocity(), 'f', 1));
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			table_->setItem(row, 5, item);

			item = new QTableWidgetItem(QString::number(pt.acceleration(), 'f', 1));
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			table_->setItem(row, 6, item);

			item = new QTableWidgetItem(QString::number(pt.jerk(), 'f', 1));
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			table_->setItem(row, 7, item);

			row++;
		}
	}

	table_->resizeColumnsToContents();
}

void TrajectoryViewWindow::update()
{
	updateTrajectory(box_->currentText());
}

void TrajectoryViewWindow::setPath(std::shared_ptr<RobotPath> path)
{
	path_ = path;

	box_->clear();
	auto names =  path_->getTrajectoryNames();
	for (const std::string& name : names)
		box_->addItem(name.c_str());
}

void TrajectoryViewWindow::setCurrentTime(double t)
{
	auto traj = path_->getTrajectory(which_);
	if (traj != nullptr)
	{
		int row = static_cast<int>(traj->getIndex(t));

		if (row < table_->rowCount())
		{
			auto model = table_->model();
			auto index = model->index(row, 0);
			table_->scrollTo(index);

			QTableWidgetSelectionRange deselect(0, 0, table_->rowCount() - 1, table_->columnCount() - 1);
			table_->setRangeSelected(deselect, false);

			QTableWidgetSelectionRange range(row, 0, row, table_->columnCount() - 1);
			table_->setRangeSelected(range, true);
		}
	}
}