//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// Author       : Lance Chaney, Jack Mair
// Mail         : lance.cha7337@mediadesign.school.nz
//

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