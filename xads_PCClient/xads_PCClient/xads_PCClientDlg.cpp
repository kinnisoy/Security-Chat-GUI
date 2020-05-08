
// xads_PCClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "xads_PCClient.h"
#include "xads_PCClientDlg.h"
#include "afxdialogex.h"
#include "RSA.h"
#include <string>

using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BigInteger* Sa; //DH协商的后的，DES对称密钥
DES DES_Container;     //DES对象

bool DH_KeyExchange(SOCKET S)
{
	//生成公钥和私钥
	BigInteger P(My_CrpytGenKey());
	BigInteger Self_Key(My_CrpytGenKey());

	//发送公钥给服务器端,接收服务器公钥
	string PublicKeyStr_B;
	if (S)
	{
		if (My_SendKey(S, P.toString().c_str()))
			PublicKeyStr_B = My_RecvKey(S);
	}
	else
		return FALSE;
	BigInteger Q(PublicKeyStr_B);

	//发送Ga，接收Gb,密钥计算
	string Gb_str;
	BigInteger* Ga = NULL;
	BigInteger* Gb = NULL;
	if (P > Q)
		Ga = new BigInteger(Q.modPow(Self_Key, P));
	else
		Ga = new BigInteger(P.modPow(Self_Key, Q));
	if (My_SendKey(S, Ga->toString().c_str()))
	{
		//接收Gb
		Gb_str = My_RecvKey(S);
		Gb = new BigInteger(Gb_str);
		if (P > Q)
			Sa = new BigInteger(Gb->modPow(Self_Key, P));
		else
			Sa = new BigInteger(Gb->modPow(Self_Key, Q));
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


// Cxads_PCClientDlg 对话框




Cxads_PCClientDlg::Cxads_PCClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cxads_PCClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ClientSock = NULL;
	isServerOn = FALSE;
}

void Cxads_PCClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITREVBOX, m_EditRevBox);
}

BEGIN_MESSAGE_MAP(Cxads_PCClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTONCONNECT, &Cxads_PCClientDlg::OnBnClickedButtonconnect)
	ON_BN_CLICKED(IDC_BUTTONSTOP, &Cxads_PCClientDlg::OnBnClickedButtonstop)
	ON_BN_CLICKED(IDC_BUTTONQUIT, &Cxads_PCClientDlg::OnBnClickedButtonquit)
	ON_BN_CLICKED(IDC_BUTTONSEND, &Cxads_PCClientDlg::OnBnClickedButtonsend)
	ON_WM_SETFOCUS()
	ON_WM_SETFOCUS()
	ON_EN_CHANGE(IDC_EDITSENDBOX, &Cxads_PCClientDlg::OnEnChangeEditsendbox)
END_MESSAGE_MAP()


// Cxads_PCClientDlg 消息处理程序

BOOL Cxads_PCClientDlg::OnInitDialog()
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
	SetDlgItemText(IDC_IPADDRESS,_T("127.0.0.1"));
	SetDlgItemText(IDC_EDITPORT,_T("8260"));
	::HideCaret(GetDlgItem(IDC_EDITREVBOX)->GetSafeHwnd());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cxads_PCClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Cxads_PCClientDlg::OnPaint()
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
HCURSOR Cxads_PCClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cxads_PCClientDlg::OnBnClickedButtonconnect()
{
	// TODO: 在此添加控件通知处理程序代码
	CreateThread(0,0,ConnectNetworkThread,this,0,NULL);

}

DWORD WINAPI ConnectNetworkThread(  LPVOID lpParameter)
{
	Cxads_PCClientDlg * pClient = (Cxads_PCClientDlg *)lpParameter;
	if(pClient->ConnectSocket(pClient))
	{
		
	}
	return 0;
}

