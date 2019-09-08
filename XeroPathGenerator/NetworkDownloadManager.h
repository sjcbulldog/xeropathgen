#pragma once

#include "GameFieldManager.h"
#include "GeneratorManager.h"
#include <QString>
#include <QStringList>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>

class NetworkDownloadManager : public QObject
{
	Q_OBJECT;

public:
	NetworkDownloadManager(GameFieldManager &games, GeneratorManager &generators);
	virtual ~NetworkDownloadManager();

	void setSite(QString site) {
		site_ = site;
	}

	void setLocation(QString loc) {
		location_ = loc;
	}

	QString site() const {
		return site_;
	}

	QString location() const {
		return location_;
	}

	bool newFields() {
		bool b = new_fields_;
		new_fields_ = false;

		return b;
	}

	void updateFields();

	void tick();

private:
	enum class State
	{
		Idle,
		FetchingManifest,
		StartDownload,
		Downloading,
		ErrorDetected,
		Complete,
	};

private:
	QUrl createURL(QString item);
	void downloadManifest();
	void processManifestReply();
	void startDownload();
	void finishDownload();

private:
	bool connected_;
	QString site_;
	QString location_;
	QNetworkAccessManager access_;
	QNetworkReply* reply_;
	State state_;
	bool new_fields_;

	QByteArray data_;
	QNetworkReply::NetworkError error_;
	QString error_str_;

	std::list<std::pair<QUrl, QString>> downloads_;

	GameFieldManager& fields_;
	GeneratorManager& generators_;
};

