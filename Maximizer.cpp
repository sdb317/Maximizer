// Maximizer.cpp : Tiles windows over multiple monitors.
//

#include "stdafx.h"
#include <psapi.h> // For access to GetModuleFileNameEx
#include <strsafe.h>

#include "CCoInitialise.h"
#include "CBstr.h"
#include "CDocument.h"
#include "CNodeList.h"
#include "CNode.h"
#include "CMonitor.h"
#include "CProcess.h"

#define MIN_ALL 419

long TaskbarSize = 0;
TCHAR Title[4096];

std::vector<CMonitor*> MonitorList;

BOOL CALLBACK MonitorEnumProc(
	HMONITOR hMonitor,
	HDC      hdcMonitor,
	LPRECT   lprcMonitor,
	LPARAM   dwData
	)
{
	CMonitor* pMonitor = new CMonitor;
	pMonitor->hMonitor = hMonitor;
	pMonitor->SubtractTaskbar = false;
	MONITORINFO MonitorInfo;
	ZeroMemory(&MonitorInfo, sizeof(MonitorInfo));
	MonitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(pMonitor->hMonitor,&MonitorInfo);
	CopyRect(&(pMonitor->MonitorRect), &(MonitorInfo.rcWork)); // .rcWork from GetMonitorInfo doesn't factor in TaskBar size sometimes!
	if (MonitorInfo.rcWork.bottom < MonitorInfo.rcMonitor.bottom)
		TaskbarSize = (MonitorInfo.rcMonitor.bottom - MonitorInfo.rcWork.bottom);
	else
		pMonitor->SubtractTaskbar = true;
	MonitorList.push_back(pMonitor);
	return TRUE;
}

BOOL CALLBACK EnumForLayout(
	HWND hwnd,
	LPARAM lParam // IncludeIconic
	)
{
	ZeroMemory(Title, sizeof(Title));
	GetWindowText(hwnd, Title, sizeof(Title) - 1);
	DWORD ProcessId = 0;
	GetWindowThreadProcessId(
		hwnd,
		&ProcessId
		); 
	HANDLE ProcessHandle = NULL;
	TCHAR FileName[MAX_PATH];
	ProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcessId);
	if (ProcessHandle != NULL) {
		if (GetModuleFileNameEx(ProcessHandle, NULL, FileName, MAX_PATH) != 0) {
#ifdef _DEBUG
			//std::wcout << FileName << std::endl;
			//std::wcout << _T("    ") << Title << std::endl;
#endif
		}
		CloseHandle(ProcessHandle);
	}
	BOOL IncludeIconic = (BOOL)lParam;
	if (IncludeIconic || (IsIconic(hwnd) == 0)) // Only tile windows that aren't already minimised
		if (IsWindowVisible(hwnd))
			if (!(_tcsstr(FileName, L"SystemApps"))) // Ignore system apps
				if (_tcslen(Title) > 0) // Ignore windows with no titles
			        if ((!(_tcsstr(Title, L"Settings"))) && (!(_tcsstr(Title, L"Microsoft Store"))))// Ignore Microsoft crap
					    if (!(_tcsstr(Title, L"Maximizer"))) // Ignore this program
						    if (!(_tcscmp(Title, L"Microsoft Edge") == 0) && !(_tcscmp(Title, L"Program Manager") == 0)) // Ignore Windows hidden desktop bits
						    if ((GetWindow(hwnd, GW_CHILD) != NULL) || (_tcscmp(Title, L"Microsoft Visual Studio") != 0)) {
							    std::wcout << FileName << std::endl;
							    std::wcout << _T("    ") << Title << std::endl;
							    HMONITOR hMonitor = MonitorFromWindow(hwnd, NULL); // Need to add it to the correct monitor's window list
							    std::for_each(
								    MonitorList.begin(),
								    MonitorList.end(),
								    [hMonitor, hwnd](CMonitor* pMonitor) { if (pMonitor->hMonitor == hMonitor) pMonitor->WindowList.push_back(hwnd); }
							    );
						    }
	return TRUE;
}

BOOL Layout()
{
	EnumDesktopWindows(NULL, (WNDENUMPROC)EnumForLayout, (LPARAM)false); // Add each window to a monitor
	for (std::vector<CMonitor*>::iterator i = MonitorList.begin(); i != MonitorList.end(); ++i){
		(*i)->Layout();
		delete *i;
	}
	HWND lTrayHwnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	SendMessage(lTrayHwnd, WM_COMMAND, MIN_ALL, 0); // Minimize all windows
	return TRUE;
}