#define MAX_BUFF 256
BOOL SEC_STATUS;
BOOL Cxads_PCClientDlg::ConnectSocket(Cxads_PCClientDlg * pClient)
{
	m_ClientSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (NULL == m_ClientSock)
	{
		MessageBox(_T("创建socket失败"));
		return FALSE;
	}
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	CString strIp;
	DWORD dPort = GetDlgItemInt(IDC_EDITPORT);
	GetDlgItemText(IDC_IPADDRESS,strIp);
	if (strIp == _T("0.0.0.0") || (dPort >= 65535 && dPort < 1024) || dPort == 0)
	{
		MessageBox(_T("请输入正确IP地址或端口"));
		return FALSE;
	}
	sa.sin_port = htons(dPort);
	char szIpAdd[32];
	USES_CONVERSION; //定义后才能使用T2A
	sprintf_s(szIpAdd,32,"%s",T2A(strIp));
	sa.sin_addr.S_un.S_addr = inet_addr(szIpAdd);
	if (SOCKET_ERROR == connect(m_ClientSock,(sockaddr *)&sa,sizeof(sa)))
	{
		MessageBox(_T("连接客户端错误,请检查你填写的IP和端口是否错误"));
		return FALSE;
	}

	pClient->SetRevBoxText(_T("连接服务器成功-正在加密信道......"));
	pClient->EnableWindow(IDC_BUTTONSTOP,TRUE);
	pClient->EnableWindow(IDC_BUTTONCONNECT,FALSE);
	isServerOn = TRUE;
	OnEnChangeEditsendbox();
	int flag;
	flag = 0;
	SEC_STATUS = FALSE;
	CString strMsg;
	Sa = NULL;
	//设置时间种子
	srand((unsigned int)time(NULL));
	if (m_ClientSock)
	{
		while (1)
		{
			if (DH_KeyExchange(m_ClientSock))
				if (Sa->toString().length() == 8)
					break;
		}
		//cout << "Key Exchange Succeed" << endl;
		pClient->SetRevBoxText(_T("DH密钥交换成功，进入安全通信状态"));
		//DES源密钥设定
		DES_Container.Set_The_Source_Key(Sa->toString());
		//DES子密钥生成
		DES_Container.Subkey_Generation();
	}
	while (TRUE)
	{
		if (socket_Select(m_ClientSock,100,TRUE))
		{
			char hash_data[41],hash_new[41];
			int iRead;
			char szMsg[MAX_BUFF] = {0};
			iRead = recv(m_ClientSock, hash_data, 41, 0);
			if (iRead < 0) {
				pClient->SetRevBoxText(strIp + _T(">>Hash出错，消息可能被篡改。") );
			}
			string Encrypted_Data;
			string Decrypted_Data;
			iRead = recv(m_ClientSock,szMsg, sizeof(szMsg),0);
			if (iRead > 0)
			{
				Encrypted_Data = szMsg;
				Decrypted_Data = DES_Container.Decryption(Encrypted_Data);
				My_HASH(Decrypted_Data, hash_new);
				strMsg = Decrypted_Data.c_str();
				//strMsg = szMsg;//这里不用改
				if (!strcmp(hash_data, hash_new)){
					pClient->SetRevBoxText(strIp + _T("(未篡改)>>") + strMsg);
				}	
				
			} 
			else
			{
				//收消息失败  就直接显示
				pClient->SetRevBoxText(_T("已断线，请重新连接"));
				isServerOn = FALSE;
				return TRUE;
			}
		}
	}
	return TRUE;
}

BOOL Cxads_PCClientDlg::EnableWindow(DWORD DlgId, BOOL bUsed)
{
	return GetDlgItem(DlgId)->EnableWindow(bUsed);
}

BOOL socket_Select(SOCKET hSocket,DWORD nTimeOut,BOOL bRead)
{
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


//客户端显示消息
void Cxads_PCClientDlg::SetRevBoxText(CString strMsg)
{
	m_EditRevBox.SetSel(-1,-1);
	m_EditRevBox.ReplaceSel(GetTime() + _T("\r\n  ") + strMsg + _T("\r\n"));
}



CString GetTime()
{
	SYSTEMTIME time;
	CString strTime;
	GetLocalTime(&time);
	strTime.Format(_T("%d-%02d-%02d %02d:%02d"),time.wYear,time.wMonth,time.wDay,
		time.wHour,time.wMinute);
	return strTime;
}

void Cxads_PCClientDlg::OnBnClickedButtonstop()
{
	// TODO: 在此添加控件通知处理程序代码
	EnableWindow(IDC_BUTTONCONNECT,TRUE);
	EnableWindow(IDC_BUTTONSEND,FALSE);
	EnableWindow(IDC_BUTTONSTOP,FALSE);
	closesocket(m_ClientSock);
	isServerOn = FALSE;
}


void Cxads_PCClientDlg::OnBnClickedButtonquit()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedButtonstop();
	SendMessage(WM_CLOSE);
}


void Cxads_PCClientDlg::OnBnClickedButtonsend()
{
	// TODO: 在此添加控件通知处理程序代码
	USES_CONVERSION;
	CString strGetMsg;
	int iWrite;
	GetDlgItemText(IDC_EDITSENDBOX,strGetMsg);
	//#################发送#############################
	//发送前加密的正确位置
	//定义新的CString变量保存加密的消息，用于发送
	char hash_data[41];
	string Statement;
	Statement = CW2A(strGetMsg.GetString());
	My_HASH(Statement, hash_data);
	send(m_ClientSock,hash_data, 41, 0);
	string Encrypted_Data = DES_Container.Encryption(Statement);
	iWrite = send(m_ClientSock, Encrypted_Data.c_str(), Encrypted_Data.length(), 0);
	//////调用加密函数结束
	//#############################################
	//iWrite = send(m_ClientSock,szBuf,256,0);
	if(SOCKET_ERROR == iWrite){
		SetRevBoxText(_T("发送错误"));
	}
	SetRevBoxText(_T("自己 >>") + strGetMsg);
	SetDlgItemText(IDC_EDITSENDBOX,_T(""));
	return; 
}

void Cxads_PCClientDlg::OnEnChangeEditsendbox()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString strMsg;
	GetDlgItemText(IDC_EDITSENDBOX,strMsg);
	if (_T("") == strMsg || !isServerOn)
	{
		EnableWindow(IDC_BUTTONSEND,FALSE);
	}
	else 
	{
		EnableWindow(IDC_BUTTONSEND,TRUE);
	}
}




