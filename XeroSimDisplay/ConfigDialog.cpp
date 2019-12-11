#include "ConfigDialog.h"
#include <QDialogButtonBox>
#include <QFileDialog>

ConfigDialog::ConfigDialog(const QString &srcdir, const QString &robot, const QString &stim, double au, double op, double ts, QWidget* parent): QDialog(parent)
{
	ui.setupUi(this);

	ui.src_dir_->setText(srcdir);
	ui.robot_->setText(robot);
	ui.stim_->setText(stim);
	ui.auto_->setText(QString::number(au));
	ui.oper_->setText(QString::number(op));
	ui.test_->setText(QString::number(ts));

	(void)connect(ui.buttons_, &QDialogButtonBox::accepted, this, &ConfigDialog::accept);
	(void)connect(ui.buttons_, &QDialogButtonBox::rejected, this, &ConfigDialog::reject);
	(void)connect(ui.simulator_button_, &QPushButton::pressed, this, &ConfigDialog::browseSrcDir);
}

ConfigDialog::~ConfigDialog()
{
}

void ConfigDialog::browseSrcDir()
{
	QString filename = QFileDialog::getExistingDirectory(this, "Source Tree Directory");
	ui.src_dir_->setText(filename);
}
