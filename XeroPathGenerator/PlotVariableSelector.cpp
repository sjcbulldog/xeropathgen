#include "PlotVariableSelector.h"
#include "PathPlotView.h"
#include "DriveBaseData.h"
#include <RobotParams.h>
#include <QAction>

using namespace xero::paths;

PlotVariableSelector::PlotVariableSelector(RobotParams::DriveType type, QList<QString> selected, QWidget *parent) : QDialog(parent)
{
	std::vector<PathPlotView::VarType> vtypes =
	{
		PathPlotView::VarType::VTPosition,
		PathPlotView::VarType::VTVelocity,
		PathPlotView::VarType::VTAcceleration,
		PathPlotView::VarType::VTJerk,
	};
	ui.setupUi(this);

	(void)connect(ui.buttons_, &QDialogButtonBox::accepted, this, &PlotVariableSelector::accept);
	(void)connect(ui.buttons_, &QDialogButtonBox::rejected, this, &PlotVariableSelector::reject);

	ui.tree_->setColumnCount(3);
	QList<QString> hlist;
	hlist.push_back("Selected");
	hlist.push_back("Trajectory");
	hlist.push_back("Variable");
	ui.tree_->setHeaderLabels(hlist);

	const std::vector<std::string>& trajnames = DriveBaseData::getTrajectories(type);
	for (const std::string& name : trajnames)
	{
		for (PathPlotView::VarType vtype : vtypes)
		{
			QString qname = name.c_str();
			qname += ",";
			qname += PathPlotView::typeToName(vtype).c_str();

			QStringList list;
			list.push_back("");
			list.push_back(name.c_str());
			list.push_back(PathPlotView::typeToName(vtype).c_str());
			QTreeWidgetItem* itemp = new QTreeWidgetItem(ui.tree_, list);
			ui.tree_->addTopLevelItem(itemp);

			QString item(QString(name.c_str()) + "," + QString(PathPlotView::typeToName(vtype).c_str()));
			if (selected.contains(item))
				itemp->setCheckState(0, Qt::CheckState::Checked);
			else
				itemp->setCheckState(0, Qt::CheckState::Unchecked);
		}
	}

	ui.tree_->resizeColumnToContents(0);
	ui.tree_->resizeColumnToContents(1);
	ui.tree_->resizeColumnToContents(2);
}

PlotVariableSelector::~PlotVariableSelector()
{
}

QList<QString> PlotVariableSelector::getPlotVariables()
{
	QList<QString> list;

	for (int i = 0; i < ui.tree_->topLevelItemCount() ; i++)
	{
		QTreeWidgetItem* item = ui.tree_->topLevelItem(i);
		if (item->checkState(0) == Qt::CheckState::Checked)
		{
			list.push_back(item->text(1) + "," + item->text(2));
		}
	}

	return list;
}
