// editorDoc.cpp : implementation of the CEditorDoc class
//

#include "stdafx.h"
#include "editor.h"

#include "editorDoc.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditorDoc

IMPLEMENT_DYNCREATE(CEditorDoc, COleDocument)

BEGIN_MESSAGE_MAP(CEditorDoc, COleDocument)
	//{{AFX_MSG_MAP(CEditorDoc)
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleDocument::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleDocument::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, COleDocument::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, COleDocument::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleDocument::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleDocument::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, COleDocument::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorDoc construction/destruction

CEditorDoc::CEditorDoc()
{
	EnableCompoundFile();

}

CEditorDoc::~CEditorDoc()
{
}

BOOL CEditorDoc::OnNewDocument()
{
	if (!COleDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CEditorDoc serialization

void CEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
	COleDocument::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CEditorDoc diagnostics

#ifdef _DEBUG
void CEditorDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void CEditorDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEditorDoc commands
