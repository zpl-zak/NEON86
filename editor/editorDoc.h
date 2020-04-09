// editorDoc.h : interface of the CEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITORDOC_H__D90A8044_ADF1_4D70_9B94_893CE84957E6__INCLUDED_)
#define AFX_EDITORDOC_H__D90A8044_ADF1_4D70_9B94_893CE84957E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CEditorDoc : public COleDocument
{
protected: // create from serialization only
	CEditorDoc();
	DECLARE_DYNCREATE(CEditorDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEditorDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITORDOC_H__D90A8044_ADF1_4D70_9B94_893CE84957E6__INCLUDED_)
