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
#include "ConstraintEditor.h"
#include "UndoManager.h"
#include "ConstraintDeleteUndo.h"
#include "ConstraintAddUndo.h"
#include <DistanceVelocityConstraint.h>

using namespace xero::paths;

ConstraintEditor::ConstraintEditor(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	ui.tree_->setModel(&model_);

	connect(ui.add_, &QAbstractButton::clicked, this, &ConstraintEditor::addOne);
	connect(ui.delete_, &QAbstractButton::clicked, this, &ConstraintEditor::deleteOne);
	connect(&model_, &ConstraintTreeModel::constraintChanged, this, &ConstraintEditor::emitConstraintChanged);
}

ConstraintEditor::~ConstraintEditor()
{
}

void ConstraintEditor::enableDisable()
{
	if (path_ == nullptr || generator_ == nullptr || !generator_->hasProperty(Generator::DistanceContraintPropertyName))
	{
		ui.tree_->setDisabled(true);
		ui.add_->setDisabled(true);
		ui.delete_->setDisabled(true);
	}
	else
	{
		ui.tree_->setDisabled(false);
		ui.add_->setDisabled(false);
		ui.delete_->setDisabled(false);
	}
}

void ConstraintEditor::addOne()
{
	std::shared_ptr<DistanceVelocityConstraint> c = std::make_shared<DistanceVelocityConstraint>(0.0, 0.0, 0.0);
	path_->addTimingConstraint(c);
	model_.reset();

	int row = static_cast<int>(path_->getConstraints().size() - 1);
	UndoManager& mgr = UndoManager::getUndoManager();
	std::shared_ptr<ConstraintAddUndo> item = std::make_shared<ConstraintAddUndo>(*this, row);
	mgr.pushUndoStack(item);
	
	emitConstraintAdded();
}

void ConstraintEditor::addConstraint(int row, const DistanceVelocityConstraint& c)
{
	std::shared_ptr<DistanceVelocityConstraint> nc = std::make_shared<DistanceVelocityConstraint>(c);
	path_->insertTimingConstraint(row, nc);
	model_.reset();

	emitConstraintAdded();
}

void ConstraintEditor::deleteOne()
{
	QModelIndex index = ui.tree_->currentIndex();
	auto c = path_->getConstraints()[index.row()];
	std::shared_ptr<DistanceVelocityConstraint> dc = std::dynamic_pointer_cast<DistanceVelocityConstraint>(c);

	UndoManager& mgr = UndoManager::getUndoManager();
	std::shared_ptr<ConstraintDeleteUndo> item = std::make_shared<ConstraintDeleteUndo>(*this, index.row(), *dc);
	mgr.pushUndoStack(item);

	path_->removeTimingConstraint(index.row());
	model_.reset();
	emitConstraintRemoved();
}

void ConstraintEditor::deleteConstraint(int row)
{
	path_->removeTimingConstraint(row);
	model_.reset();
	emitConstraintRemoved();
}

void ConstraintEditor::emitConstraintAdded()
{
	emit constraintAdded();
}
void ConstraintEditor::emitConstraintRemoved()
{
	emit constraintRemoved();
}
void ConstraintEditor::emitConstraintChanged()
{
	emit constraintChanged();
}
