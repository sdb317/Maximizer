// Maximizer.cpp : Tiles windows over multiple monitors.
//

#include "stdafx.h"

using namespace std;

TCHAR Title[256];

struct Monitor
{
	HMONITOR hMonitor;
	RECT MonitorRect;
	vector<HWND> WindowList;
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
	//CopyRect(&(pMonitor->MonitorRect), lprcMonitor);
	MONITORINFO MonitorInfo;
	ZeroMemory(&MonitorInfo, sizeof(MonitorInfo));
	MonitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(pMonitor->hMonitor,&MonitorInfo);
	CopyRect(&(pMonitor->MonitorRect), &(MonitorInfo.rcWork));
	MonitorList.push_back(pMonitor);
	return TRUE;
}

BOOL CALLBACK EnumWindowsProc(
	HWND hwnd,
	LPARAM lParam
	)
{
	ZeroMemory(Title, sizeof(Title));
	GetWindowText(hwnd, Title, sizeof(Title) - 1);
	if (_tcslen(Title)>0) // Ignore windows with no titles
		if (!(_tcsstr(Title, L"Maximizer"))) // Ignore this program
			if (!(_tcscmp(Title, L"Microsoft Edge") == 0) && !(_tcscmp(Title, L"Program Manager") == 0)) // Ignore Windows hidden desktop bits
				if (GetWindow(hwnd, GW_CHILD) != NULL)
					if (IsWindowVisible(hwnd))
						if (IsIconic(hwnd) == 0) // Only tile windows that aren't already minimised
						{
#ifdef _DEBUG
							wcout << Title << endl;
#endif
							HMONITOR hMonitor=MonitorFromWindow(hwnd,NULL); // Need to add it to the correct monitor's window list
							std::for_each(
								MonitorList.begin(), 
								MonitorList.end(), 
								[hMonitor, hwnd](Monitor* pMonitor) { if (pMonitor->hMonitor == hMonitor) pMonitor->WindowList.push_back(hwnd); }
							);
						}
	return TRUE;
}

int _tmain(
	int argc,
	_TCHAR* argv[]
	)
{
	EnumDisplayMonitors(NULL,NULL,(MONITORENUMPROC)MonitorEnumProc,NULL); // Build a list of monitors...
	LPARAM lParam = 0;
	EnumDesktopWindows(NULL,(WNDENUMPROC)EnumWindowsProc,lParam); // ...and add each window to one of them
	for (std::vector<Monitor*>::iterator i = MonitorList.begin(); i != MonitorList.end(); ++i){
		if ((*i)->WindowList.size()) {
			if ((((*i)->MonitorRect.right) - ((*i)->MonitorRect.left)) < (((*i)->MonitorRect.bottom) - ((*i)->MonitorRect.top))) { // Portrait
				int Width = ((*i)->MonitorRect.right) - ((*i)->MonitorRect.left);
				int Height = (int)floor((((*i)->MonitorRect.bottom) - ((*i)->MonitorRect.top)) / (*i)->WindowList.size());
				int j = 0;
				for (const auto &hwnd : (*i)->WindowList) {
					SetWindowPos(
						hwnd, 
						HWND_TOP, 
						((*i)->MonitorRect.left),
						((*i)->MonitorRect.top) + (Height * j),
						Width, 
						Height,
						SWP_SHOWWINDOW
						);
					j++;
				}
			}
			else { // Landscape
				int Width = (int)floor((((*i)->MonitorRect.right) - ((*i)->MonitorRect.left)) / (*i)->WindowList.size());
				int Height = ((*i)->MonitorRect.bottom) - ((*i)->MonitorRect.top);
				int j = 0;
				for (const auto &hwnd : (*i)->WindowList) {
					SetWindowPos(
						hwnd,
						HWND_TOP,
						((*i)->MonitorRect.left) + (Width * j),
						((*i)->MonitorRect.top),
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
	return 0;
}

