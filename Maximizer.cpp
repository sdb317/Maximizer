// Maximizer.cpp : Tiles windows over multiple monitors.
//

#include "stdafx.h"
#include <psapi.h> // For access to GetModuleFileNameEx
#include <strsafe.h>
#include <sstream>

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
TCHAR ClassName[4096];
TCHAR FileName[MAX_PATH];
CodLib::CCoInitialise CoInitialise;
CodLib::CDocument Document;

std::vector<CMonitor*> MonitorList;

void PrintError(
	_TCHAR* FunctionName
	)
{
	LPVOID pMessage;
	LPVOID pMessageText;
	DWORD Error = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		Error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&pMessage,
		0, 
		NULL
	);
	pMessageText = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)pMessage) + lstrlen(FunctionName) + lstrlen(L" error ") + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)pMessageText, LocalSize(pMessageText) / sizeof(TCHAR), TEXT("%s error %d: %s"), FunctionName, Error, pMessage);
	std::wcout << (LPCTSTR)pMessageText << std::endl;
}

BOOL TaskbarWindow( // Does this window correspond to the top-level window of an app in the Taskbar? I.e. is it the one we want to re-size?
	HWND hwnd
	)
{
	ZeroMemory(Title, sizeof(Title));
	GetWindowText(hwnd, Title, sizeof(Title) - 1);
/*
	if (!(_tcsstr(Title, L"Mozilla Firefox"))) // Ignore all but this program
	    return FALSE;
*/
	if (!(_tcslen(Title) > 0)) {
	    return FALSE; // If there's no title then there's nothing to say
    }
#ifdef _DEBUG
    std::wcout << Title << std::endl;
#endif
    WINDOWINFO WindowInfo;
	ZeroMemory(&WindowInfo, sizeof(WindowInfo));
    GetWindowInfo(hwnd, &WindowInfo);
    if (!(WindowInfo.dwExStyle & WS_EX_APPWINDOW)) {
        if (WindowInfo.dwExStyle & WS_EX_TOOLWINDOW) {
#ifdef _DEBUG
            std::wcout << L"\tFailed: " << L"WS_EX_TOOLWINDOW" << std::endl;
#endif
 	        return FALSE;
        }
        if (WindowInfo.dwStyle & WS_CHILD) {
#ifdef _DEBUG
            std::wcout << L"\tFailed: " << L"WS_CHILD" << std::endl;
#endif
 	        return FALSE;
        }
    }
    if ((GetParent(hwnd)) && (WindowInfo.dwStyle & WS_POPUP)) {
#ifdef _DEBUG
        std::wcout << L"\tFailed: " << L"Parent & WS_POPUP" << std::endl;
#endif
 	    return FALSE;
    }
    if (WindowInfo.dwExStyle & WS_EX_CLIENTEDGE) {
#ifdef _DEBUG
        std::wcout << L"\tFailed: " << L"WS_EX_CLIENTEDGE" << std::endl;
#endif
 	    return FALSE;
    }
    if (WindowInfo.dwExStyle & WS_EX_DLGMODALFRAME) {
#ifdef _DEBUG
        std::wcout << L"\tFailed: " << L"WS_EX_DLGMODALFRAME" << std::endl;
#endif
 	    return FALSE;
    }
    CodLib::CBstr XPath;
    XPath.Format(L"/ProcessList/Process[contains(\"%s\",Title/text())]",Title);
	CodLib::CNode Node = Document.FindNode(XPath);
    if ((!GetWindow(hwnd, GW_CHILD)) && Node.IsEmpty()) { // It should have children, but not always (hint: use Spy++ here) so see if it's in the list
#ifdef _DEBUG
        std::wcout << L"\tFailed: " << L"No children & not found: " << (_bstr_t)XPath << std::endl;
#endif
 	    return FALSE;
    }
	if (FindWindow(L"Windows.UI.Core.CoreWindow", Title)) {
#ifdef _DEBUG
        std::wcout << L"\tFailed: " << L"Metro app" << std::endl;
#endif
	    return FALSE;
    }
    WINDOWPLACEMENT WindowPlacement;
	ZeroMemory(&WindowPlacement, sizeof(WindowPlacement));
    GetWindowPlacement(hwnd, &WindowPlacement);
    if (!WindowPlacement.rcNormalPosition.left && !WindowPlacement.rcNormalPosition.top && !WindowPlacement.rcNormalPosition.right && !WindowPlacement.rcNormalPosition.bottom) {
#ifdef _DEBUG
        std::wcout << L"\tFailed: " << L"Zero size" << std::endl;
#endif
    }
 	return TRUE;
}

