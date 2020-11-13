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

#include "PropertyEditorTreeModel.h"
#include <QDialog>
#include "ui_PropertyEditor.h"

class PropertyEditor : public QDialog
{
	Q_OBJECT

public:
	PropertyEditor(QString title, QWidget *parent = Q_NULLPTR);
	~PropertyEditor();

	PropertyEditorTreeModel& getModel() {
		return model_;
	}

protected:

private:
	void okPushed();
	void cancelPushed();

private:
	Ui::PropertyEditor ui;
	PropertyEditorTreeModel model_;
};
