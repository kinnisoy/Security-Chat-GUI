// xads_PCServerDlg.cpp : ʵ���ļ�
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
	//���ɹ�Կ��˽Կ
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

	//����Ga,����Gb,��Կ����
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

		//����ͨ����Կ����
		if (P > Q)
			Sb = new BigInteger(Ga->modPow(Self_Key, P));
		else
			Sb = new BigInteger(Ga->modPow(Self_Key, Q));
	}
	return TRUE;
}


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// Cxads_PCServerDlg �Ի���




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


// Cxads_PCServerDlg ��Ϣ�������

BOOL Cxads_PCServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	EnableWindow(IDC_BUTTONSEND,FALSE);
	EnableWindow(IDC_BUTTONEND,FALSE);
	SetDlgItemText(IDC_EDITPORT,_T("8260"));
	GetDlgItem(IDC_EDITPORT)->SetFocus();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cxads_PCServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ServicePort = GetDlgItemInt(IDC_EDITPORT);
	if (m_ServicePort <= 1024 || m_ServicePort > 65535)
	{
		AfxMessageBox(_T("��������ʶ˿�"));
		SetDlgItemText(IDC_EDITPORT,_T(""));
		GetDlgItem(IDC_EDITPORT)->SetFocus();
		return ;
	}
	StartServer();
	m_isServerOpen = TRUE;
	OnEnChangeEditsendbox(); //������Ϳ���ԭ�����ݣ���ʹ���Ͱ�ť��Ч
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

//�ȴ��ͻ��˵����ӣ��������˾Ͱѿͻ�����Ϣ��������
DWORD WINAPI ListenThreadFunc(LPVOID Lparam)
{
	Cxads_PCServerDlg * pServer = (Cxads_PCServerDlg *)Lparam;;
	if (INVALID_SOCKET == (pServer->m_SockListen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)))
	{
		AfxMessageBox(_T("����socketʧ��"));
		return 0;
	}
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(pServer->m_ServicePort);
	if (0 != bind(pServer->m_SockListen,(sockaddr *)&service,sizeof(sockaddr_in)))
	{
		AfxMessageBox(_T("�󶨶˿�ʧ��"));
		return 0;
	}
	if (0 != listen(pServer->m_SockListen,5))
	{
		AfxMessageBox(_T("�����˿�ʧ��"));
		return 0;
	}

	//��ʾ����socket�ɹ�
	pServer->EnableWindow(IDC_BUTTONEND,TRUE);
//	pServer->EnableWindow(IDC_BUTTONSEND,TRUE);
	pServer->EnableWindow(IDC_BUTTONSTART,FALSE);
	pServer->SetRevBoxText(_T("���������ɹ�,��ʼ�����˿�"));
	Sb = NULL;
	//����ʱ������
	srand((unsigned int)time(NULL));
	//����ѭ���������˿�
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
			//������ȫͨ��
		if(accSock){
			while (true)
			{
				if (DH_KeyExchange(accSock))
					if (Sb->toString().length() == 8)
						break;
			}
			//DESԴ��Կ�趨
			DES_Container.Set_The_Source_Key(Sb->toString());
			//DES����Կ����
			DES_Container.Subkey_Generation();
		}
			//���ڵ��������
			CClientItem tItem;
			tItem.m_ClientSocket = accSock;
			tItem.m_strIp = inet_ntoa(clientAddr.sin_addr); //IP��ַ
			tItem.m_pMainWnd = pServer;
			int idx = pServer->m_ClientArray.Add(tItem); //idx�ǵ�x�����ӵĿͻ���
			tItem.m_hThread = CreateThread(NULL,0,ClientThreadProc, //����һ���̲߳�����:CREATE_SUSPENDED
				&(pServer->m_ClientArray.GetAt(idx)),CREATE_SUSPENDED,NULL); 
			pServer->m_ClientArray.GetAt(idx).m_hThread = tItem.m_hThread; 
			//�Ȱ�hThread�����˽ڵ㣬�ſ�ʼִ���̣߳�����
			ResumeThread(tItem.m_hThread);
			pServer->SetRevBoxText(tItem.m_strIp + _T("����-��ȫ״̬"));

			Sleep(100);
		}
	}
}
//bool SEC_STATUS = FALSE;
char BOB_KEY[1024];
#define MAX_BUFF 256
DWORD WINAPI ClientThreadProc(LPVOID Lparam){ //�����첽IOģ��ѭ����ȡsocket�ڵ���Ϣ�������͸������û�
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
				ClientItem.m_pMainWnd->SetRevBoxText(_T(">>Hash������Ϣ���ܱ��۸ġ�"));
			}
			string Encrypted_Data;
			string Decrypted_Data;
			iRet = recv(ClientItem.m_ClientSocket,szRev,sizeof(szRev),0);
			if (iRet > 0)
			{
				
				//######################����######################
				//strMsg = A2T(szRev); //���ĳ������룬Ӣ������
//				strMsg.Format(_T("%s"),szRev); //��ôд��Ӣ�Ķ�������
				Encrypted_Data = szRev;
				Decrypted_Data = DES_Container.Decryption(Encrypted_Data);
				My_HASH(Decrypted_Data, hash_new);
				//�յ���Ϣ�Ľ���λ��
				strMsg = Decrypted_Data.c_str();
				//��������������Ѿ���������
				//##############################################
				if (!strcmp(hash_data, hash_new))
					ClientItem.m_pMainWnd->SetRevBoxText(ClientItem.m_strIp + _T("(����)>>") + strMsg);
				ClientItem.m_pMainWnd->SendClientMsg(strMsg,&ClientItem);
			}else{
				//ʧ��  û�յ���Ϣ  ��ֱ����ʾ����
				strMsg = ClientItem.m_strIp + _T(" ���뿪");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	SetRevBoxText(_T("ֹͣ�����˿�"));
	m_isServerOpen = FALSE;
}

