#if !defined(AFX_ENGINEVIEWPORT_H__738DBB0A_3D8E_4434_8ABE_1E8762189BD0__INCLUDED_)
#define AFX_ENGINEVIEWPORT_H__738DBB0A_3D8E_4434_8ABE_1E8762189BD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EngineViewport.h : header file
//

#include "NeonEngine.h"

/////////////////////////////////////////////////////////////////////////////
// CEngineViewport view

class CEngineViewport : public CView
{
protected:
	CEngineViewport();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CEngineViewport)

// Attributes
public:

// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEngineViewport)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CEngineViewport();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CEngineViewport)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CEngine* mEngine;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENGINEVIEWPORT_H__738DBB0A_3D8E_4434_8ABE_1E8762189BD0__INCLUDED_)
