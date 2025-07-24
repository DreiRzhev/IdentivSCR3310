// Mcard.h : main header file for the MCARD application
//

#if !defined(AFX_MCARD_H__F5A07DC5_7259_11D6_A66A_0010B5C7D9C6__INCLUDED_)
#define AFX_MCARD_H__F5A07DC5_7259_11D6_A66A_0010B5C7D9C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMcardApp:
// See Mcard.cpp for the implementation of this class
//

class CMcardApp : public CWinApp
{
public:
	CMcardApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMcardApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMcardApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

	

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCARD_H__F5A07DC5_7259_11D6_A66A_0010B5C7D9C6__INCLUDED_)
