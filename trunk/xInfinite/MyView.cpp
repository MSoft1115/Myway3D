// WorldBuildView.cpp : MyView ���ʵ��
//

#include "stdafx.h"

#include "MyDoc.h"
#include "MyView.h"
#include "Controller.h"


// MyView

IMPLEMENT_DYNCREATE(MyView, CView)

BEGIN_MESSAGE_MAP(MyView, CView)

    ON_WM_SIZE()

END_MESSAGE_MAP()

// MyView ����/����

MyView::MyView()
{
	// TODO: �ڴ˴���ӹ������

}

MyView::~MyView()
{
}

BOOL MyView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	return CView::PreCreateWindow(cs);
}

// MyView ����

void MyView::OnDraw(CDC* /*pDC*/)
{
	MyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

    static bool bInit = true;
    if (bInit)
    {
        xInfi::Controller::Instance()->Init(m_hWnd);
        bInit = false;
    }
}

MyDoc* MyView::GetDocument() const // �ǵ��԰汾��������
{
    return (MyDoc*)m_pDocument;
}


void MyView::OnSize(UINT nType, int cx, int cy)
{
    xInfi::Controller::Instance()->Resize(cx, cy);
}