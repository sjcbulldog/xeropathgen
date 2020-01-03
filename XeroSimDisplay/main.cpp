#include "XeroSimDisplay.h"
#include "GameFieldManager.h"
#include <QtWidgets/QApplication>
#include <QStandardPaths>
#include <QMessageBox>

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("ErrorCodeXero");
	QCoreApplication::setOrganizationDomain("www.wilsonvillerobotics.com");
	QCoreApplication::setApplicationName("XeroSimDisplay");
	QCoreApplication::setApplicationVersion("1.0.0");

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);

	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
	QString appdir = dirs.front();

	//
	// We want to use the fields from the path generator program
	//
	QString pathdir = appdir.replace("XeroSimDisplay", "XeroPathGenerator");

	GameFieldManager fields;
	fields.addDefaultDirectory((pathdir + "/fields").toStdString());
	fields.copyDefaults("fields");
	if (!fields.initialize())
	{
		QMessageBox box(QMessageBox::Icon::Critical, "Error",
			"Could not initialize the game field manager", QMessageBox::StandardButton::Ok);
		box.exec();
		return -1;
	}

	XeroSimDisplay w(fields);
	w.show();
	return a.exec();
}
