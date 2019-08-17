#include "ConstraintEditor.h"
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

	emitConstraintAdded();
}

void ConstraintEditor::deleteOne()
{
	QModelIndex index = ui.tree_->currentIndex();
	path_->removeTimingConstraint(index.row());
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
