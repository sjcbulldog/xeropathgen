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

#include "FlagsEditor.h"
#include "UndoManager.h"
#include "FlagDeleteUndo.h"
#include "FlagAddUndo.h"
#include "FlagChangedUndo.h"

using namespace xero::paths;

FlagsEditor::FlagsEditor(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	ui.tree_->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tree_->setModel(&model_);
	ui.tree_->setToolTip(tr("No path selected"));

	(void)connect(&model_, &FlagsTreeModel::flagChanged, this, &FlagsEditor::emitFlagChanged);
	(void)connect(ui.tree_, &QTreeView::customContextMenuRequested, this, &FlagsEditor::contextMenu);

	none_selected_menu_ = nullptr;
	flag_selected_menu_ = nullptr;
}

FlagsEditor::~FlagsEditor()
{
}

void FlagsEditor::enableDisable()
{
	if (path_ == nullptr) {
		ui.tree_->setDisabled(true);
		ui.tree_->setToolTip(tr("No path selected - cannot edit flags"));
	}
	else
	{
		ui.tree_->setToolTip(tr("Use the right mouse button to add/delete constraints"));
		ui.tree_->setDisabled(false);
	}
}
void FlagsEditor::contextMenu(const QPoint& pt)
{
	QAction* action;

	QModelIndex index = ui.tree_->indexAt(pt);
	if (!index.isValid())
	{
		if (none_selected_menu_ == nullptr)
		{
			none_selected_menu_ = new QMenu();
			action = none_selected_menu_->addAction(tr("Add Flag"));
			(void)connect(action, &QAction::triggered, this, &FlagsEditor::addOne);
		}
		none_selected_menu_->exec(mapToGlobal(pt));
	}
	else
	{
		row_ = index.row();
		if (flag_selected_menu_ == nullptr)
		{
			flag_selected_menu_ = new QMenu();
			action = flag_selected_menu_->addAction(tr("Delete Flag"));
			(void)connect(action, &QAction::triggered, this, &FlagsEditor::deleteIDFlag);
		}
		flag_selected_menu_->exec(mapToGlobal(pt));
	}
}

void FlagsEditor::insertFlag(int row, const PathFlag& f)
{
	std::shared_ptr<PathFlag> nf = std::make_shared<PathFlag>(f);
	path_->insertFlag(row, nf);
	model_.reset();

	emitFlagAdded();
}

void FlagsEditor::deleteFlag(int row)
{
	path_->removeFlag(row);
	model_.reset();
	emitFlagRemoved();
}

void FlagsEditor::emitFlagAdded()
{
	emit flagAdded();
}
void FlagsEditor::emitFlagRemoved()
{
	emit flagRemoved();
}
void FlagsEditor::emitFlagChanged()
{
	emit flagChanged();
}

void FlagsEditor::deleteOne()
{
	QModelIndex index = ui.tree_->currentIndex();
	auto c = path_->getFlags()[index.row()];
	std::shared_ptr<PathFlag> dc = std::dynamic_pointer_cast<PathFlag>(c);

	UndoManager& mgr = UndoManager::getUndoManager();
	std::shared_ptr<FlagDeleteUndo> item = std::make_shared<FlagDeleteUndo>(*this, index.row(), *dc);
	mgr.pushUndoStack(item);

	path_->removeFlag(index.row());
	model_.reset();
	emitFlagRemoved();
}

std::string FlagsEditor::getNewName()
{
	int i = 1;
	std::string name;

	do
	{
		name = "Flag_" + std::to_string(i++);
	} while (path_->hasFlag(name));

	return name;
}

void FlagsEditor::addOne()
{
	std::shared_ptr<PathFlag> flag = std::make_shared<PathFlag>(getNewName(), 0.0, 0.0);
	path_->addFlag(flag);
	model_.reset();

	int row = static_cast<int>(path_->getConstraints().size() - 1);
	UndoManager& mgr = UndoManager::getUndoManager();
	std::shared_ptr<FlagAddUndo> item = std::make_shared<FlagAddUndo>(*this, row);
	mgr.pushUndoStack(item);

	emitFlagAdded();
}

void FlagsEditor::deleteIDFlag()
{
	assert(row_ != -1);
	deleteFlag(row_);
}