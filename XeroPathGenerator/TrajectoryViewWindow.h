#pragma once

#include "RobotPath.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QComboBox>

class TrajectoryViewWindow : public QDialog
{
	Q_OBJECT

public:
	TrajectoryViewWindow(QWidget *parent = Q_NULLPTR);
	~TrajectoryViewWindow();

	void setPath(std::shared_ptr<xero::paths::RobotPath> path);
	void setCurrentTime(double t);

private:
	void updateTrajectory(const QString &text);

private:
	QVBoxLayout* layout_;
	QComboBox* box_;
	QTableWidget* table_;
	std::shared_ptr<xero::paths::RobotPath> path_;
	std::string which_;
};
