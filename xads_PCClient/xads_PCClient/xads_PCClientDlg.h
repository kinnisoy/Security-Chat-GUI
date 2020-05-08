
// xads_PCClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include"Functions.h"
#include "DES.h"
#include <stdlib.h>
#include <time.h>



// Cxads_PCClientDlg �Ի���
class Cxads_PCClientDlg : public CDialogEx
{
// ����
public:
	Cxads_PCClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_XADS_PCCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonconnect();
	BOOL ConnectSocket(Cxads_PCClientDlg *);
	BOOL EnableWindow(DWORD DlgId, BOOL bUsed);
	void SetRevBoxText(CString strMsg);


	SOCKET m_ClientSock;
	CEdit m_EditRevBox;
	afx_msg void OnBnClickedButtonstop();
	afx_msg void OnBnClickedButtonquit();
	afx_msg void OnBnClickedButtonsend();
	afx_msg void OnEnChangeEditsendbox();

private:
	BOOL isServerOn;
	int N, E;
public:
	

};

DWORD WINAPI ConnectNetworkThread(  LPVOID lpParameter);
BOOL socket_Select(SOCKET hSocket,DWORD nTimeOut,BOOL bRead);
CString GetTime();