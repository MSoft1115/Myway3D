
// InfinateView.h : CInfinateView ��Ľӿ�
//

#pragma once


class CInfinateView : public CView
{
protected: // �������л�����
	CInfinateView();
	DECLARE_DYNCREATE(CInfinateView)

// ����
public:
	CInfinateDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CInfinateView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // InfinateView.cpp �еĵ��԰汾
inline CInfinateDoc* CInfinateView::GetDocument() const
   { return reinterpret_cast<CInfinateDoc*>(m_pDocument); }
#endif

