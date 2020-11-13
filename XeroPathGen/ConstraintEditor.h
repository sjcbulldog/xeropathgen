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

#include <QWidget>
#include "ui_ConstraintEditor.h"
#include "ConstraintTreeModel.h"
#include <DistanceVelocityConstraint.h>

class ConstraintEditor : public QWidget
{
	Q_OBJECT

public:
	ConstraintEditor(QWidget *parent = Q_NULLPTR);
	~ConstraintEditor();

	void setPath(std::shared_ptr<xero::paths::RobotPath> path) {
		model_.setPath(path);
		path_ = path;
		enableDisable();
	}

	void setGenerator(std::shared_ptr<Generator> gen) {
		model_.setGenerator(gen);
		generator_ = gen;
		enableDisable();
	}

	void addConstraint(int row, const xero::paths::DistanceVelocityConstraint& c);
	void deleteConstraint(int row);

signals:
	void constraintAdded();
	void constraintRemoved();
	void constraintChanged();

private:
	void emitConstraintAdded();
	void emitConstraintRemoved();
	void emitConstraintChanged();

private:
	void addOne();
	void deleteOne();
	void enableDisable();
	void contextMenu(const QPoint& pt);
	void deleteIDConstraint();

private:
	Ui::ConstraintEditor ui;
	ConstraintTreeModel model_;
	std::shared_ptr<Generator> generator_;
	std::shared_ptr<xero::paths::RobotPath> path_;
	QMenu* constraint_selected_menu_;
	QMenu* none_selected_menu_;
	int row_;
};
