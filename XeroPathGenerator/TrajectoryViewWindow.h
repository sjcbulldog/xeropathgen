#pragma once

#include "PathTrajectory.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QTableWidget>

class TrajectoryViewWindow : public QDialog
{
	Q_OBJECT

public:
	TrajectoryViewWindow(QWidget *parent = Q_NULLPTR);
	~TrajectoryViewWindow();

	void setTrajectory(std::shared_ptr<xero::paths::PathTrajectory> traj);

private:
	QVBoxLayout* layout_;
	QTableWidget* table_;
	std::shared_ptr<xero::paths::PathTrajectory> traj_;
};
