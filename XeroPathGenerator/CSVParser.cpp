#include "CSVParser.h"
#include <stdexcept>
#include <cassert>

CSVParser::CSVParser()
{
}

CSVParser::~CSVParser()
{
}

bool CSVParser::open(const std::string& filename)
{
	filename_ = filename;
	in_.open(filename_);
	if (!in_.is_open())
		return false;

	if (!parseHeaders())
		return false;

	if (!next())
		return false;

	return true;
}

void CSVParser::split(const std::string& line, std::vector<std::string> &result)
{
	size_t index = 0;
	std::string word;

	while (index < line.length())
	{
		while (index < line.length() && line[index] != ',')
			word += line[index++];

		result.push_back(word);
		word.clear();
		if (index < line.length())
			index++;
	}
}

bool CSVParser::next()
{
	std::string line;

	if (!std::getline(in_, line))
	{
		headers_.clear();
		data_.clear();
		return true;
	}

	std::vector<std::string> tokens;
	split(line, tokens);
	if (tokens.size() != headers_.size())
		return false;

	data_.clear();
	for (size_t i = 0; i < tokens.size(); i++)
	{
		double d;
		
		try {
			d = std::stod(tokens[i]);
			data_.push_back(d);
		}
		catch (...)
		{
			return false;
		}
	}
	return true;
}

bool CSVParser::parseHeaders()
{
	std::string line;

	if (!std::getline(in_, line))
		return false;

	headers_.clear();
	split(line, headers_);

	for (size_t i = 0; i < headers_.size(); i++)
	{
		const std::string& header = headers_[i];
		if (header.length() >= 2 && header[0] == '"' && header[header.length() - 1] == '"')
		{
			std::string newhdr = header.substr(1, header.length() - 2);
			headers_[i] = newhdr;
		}
	}
	return true;
}

bool CSVParser::hasData()
{
	return data_.size() > 0;
}

double CSVParser::getData(const char*name)
{
	assert(headers_.size() == data_.size());

	for (size_t i = 0; i < headers_.size(); i++)
	{
		if (headers_[i] == name)
		{
			return data_[i];
		}
	}

	throw std::runtime_error("data element not found");
}