#include "SelectRobotDialog.h"
#include <QAction>

using namespace xero::paths;

SelectRobotDialog::SelectRobotDialog(RobotManager &mgr, std::shared_ptr<RobotParams> current, QWidget *parent) : QDialog(parent), mgr_(mgr)
{
	ui.setupUi(this);
	ui.robots_->setModel(&model_);

	connect(ui.buttons_, &QDialogButtonBox::accepted, this, &SelectRobotDialog::accept);
	connect(ui.buttons_, &QDialogButtonBox::rejected, this, &SelectRobotDialog::reject);

	for (auto robot : mgr_.getRobots())
	{
		if (robot != current)
		{
			QList<QStandardItem*> item =
			{
				new QStandardItem(robot->getName().c_str())
			};
			model_.appendRow(item);
		}
	}
}

SelectRobotDialog::~SelectRobotDialog()
{
}

QString SelectRobotDialog::getSelectedRobot()
{
	QString ret;

	QModelIndex index = ui.robots_->currentIndex();
	if (index.isValid())
	{
		ret = model_.data(index, Qt::DisplayRole).toString();
	}

	return ret;
}
