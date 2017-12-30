/* ==========================================================================
	File :			CMonitor.cpp
	
	Class :			CMonitor

	Date :			12/21/17

	Purpose :		

	Description :	

	Usage :			

   ========================================================================*/

#include "stdafx.h"
#include "CMonitor.h"

extern long TaskbarSize;
extern TCHAR Title[4096];


////////////////////////////////////////////////////////////////////
// Public functions
//
CMonitor::CMonitor(  )
/* ============================================================
	Function :		CMonitor::CMonitor
	Description :	Constructor.
	Access :		Public
					
	Return :		void
	Parameters :	none

	Usage :			

   ============================================================*/
{

	// TODO: Implement

}

CMonitor::~CMonitor(  )
/* ============================================================
	Function :		CMonitor::~CMonitor
	Description :	Destructor.
	Access :		Public
					
	Return :		void
	Parameters :	none

	Usage :			

   ============================================================*/
{

	// TODO: Implement

}

int CMonitor::GetOrientation(  ) const
/* ============================================================
	Function :		CMonitor::GetOrientation
	Description :	Accessor. Getter for "Orientation".
	Access :		Public
					
	Return :		int
	Parameters :	none

	Usage :			Call to get the value of "Orientation".

   ============================================================*/
{

	if ((MonitorRect.right - MonitorRect.left) < (MonitorRect.bottom - MonitorRect.top))
		return Orientation::portrait;
	else
		return Orientation::landscape;

}

void CMonitor::Layout(  )
/* ============================================================
	Function :		CMonitor::Layout
	Description :	
	Access :		Public
					
	Return :		void
	Parameters :	none

	Usage :			

   ============================================================*/
{

	int Width = 0;
	int Height = 0;
	int Left = 0;
	int Top = 0;
	int j = 0;
	if (WindowList.size()) {
		switch (GetOrientation())
		{
		case Orientation::portrait:
			Width = (MonitorRect.right) - (MonitorRect.left);
			Height = (int)floor(((MonitorRect.bottom) - (MonitorRect.top) - (SubtractTaskbar ? TaskbarSize : 0)) / WindowList.size());
			j = 0;
			for (const auto &hwnd : WindowList) {
				Left = (MonitorRect.left);
				Top = (MonitorRect.top) + (Height * j);
				PositionWindow(hwnd, Left, Top, Width, Height);
				j++;
			}
			break;
		case Orientation::landscape:
			Width = (int)floor(((MonitorRect.right) - (MonitorRect.left)) / WindowList.size());
			Height = (MonitorRect.bottom) - (MonitorRect.top) - (SubtractTaskbar ? TaskbarSize : 0);
			j = 0;
			for (const auto &hwnd : WindowList) {
				Left = (MonitorRect.left) + (Width * j);
				Top = (MonitorRect.top);
				PositionWindow(hwnd, Left, Top, Width, Height);
				j++;
			}
			break;
		default:
			break;
		}
	}

}

void CMonitor::PositionWindow( HWND hWnd,double Position,double Size )
/* ============================================================
	Function :		CMonitor::PositionWindow
	Description :	
	Access :		Public
					
	Return :		void
	Parameters :	HWND hWnd,double Position,double Size
	Usage :			

   ============================================================*/
{

	int Left = 0;
	int Top = 0;
	int Width = 0;
	int Height = 0;
	if (!Size)
		Size = 1.0;
	switch (GetOrientation())
	{
	case Orientation::portrait:
		Width = (MonitorRect.right) - (MonitorRect.left);
		Height = (int)floor(((MonitorRect.bottom) - (MonitorRect.top) - (SubtractTaskbar ? TaskbarSize : 0)) * Size);
		Left = (MonitorRect.left);
		Top = (MonitorRect.top) + (int)floor((((MonitorRect.bottom) - (MonitorRect.top) - (SubtractTaskbar ? TaskbarSize : 0)) * Position));
		PositionWindow(hWnd, Left, Top, Width, Height);
		break;
	case Orientation::landscape:
		Width = (int)floor(((MonitorRect.right) - (MonitorRect.left)) * Size);
		Height = (MonitorRect.bottom) - (MonitorRect.top) - (SubtractTaskbar ? TaskbarSize : 0);
		Left = (MonitorRect.left) + (int)floor((((MonitorRect.right) - (MonitorRect.left)) * Position));
		Top = (MonitorRect.top);
		PositionWindow(hWnd, Left, Top, Width, Height);
		break;
	default:
		break;
	}

}

void CMonitor::PositionWindow(HWND hWnd,int Left,int Top,int Width,int Height)
/* ============================================================
Function :		CMonitor::PositionWindow
Description :
Access :		Public

Return :		void
Parameters :	HWND hWnd,int Left,int Top,int Width,int Height
Usage :

============================================================*/
{

#ifdef _DEBUG
	ZeroMemory(Title, sizeof(Title));
	GetWindowText(hWnd, Title, sizeof(Title) - 1);
	std::wcout << _T("Tiling: ") << Title << std::endl << Left << _T(",") << Top << _T(",") << Width << _T(",") << Height << std::endl;
#endif
	if (_tcscmp(Title, L"Microsoft Visual Studio") != 0) {
		Left += 1;
		Top += 1;
		Width += -2;
		Height += -2;
	}
	SetWindowPos(
		hWnd,
		HWND_TOP,
		Left,
		Top,
		Width,
		Height,
		SWP_SHOWWINDOW
		);

}



////////////////////////////////////////////////////////////////////
// Protected functions
//


////////////////////////////////////////////////////////////////////
// Private functions
//


