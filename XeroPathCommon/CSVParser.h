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

#include <string>
#include <vector>
#include <fstream>

/// @file CSVParser.h
///
/// @brief This file defines a simple parser for the output of the path generation programs.
///

/// @brief This class is a simple CSV parser that only supports floating point fields
class CSVParser
{
public:
	/// @brief Create the parser object
	CSVParser();

	/// @brief Destroy the parser object
	virtual ~CSVParser();

	/// @brief Open the file and ready to parse data
	/// This method also reads the header line and returns false if it is
	/// not valid.
	/// @param filename the name of the file to open
	/// @returns true if the file was opened sucessfully
	bool open(const std::string& filename);

	/// @brief close the file
	void close();

	/// @brief Returns status of data available in the file
	/// @returns true if there is more data to read, false otherwise
	bool hasData();

	/// @brief Advances to the next row of data
	/// This method returns false if the next data row does not consists of a comma
	/// seperated row of floating point numbers.  The number of entries must match the
	/// number of headers.  It does return true if the end of the data has been reached.
	/// @ref hasData() must be called to detect and of data condition.
	/// @returns true if the next data row was read sucessfully, otherwise false.
	bool next();

	/// @brief Returns the data value associated with the header field given
	/// @param header the name of a header in the CSV file
	/// @returns the floating point value for the named column in the current row
	/// @throws std::runtime_error if an invalid header name is given
	double getData(const char *header);

	bool hasDataField(const std::string &name);

private:
	//
	// Parse the headers in the first line of the file
	//
	bool parseHeaders();

	//
	// Split a string into a set of words splitting at the comma character
	//
	void split(const std::string &line, std::vector<std::string>& result);

private:
	//
	// The input stream we are reading
	//
	std::ifstream in_;

	//
	// The name of the file we are reading
	//
	std::string filename_;

	//
	// The headers for the file
	//
	std::vector<std::string> headers_;

	//
	// The current row of data, a size of zero means we have 
	// reached the end of the file
	//
	std::vector<double> data_;
};

