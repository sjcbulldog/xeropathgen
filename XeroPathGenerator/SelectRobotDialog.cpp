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
#include "SelectRobotDialog.h"
#include <QAction>

using namespace xero::paths;

SelectRobotDialog::SelectRobotDialog(RobotManager &mgr, std::shared_ptr<RobotParams> current, QWidget *parent) : QDialog(parent), mgr_(mgr)
{
	ui.setupUi(this);
	ui.robots_->setModel(&model_);

	connect(ui.buttons_, &QDialogButtonBox::accepted, this, &SelectRobotDialog::accept);
	connect(ui.buttons_, &QDialogButtonBox::rejected, this, &SelectRobotDialog::reject);

	for (auto robot : mgr_.getRobots())
	{
		if (robot != current)
		{
			QList<QStandardItem*> item =
			{
				new QStandardItem(robot->getName().c_str())
			};
			model_.appendRow(item);
		}
	}
}

SelectRobotDialog::~SelectRobotDialog()
{
}

QString SelectRobotDialog::getSelectedRobot()
{
	QString ret;

	QModelIndex index = ui.robots_->currentIndex();
	if (index.isValid())
	{
		ret = model_.data(index, Qt::DisplayRole).toString();
	}

	return ret;
}
