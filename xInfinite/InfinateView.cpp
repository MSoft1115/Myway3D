
// InfinateView.cpp : CInfinateView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "Infinate.h"
#endif

#include "InfinateDoc.h"
#include "InfinateView.h"
#include "xApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CInfinateView

IMPLEMENT_DYNCREATE(CInfinateView, CView)

BEGIN_MESSAGE_MAP(CInfinateView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CInfinateView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()

END_MESSAGE_MAP()


IMP_SLN(CInfinateView);
// CInfinateView ����/����

CInfinateView::CInfinateView()
{
	// TODO: �ڴ˴���ӹ������
	INIT_SLN;
}

CInfinateView::~CInfinateView()
{
	SHUT_SLN;
}

BOOL CInfinateView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CInfinateView ����

void CInfinateView::OnDraw(CDC* /*pDC*/)
{
	CInfinateDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	static bool bInited = false;
	if (!bInited)
	{
		xApp::Instance()->Init(m_hWnd);
		bInited = true;
	}
}


// CInfinateView ��ӡ


void CInfinateView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CInfinateView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CInfinateView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CInfinateView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CInfinateView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	/*ClientToScreen(&point);
	OnContextMenu(this, point);*/
}

void CInfinateView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


void CInfinateView::OnSize(UINT nType, int cx, int cy)
{
	if (nType != SIZE_MINIMIZED)
		xApp::Instance()->Resize(cx, cy);
}

// CInfinateView ���

#ifdef _DEBUG
void CInfinateView::AssertValid() const
{
	CView::AssertValid();
}

void CInfinateView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CInfinateDoc* CInfinateView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CInfinateDoc)));
	return (CInfinateDoc*)m_pDocument;
}
#endif //_DEBUG


// CInfinateView ��Ϣ�������
