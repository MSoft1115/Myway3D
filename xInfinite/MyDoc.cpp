#include "stdafx.h"
#include "MyDoc.h"


IMPLEMENT_DYNCREATE(MyDoc, CDocument)

BEGIN_MESSAGE_MAP(MyDoc, CDocument)
END_MESSAGE_MAP()


MyDoc::MyDoc()
{
}

MyDoc::~MyDoc()
{
}

void MyDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

