
// xads_PCServerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "ArrayList.h"
#include "Functions.h"
#include "DES.h"
#include"stdlib.h"
#include "time.h"
#include "xads_PCServer.h"
BOOL socket_Select(SOCKET hSocket,DWORD nTimeOut,BOOL bRead);
DWORD WINAPI ListenThreadFunc(LPVOID Lparam);
DWORD WINAPI ClientThreadProc(LPVOID Lparam);
CString GetTime();

// Cxads_PCServerDlg 对话框
class Cxads_PCServerDlg : public CDialogEx
{
// 构造
public:
	Cxads_PCServerDlg(CWnd* pParent = NULL);// 标准构造函数

// 对话框数据
	enum { IDD = IDD_XADS_PCSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	BOOL StartServer(void);
	DECLARE_MESSAGE_MAP();

public:
	BOOL EnableWindow(DWORD DlgId, BOOL bUsed);
	afx_msg void OnBnClickedButtonstart();
	SOCKET m_SockListen;
	UINT m_ServicePort;
	HANDLE m_hListenThread;
	CArray<CClientItem> m_ClientArray;
	CEdit m_EditRevBox;
	void SetRevBoxText(CString strMsg);
	void RemoveClientFromArray(CClientItem in_item);
	void SendClientMsg(CString strMsg,CClientItem * WhoseItem);
	BOOL TrayMyIcon(BOOL isAdd);
		
	
	afx_msg void OnBnClickedButtonend();
	afx_msg void OnBnClickedButtonquit();

private:
	BOOL equal(const CClientItem *p1 , const CClientItem * p2);
	LRESULT OnTrayCallbackMsg(WPARAM wparam , LPARAM lparam);
	BOOL m_isServerOpen;

public:
	afx_msg void OnBnClickedButtonsend();
	afx_msg void OnEnChangeEditsendbox();
	afx_msg void OnBnClickedButtonhide();
	afx_msg void OnMenuShow();
	afx_msg void OnMenuQuit();
	afx_msg void OnMenuServer();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
};
