//
// Copyright 2019 Jack W. Griffin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//
#include "NetworkDownloadManager.h"
#include <QMessageBox>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <thread>
#include <array>

NetworkDownloadManager::NetworkDownloadManager(GameFieldManager& games, GeneratorManager& generators) 
	: fields_(games), generators_(generators)
{
	state_ = State::Idle;
	reply_ = nullptr;
	new_fields_ = false;
}

NetworkDownloadManager::~NetworkDownloadManager()
{
}

void NetworkDownloadManager::tick()
{
	switch (state_)
	{
	case State::Idle:
		downloadManifest();
		break;

	case State::FetchingManifest:
		if (reply_->isFinished())
			processManifestReply();
		break;

	case State::StartDownload:
		startDownload();
		break;

	case State::Downloading:
		if (reply_ != nullptr && reply_->isFinished())
			finishDownload();
		break;

	case State::ErrorDetected:
		break;

	case State::Complete:
		break;
	}
}

void NetworkDownloadManager::updateFields()
{
}

QUrl NetworkDownloadManager::createURL(QString item)
{
	QString urlstr = "http://";
	urlstr += site_;
	urlstr += "/";
	urlstr += location_;
	urlstr += "/";
	urlstr += item;

	return QUrl(urlstr);
}

void NetworkDownloadManager::downloadManifest()
{
	QUrl url = createURL("fields.json");
	reply_ = access_.get(QNetworkRequest(url));
	state_ = State::FetchingManifest;
}

void NetworkDownloadManager::processManifestReply()
{
	auto err = reply_->error();
	if (err != QNetworkReply::NetworkError::NoError)
	{
		error_ = err;
		error_str_ = reply_->errorString();
		state_ = State::ErrorDetected;
	}
	else
	{
		//
		// Parse the manifast data
		//
		QByteArray data = reply_->readAll();
		QJsonParseError error;
		QJsonDocument doc = QJsonDocument::fromJson(data, &error);
		if (doc.isNull())
		{
			error_str_ = "Invalid JSON received - bad format";
			state_ = State::ErrorDetected;
		}

		if (!doc.isArray())
		{ 
			error_str_ = "Invalid JSON received - document is not an array";
			state_ = State::ErrorDetected;
		}

		QJsonArray arr = doc.array();
		for (QJsonValue v : arr)
		{
			if (v.isObject())
			{
				QJsonObject obj = v.toObject();
				if (obj["descriptor"].isString() && obj["image"].isString())
				{
					QString descstr = obj["descriptor"].toString();
					QString imagestr = obj["image"].toString();
					if (!fields_.hasFile(descstr))
					{
						QUrl url = createURL(descstr);
						QString dest = QString(fields_.getDefaultDir().c_str()) + "/" + descstr;

						downloads_.push_back(std::make_pair(url, dest));
					}

					if (!fields_.hasFile(imagestr))
					{
						QUrl url = createURL(imagestr);
						QString dest = QString(fields_.getDefaultDir().c_str()) + "/" + imagestr;

						downloads_.push_back(std::make_pair(url, dest));
					}
				}
			}
		}

		if (downloads_.size() == 0)
			state_ = State::Complete;
		else
			state_ = State::StartDownload;
	}
}

void NetworkDownloadManager::startDownload()
{
	auto pair = downloads_.front();

	reply_ = access_.get(QNetworkRequest(pair.first));
	state_ = State::Downloading;
}

void NetworkDownloadManager::finishDownload()
{
	std::pair<QUrl, QString> pair = downloads_.front();
	QFile file = QFile(pair.second);
	std::array<char, 1024> buffer;
	qint64 count = 0;

	if (file.open(QIODevice::OpenModeFlag::WriteOnly))
	{
		while (true)
		{
			size_t remaining = reply_->bytesAvailable();
			if (remaining > buffer.size())
				remaining = buffer.size();

			count = reply_->read(&buffer[0], remaining);
			if (count == -1)
			{
				file.close();
				file.remove();
				break;
			}

			if (count == 0)
				break;

			file.write(&buffer[0], count);
		}

		if (count == 0)
		{
			file.close();
			downloads_.pop_front();
		}
		reply_ = nullptr;
	}

	if (downloads_.size() != 0)
		state_ = State::StartDownload;
	else
	{
		new_fields_ = true;
		state_ = State::Complete;
	}
}
