#include "XeroPathGenerator.h"
#include "GameFieldManager.h"
#include "GeneratorManager.h"
#include "RobotManager.h"
#include <QtWidgets/QApplication>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <fstream>
#include <sstream>

std::ofstream logfilestream;
std::stringstream log2stream;

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	const char* file = context.file ? context.file : "";
	const char* function = context.function ? context.function : "";
	switch (type) {
	case QtDebugMsg:
		logfilestream << "Debug: ";
		log2stream << "Debug: ";
		break;
	case QtInfoMsg:
		logfilestream << "Info: ";
		log2stream << "Info: ";
		break;
	case QtWarningMsg:
		logfilestream << "Warning: ";
		log2stream << "Warning: ";
		break;
	case QtCriticalMsg:
		logfilestream << "Critical: ";
		log2stream << "Critical: ";
		break;
	case QtFatalMsg:
		logfilestream << "Error: ";
		log2stream << "Error: ";
		break;
	}
	logfilestream << localMsg.constData() << "(" << file << ":" << context.line << ", " << function << std::endl;
	log2stream << localMsg.constData() << std::endl;
}

static bool createLogFile()
{
	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
	QString dir = dirs.front();
	if (dir.length() == 0)
		return false;

	dir += "/logs";
	if (!QDir(dir).exists())
	{
		if (!QDir().mkpath(dir))
			return false;
	}

	QDateTime now = QDateTime::currentDateTime();
	QString log = dir + "/" + now.toString("dd_MM_yyyy_hh_mm_ss_zzz.'log'");
	logfilestream.open(log.toStdString());
	return true;
}

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("ErrorCodeXero");
	QCoreApplication::setOrganizationDomain("www.wilsonvillerobotics.com");
	QCoreApplication::setApplicationName("XeroPathGenerator");
	QCoreApplication::setApplicationVersion("1.0.0");

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);
	std::string error;

	if (createLogFile())
	{
		QDateTime now = QDateTime::currentDateTime();
		qInstallMessageHandler(myMessageOutput);
		qDebug() << "Starting Xero Path Generator, " << now.toString("dd/MM/yyyy @ hh:mm:ss");
	}

	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
	QString appdir = dirs.front();

	GameFieldManager fields;
	GeneratorManager generators;
	RobotManager robots;

	argc--;
	argv++;

	while (argc-- > 0)
	{
		std::string arg = *argv++;
		if (arg == "--fields")
		{
			if (argc == 0)
			{
				QMessageBox box(QMessageBox::Icon::Critical,
					"Error", "--fields flag requires path argument", QMessageBox::StandardButton::Ok);
				box.exec();
				return -1;
			}

			fields.setDirectory(*argv++);
			argc--;
		}
		else if (arg == "--generators")
		{
			if (argc == 0)
			{
				QMessageBox box(QMessageBox::Icon::Critical,
					"Error", "--generators flag requires path argument", QMessageBox::StandardButton::Ok);
				box.exec();
				return -1;
			}

			generators.setDirectory(*argv++);
			argc--;
		}
		else if (arg == "--robots")
		{
			if (argc == 0)
			{
				QMessageBox box(QMessageBox::Icon::Critical,
					"Error", "--robots flag requires path argument", QMessageBox::StandardButton::Ok);
				box.exec();
				return -1;
			}

			robots.setDirectory(*argv++);
			argc--;
		}
	}

	fields.addDirectory((appdir + "/fields").toStdString());
	generators.addDirectory((appdir + "/generators").toStdString());
	robots.addDirectory((appdir + "/robots").toStdString());

	if (!fields.initialize())
	{
		QMessageBox box(QMessageBox::Icon::Critical, "Error", 
					"Could not initialize the game field manager", QMessageBox::StandardButton::Ok);
		box.exec();
		return -1;
	}

	if (!generators.initialize())
	{
		QMessageBox box(QMessageBox::Icon::Critical, "Error", 
					"Could not initialize the generator manager", QMessageBox::StandardButton::Ok);
		box.exec();
		return -1;
	}

	if (!robots.initialize())
	{
		QMessageBox box(QMessageBox::Icon::Critical, "Error",
			"Could not initialize the robots manager", QMessageBox::StandardButton::Ok);
		box.exec();
		return -1;
	}

	try {
		XeroPathGenerator w(fields, generators, robots, logfilestream, log2stream);
		w.show();
		return a.exec();
	}
	catch(...)
	{
		return -1;
	}

	return 0;
}
