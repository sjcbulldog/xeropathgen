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
#include "ConstraintTreeModel.h"
#include "UndoManager.h"
#include "ChangeContraintUndo.h"
#include <DistanceVelocityConstraint.h>
#include <QModelIndex>
#include <QDebug>
#include <QColor>

using namespace xero::paths;

ConstraintTreeModel::ConstraintTreeModel()
{
}

ConstraintTreeModel::~ConstraintTreeModel()
{
}

QModelIndex ConstraintTreeModel::index(int row, int col, const QModelIndex& parent) const
{
	(void)parent ;
	return createIndex(row, col);
}

QModelIndex ConstraintTreeModel::parent(const QModelIndex& index) const
{
  	(void)index ;
	return QModelIndex();
}

Qt::ItemFlags ConstraintTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;

	return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

int ConstraintTreeModel::rowCount(const QModelIndex& parent) const
{
	if (path_ == nullptr)
		return 0;

	if (!generator_->hasProperty(Generator::DistanceContraintPropertyName))
		return 0;
	
	if (parent.isValid())
		return 0;

	return static_cast<int>(path_->getConstraints().size());
}

int ConstraintTreeModel::columnCount(const QModelIndex& parent) const
{
  	(void)parent ;
	return 3;
}

QVariant ConstraintTreeModel::headerData(int section, Qt::Orientation orient, int role) const
{
	QVariant ret;

	if (role == Qt::DisplayRole && orient == Qt::Orientation::Horizontal)
	{
		switch (section)
		{
		case 0:
			ret = "After (distance)";
			break;

		case 1:
			ret = "Before (distance)";
			break;

		case 2:
			ret = "Velocity";
			break;
		}
	}
	return ret;
}

bool ConstraintTreeModel::isRowValid(int row) const
{
	std::shared_ptr<PathConstraint> con;
	con = path_->getConstraints()[row];
	std::shared_ptr<DistanceVelocityConstraint> dist = std::dynamic_pointer_cast<DistanceVelocityConstraint>(con);

	if (dist->getAfter() >= dist->getBefore())
		return false;

	if (dist->getVelocity() <= 0.0)
		return false;

	return true;
}

QVariant ConstraintTreeModel::data(const QModelIndex& index, int role) const
{
	QVariant ret;

	if (path_ == nullptr)
		return ret;

	if (role == Qt::ForegroundRole)
	{
		if (!isRowValid(index.row()))
			ret = QVariant(QColor(0xff, 0x00, 0x00, 0xFF));
	}
	else if (role == Qt::EditRole || role == Qt::DisplayRole)
	{
		//
		// This is the constraint
		//
		std::shared_ptr<PathConstraint> con;
		con = path_->getConstraints()[index.row()];
		std::shared_ptr<DistanceVelocityConstraint> dist = std::dynamic_pointer_cast<DistanceVelocityConstraint>(con);

		if (dist != nullptr)
		{
			switch (index.column())
			{
			case 0:
				ret = QString::number(dist->getAfter(), 'f', 1);
				break;

			case 1:
				ret = QString::number(dist->getBefore(), 'f', 1);
				break;

			case 2:
				ret = QString::number(dist->getVelocity(), 'f', 1);
				break;
			}
		}
	}
	return ret;
}

bool ConstraintTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  	(void)role ;


	//
	// This is the constraint
	//
	std::shared_ptr<PathConstraint> con;
	con = path_->getConstraints()[index.row()];
	std::shared_ptr<DistanceVelocityConstraint> dist = std::dynamic_pointer_cast<DistanceVelocityConstraint>(con);
	std::shared_ptr<ChangeContraintUndo> undo;

	if (dist != nullptr)
	{
		switch (index.column())
		{
		case 0:
			undo = std::make_shared<ChangeContraintUndo>(*this, index.row(), ChangeContraintUndo::WhichValue::After, dist->getAfter());
			dist->setAfter(value.toDouble());
			emitConstraintChanged();
			break;

		case 1:
			undo = std::make_shared<ChangeContraintUndo>(*this, index.row(), ChangeContraintUndo::WhichValue::Before, dist->getBefore());
			dist->setBefore(value.toDouble());
			emitConstraintChanged();
			break;

		case 2:
			undo = std::make_shared<ChangeContraintUndo>(*this, index.row(), ChangeContraintUndo::WhichValue::Velocity, dist->getVelocity());
			dist->setVelocity(value.toDouble());
			emitConstraintChanged();
			break;
		}
	}

	UndoManager::getUndoManager().pushUndoStack(undo);

	return true;
}

void ConstraintTreeModel::changeBefore(int row, double value)
{
	std::shared_ptr<PathConstraint> con;
	con = path_->getConstraints()[row];
	std::shared_ptr<DistanceVelocityConstraint> dist = std::dynamic_pointer_cast<DistanceVelocityConstraint>(con);
	dist->setBefore(value);
	emitConstraintChanged();
	reset();
}

void ConstraintTreeModel::changeAfter(int row, double value)
{
	std::shared_ptr<PathConstraint> con;
	con = path_->getConstraints()[row];
	std::shared_ptr<DistanceVelocityConstraint> dist = std::dynamic_pointer_cast<DistanceVelocityConstraint>(con);
	dist->setAfter(value);
	emitConstraintChanged();
	reset();
}

void ConstraintTreeModel::changeVelocity(int row, double value)
{
	std::shared_ptr<PathConstraint> con;
	con = path_->getConstraints()[row];
	std::shared_ptr<DistanceVelocityConstraint> dist = std::dynamic_pointer_cast<DistanceVelocityConstraint>(con);
	dist->setVelocity(value);
	emitConstraintChanged();
	reset();
}


void ConstraintTreeModel::reset()
{
	beginResetModel();
	endResetModel();
}

void ConstraintTreeModel::emitConstraintChanged()
{
	emit constraintChanged();
}
