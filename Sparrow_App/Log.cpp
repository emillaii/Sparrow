#include "stdafx.h"
#include "log.h"

Log* Log::GetInstance()
{
	static Log *pLog = new Log("Sparrow_Log\\Sparrow.log");
	return pLog;
}

Log::Log(char* filename) {
	m_stream.open(filename, std::ios::out | std::ios::app);
}

void Log::Write(char* logline) {
	CString t = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S : ");
	m_stream << t + logline << endl;
}

//ToDo: Remove the dummy for ending the window string....
void Log::WriteString(CString logString) {
	char* dummy = " ";
	CString t = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S : ");
	t.Append(logString);
	m_stream << t + dummy << endl;
}

Log::~Log() {
	m_stream.close();
}