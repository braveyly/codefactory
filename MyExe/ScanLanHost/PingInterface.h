// PingInterface.h : main header file for the PINGINTERFACE application
//

#if !defined(AFX_PINGINTERFACE_H__5F75B2A5_7601_4DE6_9C38_63E2A4C1BA99__INCLUDED_)
#define AFX_PINGINTERFACE_H__5F75B2A5_7601_4DE6_9C38_63E2A4C1BA99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPingInterfaceApp:
// See PingInterface.cpp for the implementation of this class
//

class CPingInterfaceApp : public CWinApp
{
public:
	CPingInterfaceApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPingInterfaceApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPingInterfaceApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PINGINTERFACE_H__5F75B2A5_7601_4DE6_9C38_63E2A4C1BA99__INCLUDED_)
