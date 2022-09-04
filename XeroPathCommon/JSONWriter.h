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
#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <vector>
#include <list>
#include <string>

namespace xero
{
	namespace paths
	{
		class JSONWriter
		{
		public:
			JSONWriter() = delete;
			~JSONWriter() = delete;

			template<class InputIt>
			static bool write(std::ostream &strm, std::vector<std::string>& headers, InputIt first, InputIt last, const std::list<std::pair<std::string, std::string>>& props)
			{
				QJsonArray trajectory;
				QJsonObject pt;
				QJsonObject top;
				QJsonObject propobj;

				top["_version"] = "1";
				for (const std::pair<std::string, std::string>& entry : props)
				{
					propobj[QString::fromStdString(entry.first)] = QString::fromStdString(entry.second);
				}
				top["properities"] = propobj;
				for (auto it = first; it != last; it++)
				{
					pt = QJsonObject();
					const ICsv& cl = *it;
					for (size_t i = 0; i < headers.size(); i++)
					{
						double v = cl.getField(headers[i]);
						pt[headers[i].c_str()] = v;
					}
					trajectory.append(pt);
				}
				top["points"] = trajectory;
				QJsonDocument doc;
				doc.setObject(top);
				strm << doc.toJson().toStdString();

				return true;
			}
		};
	}
}




