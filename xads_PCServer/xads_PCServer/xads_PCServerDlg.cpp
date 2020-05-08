// xads_PCServerDlg.cpp : 实现文件
//
#pragma once
#include "stdafx.h"
#include "iostream"
#include "xads_PCServer.h"
#include "xads_PCServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

BigInteger* Sb;
DES DES_Container;


bool DH_KeyExchange(SOCKET S_New)
{
	//生成公钥和私钥
	BigInteger Q(My_CrpytGenKey());
	BigInteger Self_Key(My_CrpytGenKey());

	string PublicKeyStr_A;
	if (S_New)
	{
		PublicKeyStr_A = My_RecvKey(S_New);
		if (PublicKeyStr_A.length() > 0)
		{
			My_SendKey(S_New, Q.toString().c_str());
		}
	}
	else
		return FALSE;
	BigInteger P(PublicKeyStr_A);

	//接收Ga,发送Gb,密钥计算
	string Ga_str;
	BigInteger* Ga = NULL;
	BigInteger* Gb = NULL;
	if (P > Q)
		Gb = new BigInteger(Q.modPow(Self_Key, P));
	else
		Gb = new BigInteger(P.modPow(Self_Key, Q));
	if ((Ga_str = My_RecvKey(S_New)).length() > 0)
	{
		Ga = new BigInteger(Ga_str);
		My_SendKey(S_New, Gb->toString().c_str());

		//最终通信密钥计算
		if (P > Q)
			Sb = new BigInteger(Ga->modPow(Self_Key, P));
		else
			Sb = new BigInteger(Ga->modPow(Self_Key, Q));
	}
	return TRUE;
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cxads_PCServerDlg 对话框




Cxads_PCServerDlg::Cxads_PCServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cxads_PCServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_isServerOpen = FALSE;
	m_hListenThread = NULL;
	m_SockListen = NULL;
}

void Cxads_PCServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITGETBOX, m_EditRevBox);
}

BEGIN_MESSAGE_MAP(Cxads_PCServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTONSTART, &Cxads_PCServerDlg::OnBnClickedButtonstart)
	ON_BN_CLICKED(IDC_BUTTONEND, &Cxads_PCServerDlg::OnBnClickedButtonend)
	ON_BN_CLICKED(IDC_BUTTONQUIT, &Cxads_PCServerDlg::OnBnClickedButtonquit)
	ON_BN_CLICKED(IDC_BUTTONSEND, &Cxads_PCServerDlg::OnBnClickedButtonsend)
	ON_EN_CHANGE(IDC_EDITSENDBOX, &Cxads_PCServerDlg::OnEnChangeEditsendbox)
	ON_MESSAGE(WM_TRAYICON_SERVER,OnTrayCallbackMsg)
	ON_BN_CLICKED(IDC_BUTTONHIDE, &Cxads_PCServerDlg::OnBnClickedButtonhide)
	ON_COMMAND(ID_MENU_SHOW, &Cxads_PCServerDlg::OnMenuShow)
	ON_COMMAND(ID_MENU_QUIT, &Cxads_PCServerDlg::OnMenuQuit)
	ON_COMMAND(ID_MENU_SERVER, &Cxads_PCServerDlg::OnMenuServer)
	ON_WM_SIZE()
	
END_MESSAGE_MAP()


// Cxads_PCServerDlg 消息处理程序

BOOL Cxads_PCServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	EnableWindow(IDC_BUTTONSEND,FALSE);
	EnableWindow(IDC_BUTTONEND,FALSE);
	SetDlgItemText(IDC_EDITPORT,_T("8260"));
	GetDlgItem(IDC_EDITPORT)->SetFocus();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cxads_PCServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cxads_PCServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cxads_PCServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL Cxads_PCServerDlg::EnableWindow(DWORD DlgId, BOOL bUsed)
{
	return GetDlgItem(DlgId)->EnableWindow(bUsed);
}

void Cxads_PCServerDlg::OnBnClickedButtonstart()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ServicePort = GetDlgItemInt(IDC_EDITPORT);
	if (m_ServicePort <= 1024 || m_ServicePort > 65535)
	{
		AfxMessageBox(_T("请输入合适端口"));
		SetDlgItemText(IDC_EDITPORT,_T(""));
		GetDlgItem(IDC_EDITPORT)->SetFocus();
		return ;
	}
	StartServer();
	m_isServerOpen = TRUE;
	OnEnChangeEditsendbox(); //如果发送框中原有内容，则使发送按钮有效
}


BOOL Cxads_PCServerDlg::StartServer(void)
{
	m_hListenThread = CreateThread(NULL,0,ListenThreadFunc,this,0,0);
	if (m_hListenThread == NULL)
	{
		return FALSE;
	}else{
		return TRUE;
	}
}

