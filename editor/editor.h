// editor.h : main header file for the EDITOR application
//

#if !defined(AFX_EDITOR_H__8DB8B407_9B29_4678_9ACC_4D0FEDF9F96F__INCLUDED_)
#define AFX_EDITOR_H__8DB8B407_9B29_4678_9ACC_4D0FEDF9F96F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CEditorApp:
// See editor.cpp for the implementation of this class
//

class CEditorApp : public CWinApp
{
public:
	CEditorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CEditorApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITOR_H__8DB8B407_9B29_4678_9ACC_4D0FEDF9F96F__INCLUDED_)
