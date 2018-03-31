#include "stdafx.h"
#include "Sfr.h"
#include "afxdialogex.h"
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <strsafe.h>
#include "Utils.h"
#define BUFSIZE 4096 

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
void ReadFromPipe(void)

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
{
	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	for (;;)
	{
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) break;
		bSuccess = WriteFile(hParentStdOut, chBuf,
			dwRead, &dwWritten, NULL);
		if (!bSuccess) break;
	}
}

void ErrorExit(PTSTR lpszFunction)

// Format a readable error message, display a message box, 
// and exit from the application.
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(1);
}


BOOL executeCommandLine(CString cmdLine, DWORD & exitCode)
{
	SECURITY_ATTRIBUTES saAttr;
	printf("\n->Start of parent execution.\n");
	// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
		ErrorExit(TEXT("StdoutRd CreatePipe"));
	// Ensure the read handle to the pipe for STDOUT is not inherited.
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		ErrorExit(TEXT("Stdout SetHandleInformation"));
	PROCESS_INFORMATION processInformation = { 0 };
	STARTUPINFO startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.hStdError = g_hChildStd_OUT_Wr;
	startupInfo.hStdOutput = g_hChildStd_OUT_Wr;
	startupInfo.dwFlags |= STARTF_USESTDHANDLES;
	int nStrBuffer = cmdLine.GetLength() + 50;


	// Create the process
	BOOL result = CreateProcess(NULL, cmdLine.GetBuffer(nStrBuffer),
		NULL, NULL, TRUE,
		CREATE_NO_WINDOW,
		NULL, NULL, &startupInfo, &processInformation);
	cmdLine.ReleaseBuffer();


	if (!result)
	{
		// CreateProcess() failed
		// Get the error from the system
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

		// Display the error
		CString strError = (LPTSTR)lpMsgBuf;
		TRACE(_T("::executeCommandLine() failed at CreateProcess()\nCommand=%s\nMessage=%s\n\n"), cmdLine, strError);

		// Free resources created by the system
		LocalFree(lpMsgBuf);

		// We failed.
		return FALSE;
	}
	else
	{
		// Successfully created the process.  Wait for it to finish.
		WaitForSingleObject(processInformation.hProcess, INFINITE);

		// Get the exit code.
		result = GetExitCodeProcess(processInformation.hProcess, &exitCode);

		// Close the handles.
		CloseHandle(processInformation.hProcess);
		CloseHandle(processInformation.hThread);

		if (!result)
		{
			// Could not get exit code.
			TRACE(_T("Executed command but couldn't get exit code.\nCommand=%s\n"), cmdLine);
			return FALSE;
		}
		// We succeeded.
		return TRUE;
	}
}

vector<Sfr_entry> Sfr::calculateSfr(int idx, double currZ, std::string filename) {
	DWORD exitCode = 0;
	long long start = milliseconds_now();
	std::string cmd; 
	cmd = "\"C:\\Program Files\\MTF Mapper\\bin\\mtf_mapper.exe\" " + filename + " -r -q output"; 
	CString ccmd(cmd.c_str());
	if (idx == 0)
		executeCommandLine(ccmd, exitCode);
	if (idx == 1)
		executeCommandLine(_T("\"C:\\Program Files\\MTF Mapper\\bin\\mtf_mapper.exe\" \"output\\UR.png\" -r -q output1"), exitCode);
	//executeCommandLine(_T("\"C:\\Program Files\\MTF Mapper\\bin\\mtf_mapper.exe\" -a \"output\\camera0.bmp\" -r -q --debug --logfile output/result.log output"), exitCode);
	long long elapsed = milliseconds_now() - start;
	start = milliseconds_now();
	DBOUT("MTF Time elapsed: " << elapsed << "\n");
	FILE *pFile = nullptr;
	//SFR analysis
	if (idx == 0)
		pFile = fopen("output\\edge_sfr_values.txt", "r");
	else if (idx == 1)
		pFile = fopen("output1\\edge_sfr_values.txt", "r");
	vector<Sfr_entry> sfr_list;
	if (pFile != nullptr)
	{
		char buf[1024];
		int size = sizeof(buf);
		while (fgets(buf, size, pFile) != nullptr)
		{
			std::string szTemp(buf);
			std::vector<double> values;
			split(szTemp, values);
			if (values.size() > 6 && values[5] > 0.0) {
				std::vector<double> sfr;
				std::copy(values.begin() + 5, values.end(), std::back_inserter(sfr));
				sfr_list.push_back(Sfr_entry((int)values[0], values[1], values[2], sfr));
			}
		}
	}
	//Find the sfr in Nyquist Frequency @ 0.25...0.125 now when i = 8
	size_t i = 8;
	for (int j = 0; j < sfr_list.size(); j++) {
		int idx = j;
		double coef[4] = { 0,0,0,0 };
		int si = int(i);
		int ri = 1;
		int ei = si < 0 ? 0 : si;
		double eiv = 0;
		if (ei > sfr_list[idx].sfr.size() - 1) {
			eiv = (sfr_list[idx].sfr[sfr_list[idx].sfr.size() - 1] - sfr_list[idx].sfr[sfr_list[idx].sfr.size() - 2]) * (ei - (sfr_list[idx].sfr.size() - 1)) + sfr_list[idx].sfr[sfr_list[idx].sfr.size() - 1]; // extend last point linearly
		}
		else {
			eiv = sfr_list[idx].sfr[ei];
		}

		for (int c = 0; c < 4; c++) {
			coef[c] += eiv * sfr_cubic_weights[c][ri];
		}
		DBOUT("0.25 " << coef[0] * 100 << "\n");
		sfr_list[idx].sfr_025 = round(coef[0] * 100 * 1000) / 1000;
		sfr_list[idx].pz = currZ;
	}

	return sfr_list;
}