//等待客户端的连接，连接上了就把客户端信息加入链表
DWORD WINAPI ListenThreadFunc(LPVOID Lparam)
{
	Cxads_PCServerDlg * pServer = (Cxads_PCServerDlg *)Lparam;;
	if (INVALID_SOCKET == (pServer->m_SockListen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)))
	{
		AfxMessageBox(_T("建立socket失败"));
		return 0;
	}
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(pServer->m_ServicePort);
	if (0 != bind(pServer->m_SockListen,(sockaddr *)&service,sizeof(sockaddr_in)))
	{
		AfxMessageBox(_T("绑定端口失败"));
		return 0;
	}
	if (0 != listen(pServer->m_SockListen,5))
	{
		AfxMessageBox(_T("监听端口失败"));
		return 0;
	}

	//提示建立socket成功
	pServer->EnableWindow(IDC_BUTTONEND,TRUE);
//	pServer->EnableWindow(IDC_BUTTONSEND,TRUE);
	pServer->EnableWindow(IDC_BUTTONSTART,FALSE);
	pServer->SetRevBoxText(_T("服务启动成功,开始监听端口"));
	Sb = NULL;
	//设置时间种子
	srand((unsigned int)time(NULL));
	//进入循环，监听端口
	while (TRUE)
	{
		
		if (socket_Select(pServer->m_SockListen, 100, TRUE))
		{
			sockaddr_in clientAddr;
			int iLen = sizeof(sockaddr_in);
			SOCKET accSock = accept(pServer->m_SockListen, (struct sockaddr*) & clientAddr, &iLen);
			if (accSock == INVALID_SOCKET)
			{
				continue;
			}
			//建立安全通信
		if(accSock){
			while (true)
			{
				if (DH_KeyExchange(accSock))
					if (Sb->toString().length() == 8)
						break;
			}
			//DES源密钥设定
			DES_Container.Set_The_Source_Key(Sb->toString());
			//DES子密钥生成
			DES_Container.Subkey_Generation();
		}
			//将节点加入链中
			CClientItem tItem;
			tItem.m_ClientSocket = accSock;
			tItem.m_strIp = inet_ntoa(clientAddr.sin_addr); //IP地址
			tItem.m_pMainWnd = pServer;
			int idx = pServer->m_ClientArray.Add(tItem); //idx是第x个连接的客户端
			tItem.m_hThread = CreateThread(NULL,0,ClientThreadProc, //创建一个线程并挂起:CREATE_SUSPENDED
				&(pServer->m_ClientArray.GetAt(idx)),CREATE_SUSPENDED,NULL); 
			pServer->m_ClientArray.GetAt(idx).m_hThread = tItem.m_hThread; 
			//等把hThread加入了节点，才开始执行线程，如下
			ResumeThread(tItem.m_hThread);
			pServer->SetRevBoxText(tItem.m_strIp + _T("上线-安全状态"));

			Sleep(100);
		}
	}
}
//bool SEC_STATUS = FALSE;
char BOB_KEY[1024];
#define MAX_BUFF 256
DWORD WINAPI ClientThreadProc(LPVOID Lparam){ //利用异步IO模型循环读取socket内的信息，并发送给各个用户
	USES_CONVERSION;
	CString strMsg;
	int  round = 0;
	CClientItem ClientItem = *(CClientItem *)Lparam;
	//SEC_STATUS = FALSE;
	while (TRUE)
	{
	
		if ( socket_Select(ClientItem.m_ClientSocket,100,TRUE))
		{
			char szRev[MAX_BUFF] = {0};
			char hash_data[41], hash_new[41];
			int iRet;
			
			iRet = recv(ClientItem.m_ClientSocket, hash_data, 41, 0);
			if (iRet< 0) {
				ClientItem.m_pMainWnd->SetRevBoxText(_T(">>Hash出错，消息可能被篡改。"));
			}
			string Encrypted_Data;
			string Decrypted_Data;
			iRet = recv(ClientItem.m_ClientSocket,szRev,sizeof(szRev),0);
			if (iRet > 0)
			{
				
				//######################解码######################
				//strMsg = A2T(szRev); //中文出现乱码，英文正常
//				strMsg.Format(_T("%s"),szRev); //这么写连英文都不对了
				Encrypted_Data = szRev;
				Decrypted_Data = DES_Container.Decryption(Encrypted_Data);
				My_HASH(Decrypted_Data, hash_new);
				//收到消息的解码位置
				strMsg = Decrypted_Data.c_str();
				//解码结束，密文已经是明文了
				//##############################################
				if (!strcmp(hash_data, hash_new))
					ClientItem.m_pMainWnd->SetRevBoxText(ClientItem.m_strIp + _T("(可信)>>") + strMsg);
				ClientItem.m_pMainWnd->SendClientMsg(strMsg,&ClientItem);
			}else{
				//失败  没收到消息  就直接显示就行
				strMsg = ClientItem.m_strIp + _T(" 已离开");
				ClientItem.m_pMainWnd->RemoveClientFromArray(ClientItem);
				ClientItem.m_pMainWnd->SetRevBoxText(strMsg);
				break;
			}
		}
	}
	return 0;
}

