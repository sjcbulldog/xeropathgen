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
#include "FlagsTreeModel.h"
#include "UndoManager.h"
#include "FlagChangedUndo.h"
#include <QModelIndex>
#include <QDebug>
#include <QColor>

using namespace xero::paths;

FlagsTreeModel::FlagsTreeModel()
{
}

FlagsTreeModel::~FlagsTreeModel()
{
}

QModelIndex FlagsTreeModel::index(int row, int col, const QModelIndex& parent) const
{
	(void)parent;
	return createIndex(row, col);
}

QModelIndex FlagsTreeModel::parent(const QModelIndex& index) const
{
	(void)index;
	return QModelIndex();
}

Qt::ItemFlags FlagsTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;

	return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

int FlagsTreeModel::rowCount(const QModelIndex& parent) const
{
	if (path_ == nullptr)
		return 0;

	if (parent.isValid())
		return 0;

	return static_cast<int>(path_->getFlags().size());
}

int FlagsTreeModel::columnCount(const QModelIndex& parent) const
{
	(void)parent;
	return 3;
}

QVariant FlagsTreeModel::headerData(int section, Qt::Orientation orient, int role) const
{
	QVariant ret;

	if (role == Qt::DisplayRole && orient == Qt::Orientation::Horizontal)
	{
		switch (section)
		{
		case 0:
			ret = "Name";
			break;

		case 1:
			ret = "After (distance)";
			break;

		case 2:
			ret = "Before (distance)";
			break;
		}
	}
	return ret;
}

bool FlagsTreeModel::isRowValid(int row) const
{
	auto flag = path_->getFlags()[row];

	if (flag == nullptr)
		return false;

	if (flag->after() >= flag->before())
		return false;

	return true;
}

QVariant FlagsTreeModel::data(const QModelIndex& index, int role) const
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
		// This is the flag
		//
		auto flag = path_->getFlags()[index.row()];

		if (flag != nullptr)
		{
			switch (index.column())
			{
			case 0:
				ret = flag->name().c_str();
				break;

			case 1:
				ret = QString::number(flag->after(), 'f', 1);
				break;

			case 2:
				ret = QString::number(flag->before(), 'f', 1);
				break;

			}
		}
	}
	return ret;
}

bool FlagsTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	(void)role;

	//
	// This is the flag
	//
	auto flag = path_->getFlags()[index.row()];
	std::shared_ptr<FlagChangedUndo> undo;

	if (flag != nullptr)
	{
		switch (index.column())
		{
		case 0:
			undo = std::make_shared<FlagChangedUndo>(*this, index.row(), flag->name().c_str());
			flag->setName(value.toString().toStdString());
			emitFlagChanged();
			break;

		case 1:
			undo = std::make_shared<FlagChangedUndo>(*this, index.row(), FlagChangedUndo::WhichValue::After, flag->after());
			flag->setAfter(value.toDouble());
			emitFlagChanged();
			break;

		case 2:
			undo = std::make_shared<FlagChangedUndo>(*this, index.row(), FlagChangedUndo::WhichValue::Before, flag->before());
			flag->setBefore(value.toDouble());
			emitFlagChanged();
			break;

		}
	}

	UndoManager::getUndoManager().pushUndoStack(undo);

	return true;
}

void FlagsTreeModel::changeBefore(int row, double value)
{
	std::shared_ptr<PathFlag> flag = path_->getFlags()[row];
	flag->setBefore(value);
	emitFlagChanged();
	reset();
}

void FlagsTreeModel::changeAfter(int row, double value)
{
	std::shared_ptr<PathFlag> flag = path_->getFlags()[row];
	flag->setAfter(value);
	emitFlagChanged();
	reset();
}

void FlagsTreeModel::changeName(int row, const std::string &name)
{
	std::shared_ptr<PathFlag> flag = path_->getFlags()[row];
	flag->setName(name);
	emitFlagChanged();
	reset();
}

void FlagsTreeModel::reset()
{
	beginResetModel();
	endResetModel();
}

void FlagsTreeModel::emitFlagChanged()
{
	emit flagChanged();
}
