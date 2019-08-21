#pragma once
#include "ICsv.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace xero
{
	namespace paths
	{
		class CSVWriter
		{
		public:
			CSVWriter() = delete;
			~CSVWriter() = delete;

			template<class InputIt>
			static bool write(std::ostream &strm, std::vector<std::string> &headers, InputIt first, InputIt last)
			{
				for (size_t i = 0; i < headers.size(); i++)
				{
					strm << '"' << headers[i] << '"';
					if (i != headers.size() - 1)
						strm << ",";
				}
				strm << std::endl;

				for (auto it = first; it != last; it++)
				{
					const ICsv &cl = *it;
					for (size_t i = 0; i < headers.size(); i++)
					{
						double v = cl.getField(headers[i]);
						if (i != 0)
							strm << "," ;

						strm << v;
					}
					strm << std::endl;
				}

				return true;
			}
		};
	}
}


