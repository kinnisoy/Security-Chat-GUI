
// xads_PCClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cxads_PCClientApp:
// �йش����ʵ�֣������ xads_PCClient.cpp
//

class Cxads_PCClientApp : public CWinApp
{
public:
	Cxads_PCClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cxads_PCClientApp theApp;