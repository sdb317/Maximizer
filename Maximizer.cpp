// Maximizer.cpp : Tiles windows over multiple monitors.
//

#include "stdafx.h"
#include <psapi.h> // For access to GetModuleFileNameEx

#define MIN_ALL 419

using namespace std;

TCHAR Title[4096];

long TaskbarSize = 0;

struct Monitor
{
	HMONITOR hMonitor;
	RECT MonitorRect;
	vector<HWND> WindowList;
	bool SubtractTaskbar;
};

vector<Monitor*> MonitorList;

BOOL CALLBACK MonitorEnumProc(
	HMONITOR hMonitor,
	HDC      hdcMonitor,
	LPRECT   lprcMonitor,
	LPARAM   dwData
	)
{
	Monitor* pMonitor = new Monitor;
	pMonitor->hMonitor = hMonitor;
	pMonitor->SubtractTaskbar = false;
	//CopyRect(&(pMonitor->MonitorRect), lprcMonitor);
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

BOOL CALLBACK EnumWindowsProc(
	HWND hwnd,
	LPARAM lParam // IncludeIconic
	)
{
	ZeroMemory(Title, sizeof(Title));
	GetWindowText(hwnd, Title, sizeof(Title) - 1);
	DWORD processID = 0;
	GetWindowThreadProcessId(
		hwnd,
		&processID
		); 
	HANDLE processHandle = NULL;
	TCHAR filename[MAX_PATH];
	processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (processHandle != NULL) {
		if (GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH) != 0) {
#ifdef _DEBUG
			//wcout << filename << endl;
			//wcout << _T("    ") << Title << endl;
#endif
		}
		CloseHandle(processHandle);
	}
	BOOL IncludeIconic = (BOOL)lParam;
	if (IncludeIconic || (IsIconic(hwnd) == 0)) // Only tile windows that aren't already minimised
		if (IsWindowVisible(hwnd))
			if (!(_tcsstr(filename, L"SystemApps"))) // Ignore system apps
				if (_tcslen(Title) > 0) // Ignore windows with no titles
					if (!(_tcsstr(Title, L"Maximizer.exe"))) // Ignore this program
						if (!(_tcscmp(Title, L"Microsoft Edge") == 0) && !(_tcscmp(Title, L"Program Manager") == 0)) // Ignore Windows hidden desktop bits
						if ((GetWindow(hwnd, GW_CHILD) != NULL) || (_tcscmp(Title, L"Microsoft Visual Studio") != 0)) {
							wcout << filename << endl;
							wcout << _T("    ") << Title << endl;
							HMONITOR hMonitor = MonitorFromWindow(hwnd, NULL); // Need to add it to the correct monitor's window list
							std::for_each(
								MonitorList.begin(),
								MonitorList.end(),
								[hMonitor, hwnd](Monitor* pMonitor) { if (pMonitor->hMonitor == hMonitor) pMonitor->WindowList.push_back(hwnd); }
							);
						}
	return TRUE;
}

BOOL Record(
	)
{
	EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, (LPARAM)true); // ...and add each window to one of them
	return TRUE;
}

int _tmain(
	int argc,
	_TCHAR* argv[]
	)
{
	/*
	DISPLAY_DEVICE DisplayDevice;
	ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
	DisplayDevice.cb = sizeof(DisplayDevice);
	DWORD iDevNum = 0;
	while (EnumDisplayDevices(NULL, iDevNum, &DisplayDevice, EDD_GET_DEVICE_INTERFACE_NAME)) {
		//LONG ChangeDisplaySettingsEx( // Use this to fix wonky screen positions
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
	if (argc >= 2) {
		if (_tcscmp(argv[1], L"R") == 0) {
			Record();
		}
	}
	else {
		EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)MonitorEnumProc, NULL); // Build a list of monitors...
		EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, (LPARAM)false); // ...and add each window to one of them
		for (std::vector<Monitor*>::iterator i = MonitorList.begin(); i != MonitorList.end(); ++i){
			if ((*i)->WindowList.size()) {
				if ((((*i)->MonitorRect.right) - ((*i)->MonitorRect.left)) < (((*i)->MonitorRect.bottom) - ((*i)->MonitorRect.top))) { // Portrait
					int Width = ((*i)->MonitorRect.right) - ((*i)->MonitorRect.left);
					int Height = (int)floor((((*i)->MonitorRect.bottom) - ((*i)->MonitorRect.top) - ((*i)->SubtractTaskbar ? TaskbarSize : 0)) / (*i)->WindowList.size());
					int j = 0;
					for (const auto &hwnd : (*i)->WindowList) {
						int Left = ((*i)->MonitorRect.left);
						int Top = ((*i)->MonitorRect.top) + (Height * j);
#ifdef _DEBUG
						ZeroMemory(Title, sizeof(Title));
						GetWindowText(hwnd, Title, sizeof(Title) - 1);
						wcout << _T("Tiling: ") << Title << endl << Left << _T(",") << Top << _T(",") << Width << _T(",") << Height << endl;
#endif
						if (_tcscmp(Title, L"Microsoft Visual Studio") != 0) {
							Left += 1;
							Top += 1;
							Width += -2;
							Height += -2;
						}
						SetWindowPos(
							hwnd,
							HWND_TOP,
							Left,
							Top,
							Width,
							Height,
							SWP_SHOWWINDOW
							);
						j++;
					}
				}
				else { // Landscape
					int Width = (int)floor((((*i)->MonitorRect.right) - ((*i)->MonitorRect.left)) / (*i)->WindowList.size());
					int Height = ((*i)->MonitorRect.bottom) - ((*i)->MonitorRect.top) - ((*i)->SubtractTaskbar ? TaskbarSize : 0);
					int j = 0;
					for (const auto &hwnd : (*i)->WindowList) {
						int Left = ((*i)->MonitorRect.left) + (Width * j);
						int Top = ((*i)->MonitorRect.top);
#ifdef _DEBUG
						ZeroMemory(Title, sizeof(Title));
						GetWindowText(hwnd, Title, sizeof(Title) - 1);
						wcout << _T("Tiling: ") << Title << endl << Left << _T(",") << Top << _T(",") << Width << _T(",") << Height << endl;
#endif
						SetWindowPos(
							hwnd,
							HWND_TOP,
							Left,
							Top,
							Width,
							Height,
							SWP_SHOWWINDOW
							);
						j++;
					}
				}
			}
			delete *i;
		}
		MonitorList.clear();
		HWND lTrayHwnd = FindWindow(_T("Shell_TrayWnd"), NULL);
		SendMessage(lTrayHwnd, WM_COMMAND, MIN_ALL, 0); // Minimize all windows
	}
	return 0;
}

