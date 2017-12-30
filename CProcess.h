#ifndef _CPROCESS_H_1DD08332_BFF3_4888_9BC028E0158F
#define _CPROCESS_H_1DD08332_BFF3_4888_9BC028E0158F

///////////////////////////////////////////////////////////
// File :		CProcess.h
// Created :	12/22/17
//

class CMonitor;

class CProcess
{
public:
// Construction/destruction
	CProcess( _bstr_t Executable = L"",_bstr_t Title = L"",CMonitor* pMonitor = NULL,double Position = 0.0,double Size = 1.0 );
	virtual ~CProcess(  );

// Operations
	bool Create(  );
	bool Layout(  );


// Attributes
	_bstr_t Executable;
	_bstr_t Title;
	CMonitor* pMonitor;
	double Position;
	double Size;
	DWORD PID;
	HWND hWnd;


};
#endif //_CPROCESS_H_1DD08332_BFF3_4888_9BC028E0158F
