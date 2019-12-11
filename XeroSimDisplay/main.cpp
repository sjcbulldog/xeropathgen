#include "XeroSimDisplay.h"
#include "GameFieldManager.h"
#include <QtWidgets/QApplication>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("ErrorCodeXero");
	QCoreApplication::setOrganizationDomain("www.wilsonvillerobotics.com");
	QCoreApplication::setApplicationName("XeroPathGenerator");
	QCoreApplication::setApplicationVersion("1.0.0");

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);


	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
	QString appdir = dirs.front();

	GameFieldManager fields;
	fields.addDefaultDirectory((appdir + "/fields").toStdString());

	XeroSimDisplay w(fields);
	w.show();
	return a.exec();
}
