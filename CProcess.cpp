/* ==========================================================================
	File :			CProcess.cpp
	
	Class :			CProcess

	Date :			12/21/17

	Purpose :		

	Description :	

	Usage :			

   ========================================================================*/

#include "stdafx.h"
#include "CProcess.h"
#include "CMonitor.h"


////////////////////////////////////////////////////////////////////
// Public functions
//
CProcess::CProcess(_bstr_t Executable, _bstr_t Title, CMonitor* pMonitor, double Position, double Size) :
	Executable(L""),
	Title(L""),
	pMonitor( NULL ),
	Position( 0.0 ),
	Size( 1.0 ),
	PID(NULL),
	hWnd(NULL)
/* ============================================================
	Function :		CProcess::CProcess
	Description :	Constructor.
	Access :		Public
					
	Return :		void
	Parameters :	_bstr_t Executable,CMonitor* pMonitor,double Position,double Size
	Usage :			

   ============================================================*/
{

	this->Executable = Executable;
	this->Title = Title;
	this->pMonitor = pMonitor;
	this->Position = Position;
	this->Size = Size;

}

CProcess::~CProcess(  )
/* ============================================================
	Function :		CProcess::~CProcess
	Description :	Destructor.
	Access :		Public
					
	Return :		void
	Parameters :	none

	Usage :			

   ============================================================*/
{

	// TODO: Implement

}

bool CProcess::Create(  )
/* ============================================================
	Function :		CProcess::Create
	Description :	
	Access :		Public
					
	Return :		bool
	Parameters :	none

	Usage :			

   ============================================================*/
{

	// TODO: Implement
	return true;

}

bool CProcess::Layout()
/* ============================================================
Function :		CProcess::Layout
Description :
Access :		Public

Return :		bool
Parameters :	none

Usage :

============================================================*/
{

	pMonitor->PositionWindow(hWnd, Position, Size);
	return true;

}



////////////////////////////////////////////////////////////////////
// Protected functions
//


////////////////////////////////////////////////////////////////////
// Private functions
//


