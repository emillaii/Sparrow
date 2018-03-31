#pragma once
#include <fstream>

using namespace std;

class Log {
public:
	static Log* GetInstance();
	Log(char* filename);
	~Log();
	void Write(char* logline);
	void WriteString(CString logString);
private:
	ofstream m_stream;
};