void GetProcessInfo( // Get the executable for the app
	HWND hwnd
	)
{
	ZeroMemory(ClassName, sizeof(ClassName));
    GetClassName(hwnd, ClassName, sizeof(ClassName) - 1);
	DWORD ProcessId = 0;
	GetWindowThreadProcessId(
		hwnd,
		&ProcessId
	); 
	HANDLE ProcessHandle = NULL;
	ZeroMemory(FileName, sizeof(FileName));
	ProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcessId);
	if (ProcessHandle != NULL) {
		if (GetModuleFileNameEx(ProcessHandle, NULL, FileName, MAX_PATH) != 0) {
#ifdef _DEBUG
			//std::wcout << FileName << std::endl;
			//std::wcout << _T("    ") << Title << std::endl;
			//std::wcout << _T("    ") << ClassName << std::endl;
#endif
		}
		CloseHandle(ProcessHandle);
	}
    else {
        PrintError(L"OpenProcess");
    }
}

BOOL CALLBACK MonitorEnumProc( // Build a list of the system's monitors
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

BOOL Create( // Read the list and create each process
	_TCHAR* DocumentName
	)
{
	Document.LoadDocument(CodLib::CBstr(DocumentName));
	CodLib::CNodeList Nodes = Document.FindNodes(L"/ProcessList/Process");
	CodLib::CNode Node;
	while (!((Node = Nodes.NextNode()).IsEmpty())) {
		_bstr_t CommandLine = (_bstr_t)Node.FindNode(L"CommandLine");
		std::wcout << CommandLine << std::endl;
		STARTUPINFO StartupInfo;
		ZeroMemory(&StartupInfo, sizeof(StartupInfo));
		StartupInfo.cb = sizeof(StartupInfo);
		PROCESS_INFORMATION ProcessInformation;
		ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));
		if (!
			CreateProcess(
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
			)
        )
            PrintError(L"CreateProcess");
	}
	return TRUE;
}

BOOL CALLBACK EnumForLayout( // Add a list of 'restored' windows to tile for each monitor
	HWND hwnd,
	LPARAM lParam // IncludeIconic
	)
{
	ZeroMemory(Title, sizeof(Title));
	GetWindowText(hwnd, Title, sizeof(Title) - 1);
	ZeroMemory(ClassName, sizeof(ClassName));
    GetClassName(hwnd, ClassName, sizeof(ClassName) - 1);
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
			//std::wcout << _T("    ") << ClassName << std::endl;
#endif
		}
		CloseHandle(ProcessHandle);
	}
    else {
        PrintError(L"OpenProcess");
    }
	if (FindWindow(L"Windows.UI.Core.CoreWindow", Title)) { // Ignore all hidden 'Metro' apps
		std::wcout << _T("Metro: ") << Title << std::endl;
	    return TRUE;
    }
	BOOL IncludeIconic = (BOOL)lParam;
	if (IncludeIconic || (IsIconic(hwnd) == 0)) // Only tile windows that aren't already minimised
		if (IsWindowVisible(hwnd))
			if (!(_tcsstr(FileName, L"SystemApps"))) // Ignore system apps
				if (_tcslen(Title) > 0) // Ignore windows with no titles

			        if ((!(_tcsstr(Title, L"Settings"))) && (!(_tcsstr(Title, L"Microsoft Store"))) && (!(_tcsstr(Title, L"Metro: "))))// Ignore Microsoft crap
					    if (!(_tcsstr(Title, L"Maximizer"))) // Ignore this program
						    if (!(_tcscmp(Title, L"Microsoft Edge") == 0) && !(_tcscmp(Title, L"Program Manager") == 0) && !(_tcscmp(Title, L"JamPostMessageWindow") == 0) && !(_tcscmp(Title, L"ZPToolBarParentWnd") == 0) && !(_tcscmp(Title, L"OverlayWindow") == 0)) // Ignore Windows hidden desktop bits
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

BOOL Layout( // Tile and (optionally) minimise
    )
{
	EnumDesktopWindows(NULL, (WNDENUMPROC)EnumForLayout, (LPARAM)false); // Add each window to a monitor
	for (std::vector<CMonitor*>::iterator i = MonitorList.begin(); i != MonitorList.end(); ++i){
		(*i)->Layout();
		delete *i;
	}
    SHORT KeyState=GetAsyncKeyState(VK_LCONTROL); // Is the left Ctrl key being held down?
    if (KeyState & 0x8000) {
	    HWND lTrayHwnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	    SendMessage(lTrayHwnd, WM_COMMAND, MIN_ALL, 0); // Minimize all windows
    }
	return TRUE;
}

