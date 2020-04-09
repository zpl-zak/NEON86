// CntrItem.h : interface of the CEditorCntrItem class
//

#if !defined(AFX_CNTRITEM_H__0D0A8D2D_DF63_4408_905B_FC450B461529__INCLUDED_)
#define AFX_CNTRITEM_H__0D0A8D2D_DF63_4408_905B_FC450B461529__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEditorDoc;
class CEditorView;

class CEditorCntrItem : public COleClientItem
{
	DECLARE_SERIAL(CEditorCntrItem)

// Constructors
public:
	CEditorCntrItem(CEditorDoc* pContainer = NULL);

// Attributes
public:
	CEditorDoc* GetDocument()
		{ return (CEditorDoc*)COleClientItem::GetDocument(); }
	CEditorView* GetActiveView()
		{ return (CEditorView*)COleClientItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorCntrItem)
	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CEditorCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRITEM_H__0D0A8D2D_DF63_4408_905B_FC450B461529__INCLUDED_)