BOOL socket_Select(SOCKET hSocket,DWORD nTimeOut,BOOL bRead){
	FD_SET fdset;
	timeval tv;
	FD_ZERO(&fdset);
	FD_SET(hSocket,&fdset);
	nTimeOut = nTimeOut > 1000 ? 1000 : nTimeOut;
	tv.tv_sec = 0;
	tv.tv_usec = nTimeOut;
	int iRet = 0;
	if (bRead)
	{
		iRet = select(0,&fdset,NULL,NULL,&tv);
	} 
	else
	{
		iRet = select(0,NULL,&fdset,NULL,&tv);
	}
	if (iRet <= 0)
	{
		return FALSE;
	} 
	else if (FD_ISSET(hSocket,&fdset))
	{
		return TRUE;
	}
	return FALSE;
}



void Cxads_PCServerDlg::OnBnClickedButtonend()
{
	// TODO: 在此添加控件通知处理程序代码
	int AllClient = m_ClientArray.GetCount();
	for (int idx = 0 ; idx < AllClient ; idx++)
	{
		closesocket(m_ClientArray.GetAt(idx).m_ClientSocket);
	}
	m_ClientArray.RemoveAll();
	closesocket(m_SockListen);
	EnableWindow(IDC_BUTTONSEND,FALSE);
	EnableWindow(IDC_BUTTONSTART,TRUE);
	EnableWindow(IDC_BUTTONEND,FALSE);
	SetRevBoxText(_T("停止监听端口"));
	m_isServerOpen = FALSE;
}

//设置文本框文本
//接收到消息显示文本
void Cxads_PCServerDlg::SetRevBoxText(CString strMsg){
	m_EditRevBox.SetSel(-1,-1);

	m_EditRevBox.ReplaceSel(GetTime() + _T("\r\n  ") + strMsg + _T("\r\n"));
}

//客户端下线，从链表移除该节点
void Cxads_PCServerDlg::RemoveClientFromArray(CClientItem in_item){
	for (int idx = 0 ; idx < m_ClientArray.GetCount() ; idx++)
	{
		if (in_item.m_ClientSocket == m_ClientArray[idx].m_ClientSocket &&
			in_item.m_hThread == m_ClientArray[idx].m_hThread &&
			in_item.m_strIp == m_ClientArray[idx].m_strIp &&
			in_item.m_pMainWnd == m_ClientArray[idx].m_pMainWnd)
		{
			m_ClientArray.RemoveAt(idx);
		}
	}
	return;
}

CString GetTime(){
	SYSTEMTIME time;
	CString strTime;
	GetLocalTime(&time);
	strTime.Format(_T("%d-%02d-%02d %02d:%02d"),time.wYear,time.wMonth,time.wDay,
		time.wHour,time.wMinute);
	return strTime;
}



//退出按钮
void Cxads_PCServerDlg::OnBnClickedButtonquit()
{
	if (m_isServerOpen)
	{
		OnBnClickedButtonend();
	}
	SendMessage(WM_CLOSE);
	// TODO: 在此添加控件通知处理程序代码
}

void Cxads_PCServerDlg::SendClientMsg(CString strMsg,CClientItem * pWhoseItem)
{
	USES_CONVERSION;
	
	//############发送######################
	char hash_data[41];
	//发送前进行加密  即以获得对称密钥,建立安全连接之后
	string Statement;
	Statement = CW2A(strMsg.GetString());
	My_HASH(Statement, hash_data);
	send(m_ClientArray.GetAt(0).m_ClientSocket, hash_data, 41, 0);
	string Encrypted_Data = DES_Container.Encryption(Statement);
	//调用加密函数结束
	//####################################
	for (int i = 0 ; i < m_ClientArray.GetCount() ; i++)
	{
		if (!pWhoseItem || !equal(pWhoseItem,&(m_ClientArray.GetAt(i))))
		{
	//		send(m_ClientArray.GetAt(i).m_ClientSocket,szBuf,256,0);
			
			send(m_ClientArray.GetAt(i).m_ClientSocket, Encrypted_Data.c_str(), Encrypted_Data.length(), 0);
		}
	}
}

