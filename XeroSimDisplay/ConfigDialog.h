#pragma once

#include <QDialog>
#include "ui_ConfigDialog.h"

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog(const QString& srcdir, const QString& robot, const QString& stim, double au, double op, double ts, QWidget *parent = Q_NULLPTR);
	~ConfigDialog();

	QString getSrcDir() {
		return ui.src_dir_->text();
	}

	QString getRobot() {
		return ui.robot_->text();
	}

	QString getStimulusFile() {
		return ui.stim_->text();
	}

	double getAuto() {
		return ui.auto_->text().toDouble();
	}

	double getOper() {
		return ui.oper_->text().toDouble();
	}

	double getTest() {
		return ui.test_->text().toDouble();
	}

private:
	void browseSrcDir();

private:
	Ui::ConfigDialog ui;
};
