#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <vector>

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
			static bool write(std::ostream &strm, std::vector<std::string>& headers, InputIt first, InputIt last)
			{
				QJsonArray trajectory;
				QJsonObject pt;
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
				QJsonDocument doc;
				doc.setArray(trajectory);
				strm << doc.toJson().toStdString();

				return true;
			}
		};
	}
}




