
// xads_PCServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cxads_PCServerApp:
// �йش����ʵ�֣������ xads_PCServer.cpp
//

class Cxads_PCServerApp : public CWinApp
{
public:
	Cxads_PCServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cxads_PCServerApp theApp;