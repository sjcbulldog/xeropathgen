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

