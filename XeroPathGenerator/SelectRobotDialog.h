#pragma once

#include "RobotManager.h"
#include <QDialog>
#include <QStandardItemModel>
#include "ui_SelectRobotDialog.h"

class SelectRobotDialog : public QDialog
{
	Q_OBJECT

public:
	SelectRobotDialog(RobotManager &mgr, QWidget *parent = Q_NULLPTR);
	~SelectRobotDialog();

	QString getSelectedRobot();

private:
	Ui::SelectRobotDialog ui;
	QStandardItemModel model_;
	RobotManager& mgr_;
};
