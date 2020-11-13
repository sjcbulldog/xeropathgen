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

#include "EditableProperty.h"
#include <Qt>
#include <QAbstractItemModel>
#include <QHash>
#include <QVariant>
#include <memory>
#include <vector>

class PropertyEditorTreeModel : public QAbstractItemModel
{
public:
	PropertyEditorTreeModel();
	virtual ~PropertyEditorTreeModel();

	void addProperty(std::shared_ptr<EditableProperty> prop)  {
		props_.push_back(prop);
	}

	const std::vector<std::shared_ptr<EditableProperty>> & getProperties() const {
		return props_;
	}

	std::shared_ptr<EditableProperty> getProperty(const QModelIndex& index);
	std::shared_ptr<EditableProperty> getProperty(QString name);

	QModelIndex index(int row, int col, const QModelIndex& parent) const;
	QModelIndex parent(const QModelIndex& index) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	int rowCount(const QModelIndex& parent) const;
	int columnCount(const QModelIndex& parent) const;
	QVariant data(const QModelIndex& index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orient, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

private:

private:
	std::vector<std::shared_ptr<EditableProperty>> props_;
};

