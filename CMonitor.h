#ifndef _CMONITOR_H_8CEF3D16_75C3_4B86_BD4E8F4A2C5
#define _CMONITOR_H_8CEF3D16_75C3_4B86_BD4E8F4A2C5

///////////////////////////////////////////////////////////
// File :		CMonitor.h
// Created :	12/22/17
//


class CMonitor
{
public:
// Construction/destruction
	CMonitor(  );
	virtual ~CMonitor(  );

// Operations
	int GetOrientation(  ) const;
	void Layout(  );
	void PositionWindow( HWND hWnd,double Position = 0.0,double Size = 1.0 );
	void PositionWindow( HWND hWnd,int Left,int Top,int Width,int Height );


// Attributes
	HMONITOR hMonitor;
	RECT MonitorRect;
	std::vector<HWND> WindowList;
	bool SubtractTaskbar;
	enum Orientation { portrait = 1, landscape = 2 };


};
#endif //_CMONITOR_H_8CEF3D16_75C3_4B86_BD4E8F4A2C5
