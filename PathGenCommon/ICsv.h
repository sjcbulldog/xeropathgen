#pragma once

#include <string>

namespace xero {
	namespace paths {
		class ICsv
		{
		public:
			virtual double getField(const std::string &field) const = 0;
		};
	}
}
