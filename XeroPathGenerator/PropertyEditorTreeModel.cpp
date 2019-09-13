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
#include "PropertyEditorTreeModel.h"
#include "EditableProperty.h"

PropertyEditorTreeModel::PropertyEditorTreeModel()
{
}

PropertyEditorTreeModel::~PropertyEditorTreeModel()
{
}

std::shared_ptr<EditableProperty> PropertyEditorTreeModel::getProperty(const QModelIndex& index)
{
	if (!index.isValid())
		return nullptr;

	if (index.row() >= static_cast<int>(props_.size()))
		return nullptr;

	return props_[index.row()];
}

std::shared_ptr<EditableProperty> PropertyEditorTreeModel::getProperty(QString name)
{
	for (auto prop : props_)
	{
		if (prop->getName() == name)
			return prop;
	}

	return nullptr;
}

QModelIndex PropertyEditorTreeModel::index(int row, int col, const QModelIndex& parent) const
{
  	(void)parent ;
	return createIndex(row, col);
}

QModelIndex PropertyEditorTreeModel::parent(const QModelIndex& index) const
{
  	(void)index ;
	return QModelIndex();
}

Qt::ItemFlags PropertyEditorTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid() || index.column() != 1 || props_[index.row()]->isReadOnly())
		return Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;

	return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

int PropertyEditorTreeModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return static_cast<int>(props_.size());
}

int PropertyEditorTreeModel::columnCount(const QModelIndex& parent) const
{
  	(void)parent ;
	return 3;
}

QVariant PropertyEditorTreeModel::headerData(int section, Qt::Orientation orient, int role) const
{
	QVariant ret;

	if (role == Qt::DisplayRole && orient == Qt::Orientation::Horizontal)
	{
		if (section == 0)
			ret = "Name";
		else if (section == 1)
			ret = "Value";
		else
			ret = "Description";
	}

	return ret;
}

QVariant PropertyEditorTreeModel::data(const QModelIndex& index, int role) const
{
	QVariant ret;

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		auto prop = props_[index.row()];
		if (index.column() == 0)
		{

			ret = prop->getName();
		}
		else if (index.column() == 1)
		{
			ret = prop->getValue();
			if (prop->getType() == EditableProperty::PropertyType::PTDouble)
			{
				double d = ret.toDouble();
				ret = QString::number(d, 'f', 2);
			}
			else if (prop->getType() == EditableProperty::PropertyType::PTInteger)
			{
				int d = ret.toInt();
				ret = QString::number(d);
			}
		}
		else if (index.column() == 2)
		{
			ret = prop->getDescription();
		}
	}
	else if (role == Qt::WhatsThisRole)
	{
		auto prop = props_[index.row()];
		ret = prop->getDescription();
	}
	return ret;
}

bool PropertyEditorTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  	(void)role ;
	if (index.isValid())
	{
		assert(index.column() == 1);
		auto prop = props_[index.row()];
		prop->setValue(value);
	}
	return true;
}
