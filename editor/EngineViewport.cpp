// EngineViewport.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "EngineViewport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEngineViewport

IMPLEMENT_DYNCREATE(CEngineViewport, CView)

CEngineViewport::CEngineViewport()
{
	mEngine = CEngine::the();
}

CEngineViewport::~CEngineViewport()
{
}


BEGIN_MESSAGE_MAP(CEngineViewport, CView)
	//{{AFX_MSG_MAP(CEngineViewport)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEngineViewport drawing

void CEngineViewport::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CEngineViewport diagnostics

#ifdef _DEBUG
void CEngineViewport::AssertValid() const
{
	CView::AssertValid();
}

void CEngineViewport::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEngineViewport message handlers

int CEngineViewport::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CEngineViewport::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	mEngine->GetRenderer()->Present();
	this->Invalidate(FALSE);
}

void CEngineViewport::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CEngineViewport::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CEngineViewport::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	RECT rect;
	rect.right = cx;
	rect.bottom = cy;
	
	if (mEngine->GetRenderer())
		mEngine->GetRenderer()->Resize(rect);
}

void CEngineViewport::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	RECT rect;
	GetClientRect(&rect);

	mEngine->Init(GetSafeHwnd(), rect); 
}

void CEngineViewport::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

}
