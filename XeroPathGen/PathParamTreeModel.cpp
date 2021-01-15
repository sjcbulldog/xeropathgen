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
#include "PathParamTreeModel.h"
#include "GeneratorManager.h"
#include "Generator.h"
#include "PathParameterChangeUndo.h"
#include "UndoManager.h"

using namespace xero::paths;

std::vector<std::string> PathParamTreeModel::RowLabelsAngles =
{
	"Start Velocity",
	"End Velocity",
	"Max Velocity",
	"Max Acceleration",
	"Max Jerk",
	"Max Centripetal Force (N)",
	"Start Angle",
	"Start Angle Delay",
	"End Angle",
	"End Angle Delay",
	"Length",
	"Time",
};

std::vector<std::string> PathParamTreeModel::RowLabelsNoAngles =
{
	"Start Velocity",
	"End Velocity",
	"Max Velocity",
	"Max Acceleration",
	"Max Jerk",
	"Max Centripetal Force (N)",
	"Length",
	"Time"
};

PathParamTreeModel::PathParamTreeModel(PathFileTreeModel &model) : pathfile_model_(model)
{
	setAnglesRequired(false);
}

PathParamTreeModel::~PathParamTreeModel()
{
}

QModelIndex PathParamTreeModel::index(int row, int col, const QModelIndex& parent) const
{
  	(void)parent ;
	return createIndex(row, col);
}

QModelIndex PathParamTreeModel::parent(const QModelIndex& index) const
{
  	(void)index ;
	return QModelIndex();
}

Qt::ItemFlags PathParamTreeModel::flags(const QModelIndex& index) const
{
	if (path_ == nullptr)
		return Qt::ItemFlags();

	if (index.row() <= 1) {
		//
		// Start and end velocity
		//
		if (!generator_->hasProperty(Generator::NonZeroStartEndVelocities))
			return Qt::ItemFlags();
	}

	if (index.column() == 0 || index.row() >= read_write_rows_)
		return Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;

	return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

int PathParamTreeModel::rowCount(const QModelIndex& parent) const
{
	int ret;

	if (parent.isValid() || path_ == nullptr)
		return 0;

	if (angles_required_)
		ret = static_cast<int>(RowLabelsAngles.size());
	else
		ret = static_cast<int>(RowLabelsNoAngles.size());

	return ret;
}

int PathParamTreeModel::columnCount(const QModelIndex& parent) const
{
 	(void)parent ;
	if (path_ == nullptr)
		return 0;

	return 2;
}

QVariant PathParamTreeModel::headerData(int section, Qt::Orientation orient, int role) const
{
	QVariant ret;

	if (role == Qt::DisplayRole && orient == Qt::Orientation::Horizontal)
	{
		if (section == 0)
			ret = "Name";
		else if (section == 1)
			ret = "Value";
	}

	return ret;
}

QVariant PathParamTreeModel::data(const QModelIndex& index, int role) const
{
	QVariant ret;

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return ret;

	if (index.column() == 0)
	{
		if (angles_required_)
			ret = RowLabelsAngles[index.row()].c_str();
		else
			ret = RowLabelsNoAngles[index.row()].c_str();
	}
	else
	{
		if (path_ == nullptr)
			ret = "0";
		else
		{
			switch (index.row())
			{
			case 0:
				if (!generator_->hasProperty(Generator::NonZeroStartEndVelocities))
					ret = "0.0";
				else
					ret = QString::number(path_->getStartVelocity(), 'f', 1);
				break;
			case 1:
				if (!generator_->hasProperty(Generator::NonZeroStartEndVelocities))
					ret = "0.0";
				else
					ret = QString::number(path_->getEndVelocity(), 'f', 1);
				break;
			case 2:
				ret = QString::number(path_->getMaxVelocity(), 'f', 1);
				break;
			case 3:
				ret = QString::number(path_->getMaxAccel(), 'f', 1);
				break;
			case 4:
				ret = QString::number(path_->getMaxJerk(), 'f', 1) ;
				break;
			case 5:
				ret = QString::number(path_->getMaxCentripetal(), 'f', 1);
				break;
			case 6:
				if (angles_required_)
					ret = QString::number(path_->getStartAngle(), 'f', 1);
				else
					ret = QString::number(path_->getDistance(), 'f', 1);
				break;
			case 7:
				if (angles_required_)
					ret = QString::number(path_->getStartAngleDelay(), 'f', 1);
				else
					ret = QString::number(path_->getTime());
				break;
			case 8:
				ret = QString::number(path_->getEndAngle(), 'f', 1);
				break;
			case 9:
				ret = QString::number(path_->getEndAngleDelay(), 'f', 1);
				break;
			case 10:
				ret = QString::number(path_->getDistance(), 'f', 1);
				break;
			case 11:
				ret = QString::number(path_->getTime(), 'f', 1);
				break;
			}
		}
	}

	return ret;
}

bool PathParamTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  	(void)role ;
	if (index.isValid())
	{
		assert(index.column() == 1);

		if (!value.canConvert<double>())
			return false;
		double v = value.toDouble();

		std::shared_ptr<PathParameterChangeUndo> undo = std::make_shared<PathParameterChangeUndo>(*this, pathfile_model_, path_);
		UndoManager::getUndoManager().pushUndoStack(undo);

		switch (index.row())
		{
		case 0:
			path_->setStartVelocity(v);
			break;
		case 1:
			path_->setEndVelocity(v);
			break;
		case 2:
			path_->setMaxVelocity(v);
			break;
		case 3:
			path_->setMaxAccel(v);
			break;
		case 4:
			path_->setMaxJerk(v);
			break;
		case 5:
			path_->setMaxCentripetal(v);
			break;
		case 6:
			if (angles_required_)
				path_->setStartAngle(v);
			break;
		case 7:
			if (angles_required_)
				path_->setStartAngleDelay(v);
			break;
		case 8:
			path_->setEndAngle(v);
			break;
		case 9:
			path_->setEndAngleDelay(v);
			break;
		}

		emit dataChanged(index, index);
	}
	return true;
}

void PathParamTreeModel::updated()
{
	QModelIndex index;
	reset();
	emit dataChanged(index, index);
}

void PathParamTreeModel::reset()
{
	beginResetModel();
	endResetModel();
}
