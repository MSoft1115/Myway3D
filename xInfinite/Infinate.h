
// Infinate.h : Infinate Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CInfinateApp:
// �йش����ʵ�֣������ Infinate.cpp
//

class CInfinateApp : public CWinAppEx
{
public:
	CInfinateApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

	int Run();
	void OnIdle();

protected:
	afx_msg void OnNewScene();
	afx_msg void OnOpenScene();
	afx_msg void OnSaveScene();
	afx_msg void OnExportScene();

	afx_msg void OnViewPane();
};

extern CInfinateApp theApp;
