// CntrItem.cpp : implementation of the CEditorCntrItem class
//

#include "stdafx.h"
#include "editor.h"

#include "editorDoc.h"
#include "editorView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditorCntrItem implementation

IMPLEMENT_SERIAL(CEditorCntrItem, COleClientItem, 0)

CEditorCntrItem::CEditorCntrItem(CEditorDoc* pContainer)
	: COleClientItem(pContainer)
{
}

CEditorCntrItem::~CEditorCntrItem()
{
}

void CEditorCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);
}

BOOL CEditorCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	if (!COleClientItem::OnChangeItemPosition(rectPos))
		return FALSE;

	return TRUE;
}

void CEditorCntrItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);

	rPosition.SetRect(10, 10, 210, 210);
}

void CEditorCntrItem::OnActivate()
{
    // Allow only one inplace activate item per frame
    CEditorView* pView = GetActiveView();
    ASSERT_VALID(pView);
    COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
    if (pItem != NULL && pItem != this)
        pItem->Close();
    
    COleClientItem::OnActivate();
}

void CEditorCntrItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

    // Hide the object if it is not an outside-in object
    DWORD dwMisc = 0;
    m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
    if (dwMisc & OLEMISC_INSIDEOUT)
        DoVerb(OLEIVERB_HIDE, NULL);
}

void CEditorCntrItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	COleClientItem::Serialize(ar);

	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEditorCntrItem diagnostics

#ifdef _DEBUG
void CEditorCntrItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CEditorCntrItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