//�����ı����ı�
//���յ���Ϣ��ʾ�ı�
void Cxads_PCServerDlg::SetRevBoxText(CString strMsg){
	m_EditRevBox.SetSel(-1,-1);

	m_EditRevBox.ReplaceSel(GetTime() + _T("\r\n  ") + strMsg + _T("\r\n"));
}

//�ͻ������ߣ��������Ƴ��ýڵ�
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



//�˳���ť
void Cxads_PCServerDlg::OnBnClickedButtonquit()
{
	if (m_isServerOpen)
	{
		OnBnClickedButtonend();
	}
	SendMessage(WM_CLOSE);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void Cxads_PCServerDlg::SendClientMsg(CString strMsg,CClientItem * pWhoseItem)
{
	USES_CONVERSION;
	
	//############����######################
	char hash_data[41];
	//����ǰ���м���  ���Ի�öԳ���Կ,������ȫ����֮��
	string Statement;
	Statement = CW2A(strMsg.GetString());
	My_HASH(Statement, hash_data);
	send(m_ClientArray.GetAt(0).m_ClientSocket, hash_data, 41, 0);
	string Encrypted_Data = DES_Container.Encryption(Statement);
	//���ü��ܺ�������
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

//������Ͱ�ť
void Cxads_PCServerDlg::OnBnClickedButtonsend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strMsg;
	GetDlgItemText(IDC_EDITSENDBOX,strMsg);
	//�����Ϣ���ܺ�����SendClientMsg����
	SendClientMsg(strMsg,NULL);
	SetRevBoxText(_T("������>>") + strMsg);
	SetDlgItemText(IDC_EDITSENDBOX,_T(""));
}


//���ݷ��Ϳ��Ƿ��������жϰ����Ƿ񼤻�
void Cxads_PCServerDlg::OnEnChangeEditsendbox()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
		wcscpy_s(tnd.szTip,_countof(tnd.szTip),L"����ͨ�� - �����");
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
		mMenu.ModifyMenu(ID_MENU_SERVER,MF_BYCOMMAND,ID_MENU_SERVER,_T("�رշ�����"));
	} 
	else
	{
		mMenu.LoadMenu(IDR_MENU1);
		mMenu.SetMenuItemBitmaps(ID_MENU_SERVER,MF_BYCOMMAND | MF_STRING | MF_ENABLED,&IconBitmapOn,&IconBitmapOff);
		mMenu.ModifyMenu(ID_MENU_SERVER,MF_BYCOMMAND,ID_MENU_SERVER,_T("�򿪷�����"));
	}
	switch(lparam){
	case WM_RBUTTONUP:
		{
			CPoint pt;
			GetCursorPos(&pt); //����������λ��
			pMenu = mMenu.GetSubMenu(0);
			SetForegroundWindow(); //���ӱ�����˵���������ʧ
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

//������ذ�ť
void Cxads_PCServerDlg::OnBnClickedButtonhide()
{
	ShowWindow(SW_MINIMIZE);
	Sleep(200);
	TrayMyIcon(TRUE);
}

//����˵��ϵ���ʾ
void Cxads_PCServerDlg::OnMenuShow()
{
	TrayMyIcon(FALSE);
	ShowWindow(SW_RESTORE);
	SetForegroundWindow();
}

//����˵����˳�
void Cxads_PCServerDlg::OnMenuQuit()
{
	TrayMyIcon(FALSE);
	OnBnClickedButtonquit();
}

//����˵��ϴ�/�رշ�������ѡ��
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

//�����ڴ�С�ı�ʱ���͸���Ϣ
void Cxads_PCServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (SIZE_MINIMIZED == nType) //������С���������ص�����
	{
		ShowWindow(SW_MINIMIZE);
		Sleep(200);
		TrayMyIcon(TRUE);
	}
}







