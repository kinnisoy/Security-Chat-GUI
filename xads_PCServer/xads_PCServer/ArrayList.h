#include "stdafx.h"
class Cxads_PCServerDlg;

class CClientItem
{
public:
	CString m_strIp;
	SOCKET m_ClientSocket;
	HANDLE m_hThread;
	Cxads_PCServerDlg *m_pMainWnd;
	CClientItem(){
		m_ClientSocket = INVALID_SOCKET;
		m_hThread = NULL;
		m_pMainWnd = NULL;
	}
};