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
#include <QString>
#include <QVariant>

class EditableProperty
{
public:
	enum PropertyType
	{
		PTDouble,
		PTString,
		PTStringList,
		PTInteger,
	};

public:
	EditableProperty(QString name, PropertyType type, QVariant value, QString desc, bool ronly = false) {
		name_ = name;
		type_ = type;
		value_ = value;
		description_ = desc;
		readonly_ = ronly;
	}

	QString getName() const {
		return name_;
	}

	PropertyType getType() const {
		return type_;
	}

	QVariant getValue() const {
		return value_;
	}

	void setValue(QVariant v) {
		value_ = v;
	}

	QString getDescription() const {
		return description_;
	}

	bool isReadOnly() const {
		return readonly_;
	}

	void addChoice(QString str) {
		choices_.push_back(str);
	}

	const QList<QString>& getChoices() const {
		return choices_;
	}

private:
	PropertyType type_;
	QString name_;
	QVariant value_;
	QString description_;
	QList<QString> choices_;
	bool readonly_;
};

