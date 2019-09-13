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
#include "PropertyEditor.h"
#include "PropertyEditorItemDelegate.h"

PropertyEditor::PropertyEditor(QString title, QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle(title);

	ui.tree_->setModel(&model_);
	ui.tree_->setItemDelegate(new PropertyEditorItemDelegate(model_, ui.tree_));
	ui.tree_->setWordWrap(true);

	ui.tree_->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui.tree_->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	ui.tree_->header()->setSectionResizeMode(2, QHeaderView::Stretch);

	connect(ui.buttons_, &QDialogButtonBox::accepted, this, &PropertyEditor::accept);
	connect(ui.buttons_, &QDialogButtonBox::rejected, this, &PropertyEditor::reject);
}

PropertyEditor::~PropertyEditor()
{
}