BOOL Cxads_PCServerDlg::equal(const CClientItem * p1 , const CClientItem * p2)
{
	if (p1->m_ClientSocket == p2->m_ClientSocket && p1->m_hThread == p2->m_hThread && p1->m_strIp == p2->m_strIp)
	{
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

//点击发送按钮
void Cxads_PCServerDlg::OnBnClickedButtonsend()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strMsg;
	GetDlgItemText(IDC_EDITSENDBOX,strMsg);
	//添加消息加密函數在SendClientMsg函数
	SendClientMsg(strMsg,NULL);
	SetRevBoxText(_T("服务器>>") + strMsg);
	SetDlgItemText(IDC_EDITSENDBOX,_T(""));
}


//根据发送框是否有内容判断按键是否激活
void Cxads_PCServerDlg::OnEnChangeEditsendbox()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString strMsg;
	GetDlgItemText(IDC_EDITSENDBOX,strMsg);
	if (strMsg == _T("") || !m_isServerOpen)
	{
		EnableWindow(IDC_BUTTONSEND,FALSE);
	}
	else{
		EnableWindow(IDC_BUTTONSEND,TRUE);
	}
}

BOOL Cxads_PCServerDlg::TrayMyIcon(BOOL isAdd)
{
	BOOL bReturn = FALSE;
	NOTIFYICONDATA tnd;
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = GetSafeHwnd();
	tnd.uID = IDR_MAINFRAME;
	if (TRUE == isAdd)
	{
		tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYICON_SERVER;
		tnd.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
		wcscpy_s(tnd.szTip,_countof(tnd.szTip),L"加密通信 - 服务端");
		ShowWindow(SW_HIDE);
		bReturn = Shell_NotifyIcon(NIM_ADD,&tnd);
	} 
	else
	{
		ShowWindow(SW_SHOWNA);
		SetForegroundWindow();
		bReturn = Shell_NotifyIcon(NIM_DELETE,&tnd);
	}
	return bReturn;
}

LRESULT Cxads_PCServerDlg::OnTrayCallbackMsg(WPARAM wparam , LPARAM lparam)
{
	CBitmap IconBitmapOn;
	CBitmap IconBitmapOff;
	CMenu mMenu, * pMenu = NULL;
//	CBitmap IconBitmapOn ,IconBitmapOff;
	IconBitmapOn.LoadBitmap(IDB_BITMAPON);
	IconBitmapOff.LoadBitmap(IDB_BITMAPOFF);
	if (m_isServerOpen)
	{
		mMenu.LoadMenu(IDR_MENU1);
		mMenu.SetMenuItemBitmaps(ID_MENU_SERVER,MF_BYCOMMAND | MF_STRING | MF_ENABLED,&IconBitmapOff,&IconBitmapOn);
		mMenu.ModifyMenu(ID_MENU_SERVER,MF_BYCOMMAND,ID_MENU_SERVER,_T("关闭服务器"));
	} 
	else
	{
		mMenu.LoadMenu(IDR_MENU1);
		mMenu.SetMenuItemBitmaps(ID_MENU_SERVER,MF_BYCOMMAND | MF_STRING | MF_ENABLED,&IconBitmapOn,&IconBitmapOff);
		mMenu.ModifyMenu(ID_MENU_SERVER,MF_BYCOMMAND,ID_MENU_SERVER,_T("打开服务器"));
	}
	switch(lparam){
	case WM_RBUTTONUP:
		{
			CPoint pt;
			GetCursorPos(&pt); //获得鼠标点击的位置
			pMenu = mMenu.GetSubMenu(0);
			SetForegroundWindow(); //不加本句则菜单弹出不消失
			pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,pt.x,pt.y,this);
			break;
		}
	case WM_LBUTTONDBLCLK:
		{
			ShowWindow(SW_RESTORE);
			SetForegroundWindow();
			TrayMyIcon(FALSE);
			break;
		}
	default:
		break;
	}
	return NULL;
}

//点击隐藏按钮
void Cxads_PCServerDlg::OnBnClickedButtonhide()
{
	ShowWindow(SW_MINIMIZE);
	Sleep(200);
	TrayMyIcon(TRUE);
}

//点击菜单上的显示
void Cxads_PCServerDlg::OnMenuShow()
{
	TrayMyIcon(FALSE);
	ShowWindow(SW_RESTORE);
	SetForegroundWindow();
}

//点击菜单上退出
void Cxads_PCServerDlg::OnMenuQuit()
{
	TrayMyIcon(FALSE);
	OnBnClickedButtonquit();
}

//点击菜单上打开/关闭服务器的选项
void Cxads_PCServerDlg::OnMenuServer()
{
	if (TRUE == m_isServerOpen)
	{
		OnBnClickedButtonend();
	} 
	else
	{
		OnBnClickedButtonstart();
	}

}

//当窗口大小改变时发送该消息
void Cxads_PCServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (SIZE_MINIMIZED == nType) //若是最小化，则隐藏到托盘
	{
		ShowWindow(SW_MINIMIZE);
		Sleep(200);
		TrayMyIcon(TRUE);
	}
}







