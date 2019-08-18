#include "PropertyEditor.h"
#include "PropertyEditorItemDelegate.h"

PropertyEditor::PropertyEditor(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	ui.tree_->setModel(&model_);
	ui.tree_->setItemDelegate(new PropertyEditorItemDelegate(model_, ui.tree_));
	ui.tree_->setWordWrap(true);

	connect(ui.buttons_, &QDialogButtonBox::accepted, this, &PropertyEditor::accept);
	connect(ui.buttons_, &QDialogButtonBox::rejected, this, &PropertyEditor::reject);
}

PropertyEditor::~PropertyEditor()
{
}

