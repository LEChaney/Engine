//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "Log.h"

Log::Log()
	: m_willWriteToConsole{ true }
	, m_filePath{}
{
}

Log::~Log()
{
	if (m_file.is_open())
		m_file.close();
}

void Log::setConsoleOut(bool willWriteToConsole)
{
	m_willWriteToConsole = willWriteToConsole;
}

void Log::setOutputFile(const std::string & filePath)
{
	if (m_filePath != "")
		m_file.close();
	
	m_filePath = filePath;
	m_file.open(m_filePath);
}

Log g_log;