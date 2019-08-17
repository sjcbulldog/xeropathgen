#pragma once

#include <string>

namespace xero {
	namespace paths {
		class PathBase
		{
		public:
			PathBase();
			virtual ~PathBase();

			virtual const std::string &getName() const = 0;
			virtual void setName(const std::string& name) = 0;
		};
	}
}