BOOL CALLBACK EnumForCreate(
	HWND hwnd,
	LPARAM lParam // CProcess*
	)
{
	ZeroMemory(Title, sizeof(Title));
	GetWindowText(hwnd, Title, sizeof(Title) - 1);
	CProcess* pProcess = (CProcess*)lParam;
	DWORD ProcessId = 0;
	GetWindowThreadProcessId(
		hwnd,
		&ProcessId
		);
	if ((ProcessId == pProcess->PID) && (_tcsstr(Title, pProcess->Title)))
	{
		pProcess->hWnd = hwnd;
		pProcess->Layout();
		return FALSE;
	}
	else
		return TRUE;
}

BOOL Create( // Read the list, create each process, get the top-most window and position with a specified display and layout
	_TCHAR* DocumentName
	)
{
	HWND lTrayHwnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	CodLib::CCoInitialise CoInitialise;
	CodLib::CDocument Document;
	Document.LoadDocument(CodLib::CBstr(DocumentName));
	CodLib::CNodeList Nodes = Document.FindNodes(L"/ProcessList/Process");
	CodLib::CNode Node;
	while (!((Node = Nodes.NextNode()).IsEmpty()))
	{
		_bstr_t CommandLine = (_bstr_t)Node.FindNode(L"CommandLine");
		_bstr_t Title = (_bstr_t)Node.FindNode(L"Title");
		long Monitor = (long)Node.FindNode(L"Monitor");
		double Position = (double)Node.FindNode(L"Position");
		double Size = (double)Node.FindNode(L"Size");
		std::wcout << CommandLine << std::endl;
		STARTUPINFO StartupInfo;
		ZeroMemory(&StartupInfo, sizeof(StartupInfo));
		StartupInfo.cb = sizeof(StartupInfo);
		PROCESS_INFORMATION ProcessInformation;
		ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));
		BOOL Status = 
			CreateProcess
				(
				NULL, // _In_opt_ LPCTSTR lpApplicationName,
				(LPTSTR)CommandLine, // (LPTSTR)CommandLine, // _Inout_opt_ LPTSTR lpCommandLine,
				NULL, // _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
				NULL, // _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
				FALSE, // _In_ BOOL bInheritHandles,
				DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP, // _In_ DWORD dwCreationFlags,
				NULL, // _In_opt_ LPVOID lpEnvironment,
				NULL, // _In_opt_ LPCTSTR lpCurrentDirectory,
				&StartupInfo, // _In_ LPSTARTUPINFO lpStartupInfo,
				&ProcessInformation // _Out_ LPPROCESS_INFORMATION lpProcessInformation
				);
		if (!Status)
		{
			LPVOID pMessage;
			LPVOID pMessageText;
			DWORD Error = GetLastError();
			FormatMessage
				(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				Error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&pMessage,
				0, 
				NULL
				);
			pMessageText = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)pMessage) + lstrlen(L"CreateProcess error ") + 40) * sizeof(TCHAR));
			StringCchPrintf((LPTSTR)pMessageText, LocalSize(pMessageText) / sizeof(TCHAR), TEXT("CreateProcess error %d: %s"), Error, pMessage);
			std::wcout << (LPCTSTR)pMessageText << std::endl;
		}
		else
		{
			//WaitForInputIdle
			//	(
			//	ProcessInformation.hProcess,
			//	20 * 1000
			//	);
			Sleep(30 * 1000);
            CProcess Process(CommandLine, Title, MonitorList[Monitor], Position, Size);
			Process.PID = ProcessInformation.dwProcessId;
			EnumDesktopWindows(NULL, (WNDENUMPROC)EnumForCreate, (LPARAM)&Process); // Find main window
		}
#ifndef _DEBUG
		SendMessage(lTrayHwnd, WM_COMMAND, MIN_ALL, 0); // Minimize all windows
#endif
	}
	return TRUE;
}

BOOL Record()
{
	return TRUE;
}

int _tmain(
	int argc,
	_TCHAR* argv[]
	)
{
	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)MonitorEnumProc, NULL); // Build a list of monitors...
	if (argc >= 2) {
		switch (argv[1][0])
		{
		case '-':
			switch (argv[1][1])
			{
			case 'l': // Layout restored windows
				Layout();
				break;
			case 'c': // Create processes from a list
				Create(argv[2]);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
	else {
	}
	MonitorList.clear();
	return 0;
}

/* Use this to fix wonky screen positions
DISPLAY_DEVICE DisplayDevice;
ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
DisplayDevice.cb = sizeof(DisplayDevice);
DWORD iDevNum = 0;
while (EnumDisplayDevices(NULL, iDevNum, &DisplayDevice, EDD_GET_DEVICE_INTERFACE_NAME)) {
//LONG ChangeDisplaySettingsEx(
//DisplayDevice.DeviceName,
//_In_ DEVMODE *lpDevMode,
//HWND    hwnd,
//_In_ DWORD   dwflags,
//_In_ LPVOID  lParam
//);
iDevNum++;
}
return 0;
*/

