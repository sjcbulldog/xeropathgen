#pragma once

#include "MathUtils.h"
#include "ICsv.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <vector>

namespace xero
{
	namespace paths
	{
		class PathWeaverWriter
		{
		public:
			PathWeaverWriter() = delete;
			~PathWeaverWriter() = delete;

			template<class InputIt>
			static bool write(std::ostream & strm, std::vector<std::string> & headers, InputIt first, InputIt last)
			{
				QJsonArray trajectory;
				QJsonObject pt, pose, trans, rot;
				QJsonObject top;

				for (auto it = first; it != last; it++)
				{
					pt = QJsonObject();
					const ICsv& cl = *it;

					pt["time"] = cl.getField("time");
					pt["velocity"] = cl.getField("velocity");
					pt["acceleration"] = cl.getField("acceleration");
					pt["curvature"] = cl.getField("curvature");

					pose = QJsonObject();

					trans = QJsonObject();
					trans["x"] = cl.getField("x");
					trans["y"] = cl.getField("y");
					pose["translation"] = trans;

					rot = QJsonObject();
					rot["radians"] = xero::paths::MathUtils::degreesToRadians(cl.getField("heading"));
					pose["rotation"] = rot;

					pt["pose"] = pose;
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

