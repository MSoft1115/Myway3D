#pragma once

#include "resource.h"       // ������

class MyApp : public CWinApp
{
    DECLARE_MESSAGE_MAP()

public:
	MyApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual void OnIdle();

};

extern MyApp theApp;