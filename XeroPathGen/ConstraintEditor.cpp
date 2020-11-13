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
#include <QMenu>

using namespace xero::paths;

ConstraintEditor::ConstraintEditor(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	ui.tree_->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tree_->setModel(&model_);
	ui.tree_->setToolTip(tr("No path selected"));

	(void)connect(&model_, &ConstraintTreeModel::constraintChanged, this, &ConstraintEditor::emitConstraintChanged);
	(void)connect(ui.tree_, &QTreeView::customContextMenuRequested, this, &ConstraintEditor::contextMenu);

	constraint_selected_menu_ = nullptr;
	none_selected_menu_ = nullptr;
}

ConstraintEditor::~ConstraintEditor()
{
}

void ConstraintEditor::contextMenu(const QPoint& pt)
{
	QAction* action;

	QModelIndex index = ui.tree_->indexAt(pt);
	if (!index.isValid())
	{
		if (none_selected_menu_ == nullptr)
		{
			none_selected_menu_ = new QMenu();
			action = none_selected_menu_->addAction(tr("Add Constraint"));
			(void)connect(action, &QAction::triggered, this, &ConstraintEditor::addOne);
		}
		none_selected_menu_->exec(mapToGlobal(pt));
	}
	else
	{
		row_ = index.row();
		if (constraint_selected_menu_ == nullptr)
		{
			constraint_selected_menu_ = new QMenu();
			action = constraint_selected_menu_->addAction(tr("Delete Constraint"));
			(void)connect(action, &QAction::triggered, this, &ConstraintEditor::deleteIDConstraint);
		}
		constraint_selected_menu_->exec(mapToGlobal(pt));
	}
}

void ConstraintEditor::deleteIDConstraint()
{
	assert(row_ != -1);
	deleteConstraint(row_);
}


void ConstraintEditor::enableDisable()
{
	if (path_ == nullptr || generator_ == nullptr)	{
		ui.tree_->setDisabled(true);
		ui.tree_->setToolTip(tr("No path selected - cannot edit constraints"));
	}
	else if (!generator_->hasProperty(Generator::DistanceContraintPropertyName))
	{
		ui.tree_->setToolTip(tr("The current generator does not support constraints"));
	}
	else
	{
		ui.tree_->setToolTip(tr("Use the right mouse button to add/delete constraints"));
		ui.tree_->setDisabled(false);
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