BOOL CALLBACK EnumForRecord( // Record the current window positions in the XML doc
	HWND hwnd,
	LPARAM lParam
	)
{
    if (!TaskbarWindow(hwnd))
	    return TRUE;
	GetProcessInfo(hwnd);
    CodLib::CBstr XPath;
    XPath.Format(L"/ProcessList/Process[CommandLine/text()=\"%s\"]",FileName);
	CodLib::CNode Node = Document.FindNode(XPath);
    if ((!Node.IsEmpty()) && (_tcslen(Title) > 0)) {
        WINDOWPLACEMENT WindowPlacement;
	    ZeroMemory(&WindowPlacement, sizeof(WindowPlacement));
        GetWindowPlacement(hwnd, &WindowPlacement);
        if (!WindowPlacement.rcNormalPosition.left && !WindowPlacement.rcNormalPosition.top && !WindowPlacement.rcNormalPosition.right && !WindowPlacement.rcNormalPosition.bottom)
	        return TRUE;
		// Should update the XML doc nodes here!
        std::wcout << FileName << L" - " << Title << std::endl;
		std::wcout << L"\t" << L"(" << WindowPlacement.rcNormalPosition.left << L"," << WindowPlacement.rcNormalPosition.top << L"," << WindowPlacement.rcNormalPosition.right << L"," << WindowPlacement.rcNormalPosition.bottom << L")" << std::endl;
    }
	return TRUE;
}

BOOL Record(
	_TCHAR* DocumentName
    )
{
	CodLib::CCoInitialise CoInitialise;
	CodLib::CDocument Document;
	Document.LoadDocument(CodLib::CBstr(DocumentName));
	EnumDesktopWindows(NULL, (WNDENUMPROC)EnumForRecord, (LPARAM)NULL);
	return TRUE;
}

BOOL CALLBACK EnumForPlayback( // Restore and position the windows based on the window positions in the XML doc
	HWND hwnd,
	LPARAM lParam
	)
{
    if (!TaskbarWindow(hwnd))
	    return TRUE;
	GetProcessInfo(hwnd);
    CodLib::CBstr XPath;
    XPath.Format(L"/ProcessList/Process[CommandLine/text()=\"%s\" and contains(\"%s\",Title/text())]",FileName,Title);
#ifdef _DEBUG
    std::wcout << (_bstr_t)XPath << std::endl;
#endif
	CodLib::CNode Node = Document.FindNode(XPath);
    if ((!Node.IsEmpty()) && (_tcslen(Title) > 0)) {
        std::wcout << FileName << L" - " << Title << std::endl;
		long left = (long)Node.FindNode(L"left");
		long top = (long)Node.FindNode(L"top");
		long right = (long)Node.FindNode(L"right");
		long bottom = (long)Node.FindNode(L"bottom");
		long width = right - left;
		long height = bottom - top;
	    if (_tcscmp(Title, L"Microsoft Visual Studio") == 0) { // To include the border...
		    left += 1;
		    top += 1;
		    width += -2;
		    height += -2;
	    }
        WINDOWPLACEMENT WindowPlacement;
	    ZeroMemory(&WindowPlacement, sizeof(WindowPlacement));
        GetWindowPlacement(hwnd, &WindowPlacement);
        WindowPlacement.showCmd = SW_RESTORE;
		if (!
	        SetWindowPlacement( // Need to restore before re-sizing
		        hwnd,
                &WindowPlacement
		    )
        )
            PrintError(L"SetWindowPos");
		else
		    if (!
	            SetWindowPos(
		            hwnd,
		            HWND_TOP,
		            left,
		            top,
		            width,
		            height,
		            SWP_SHOWWINDOW
		        )
            )
                PrintError(L"SetWindowPos");
		    else
                std::wcout << L"\t" << L"(" << left << L"," << top << L"," << right << L"," << bottom << L")" << std::endl;
    }
	return TRUE;
}

BOOL Playback(
	_TCHAR* DocumentName
    )
{
	Document.LoadDocument(CodLib::CBstr(DocumentName));
	EnumDesktopWindows(NULL, (WNDENUMPROC)EnumForPlayback, (LPARAM)NULL);
	return TRUE;
}

int _tmain(
	int argc,
	_TCHAR* argv[]
	)
{
	HWND lTrayHwnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)MonitorEnumProc, NULL); // Build a list of monitors...
	if (argc >= 2) {
		switch (argv[1][0]) {
		case '-':
			switch (argv[1][1]) {
			case 'c': // Create processes from a list
				Create(argv[2]);
#ifndef _DEBUG
		        SendMessage(lTrayHwnd, WM_COMMAND, MIN_ALL, 0); // Minimize all windows
#endif
				break;
			case 'l': // Layout restored windows
				Layout();
				break;
			case 'r': // Save all window positions
				Record(argv[2]);
				break;
			case 'p': // Restore all window positions
				Playback(argv[2]);
#ifndef _DEBUG
		        SendMessage(lTrayHwnd, WM_COMMAND, MIN_ALL, 0); // Minimize all windows
#endif
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

