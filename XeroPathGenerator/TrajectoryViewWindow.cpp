#include "TrajectoryViewWindow.h"

using namespace xero::paths;

TrajectoryViewWindow::TrajectoryViewWindow(QWidget *parent) : QDialog(parent)
{
	layout_ = new QVBoxLayout();
	setLayout(layout_);
	table_ = new QTableWidget(0, 8, this);
}

TrajectoryViewWindow::~TrajectoryViewWindow()
{
}

void TrajectoryViewWindow::setTrajectory(std::shared_ptr<PathTrajectory> traj)
{
	int row = 0;
	traj_ = traj;

	for (const auto& pt : (*traj))
	{
		QTableWidgetItem* item = new QTableWidgetItem(QString::number(pt.time(), 'f', 2));
		table_->setItem(row, 0, item);
	}
}
