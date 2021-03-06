#pragma once

#include "IDockPane.h"

class xKeyFrameTab : public CMFCTabCtrl
{
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
};


class xEnvironmentPane : public IDockPane
{
	DECLARE_MESSAGE_MAP()

	DECLARE_SINGLETON(xEnvironmentPane);

public:
	xEnvironmentPane();
	virtual ~xEnvironmentPane();

	void AdjustLayout();

	void _Frush();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point) {}


	void _Create(Event * sender);
	void _Init(Event * sender);
	void _Shutdown(Event * sender);
	void _Update(Event * sender);

	void _Frush(CMFCPropertyGridCtrl & PropertyGrid, IPropertyObj * obj);
	void _ToCtrl(CMFCPropertyGridProperty * gp, IPropertyObj * obj, const Property * p);

protected:
	tEventListener<xEnvironmentPane> OnCreatePane;
	tEventListener<xEnvironmentPane> OnInit;
	tEventListener<xEnvironmentPane> OnShutdown;
	tEventListener<xEnvironmentPane> OnUpdate;

	xKeyFrameTab mTab;

	CMFCPropertyGridCtrl mNight;
	CMFCPropertyGridCtrl mMorning;
	CMFCPropertyGridCtrl mNoon;
	CMFCPropertyGridCtrl mEvening;
	CMFCPropertyGridCtrl mGlobal;

};

