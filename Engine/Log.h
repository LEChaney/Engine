#pragma once

#include <iostream>
#include <string>
#include <fstream>

class Log {
public:
	Log();
	~Log();
	void setConsoleOut(bool);
	void setOutputFile(const std::string& filePath);

	template <typename T>
	Log& operator<<(T);

private:
	bool m_willWriteToConsole;
	std::string m_filePath;
	std::ofstream m_file;
};

template<typename T>
inline Log& Log::operator<<(T thingToWriteOut)
{
	if (m_willWriteToConsole) {
		std::cerr << thingToWriteOut;
	} 
	if (m_filePath != "")
	{
		m_file << thingToWriteOut;
	} 
	if (m_filePath == "" && !m_willWriteToConsole) {
		std::cerr << "ERROR: No log output set, either set console or log file";
	}

	return *this;
}

extern Log g_log;