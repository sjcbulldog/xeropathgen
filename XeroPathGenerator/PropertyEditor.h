#pragma once

#include "PropertyEditorTreeModel.h"
#include <QDialog>
#include "ui_PropertyEditor.h"

class PropertyEditor : public QDialog
{
	Q_OBJECT

public:
	PropertyEditor(QWidget *parent = Q_NULLPTR);
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
