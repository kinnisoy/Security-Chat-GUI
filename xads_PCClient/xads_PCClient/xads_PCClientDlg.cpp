
// xads_PCClientDlg.cpp : ʵ���ļ�
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

BigInteger* Sa; //DHЭ�̵ĺ�ģ�DES�Գ���Կ
DES DES_Container;     //DES����

bool DH_KeyExchange(SOCKET S)
{
	//���ɹ�Կ��˽Կ
	BigInteger P(My_CrpytGenKey());
	BigInteger Self_Key(My_CrpytGenKey());

	//���͹�Կ����������,���շ�������Կ
	string PublicKeyStr_B;
	if (S)
	{
		if (My_SendKey(S, P.toString().c_str()))
			PublicKeyStr_B = My_RecvKey(S);
	}
	else
		return FALSE;
	BigInteger Q(PublicKeyStr_B);

	//����Ga������Gb,��Կ����
	string Gb_str;
	BigInteger* Ga = NULL;
	BigInteger* Gb = NULL;
	if (P > Q)
		Ga = new BigInteger(Q.modPow(Self_Key, P));
	else
		Ga = new BigInteger(P.modPow(Self_Key, Q));
	if (My_SendKey(S, Ga->toString().c_str()))
	{
		//����Gb
		Gb_str = My_RecvKey(S);
		Gb = new BigInteger(Gb_str);
		if (P > Q)
			Sa = new BigInteger(Gb->modPow(Self_Key, P));
		else
			Sa = new BigInteger(Gb->modPow(Self_Key, Q));
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


// Cxads_PCClientDlg �Ի���




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


// Cxads_PCClientDlg ��Ϣ�������

BOOL Cxads_PCClientDlg::OnInitDialog()
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
	SetDlgItemText(IDC_IPADDRESS,_T("127.0.0.1"));
	SetDlgItemText(IDC_EDITPORT,_T("8260"));
	::HideCaret(GetDlgItem(IDC_EDITREVBOX)->GetSafeHwnd());

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cxads_PCClientDlg::OnPaint()
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
HCURSOR Cxads_PCClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cxads_PCClientDlg::OnBnClickedButtonconnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		MessageBox(_T("����socketʧ��"));
		return FALSE;
	}
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	CString strIp;
	DWORD dPort = GetDlgItemInt(IDC_EDITPORT);
	GetDlgItemText(IDC_IPADDRESS,strIp);
	if (strIp == _T("0.0.0.0") || (dPort >= 65535 && dPort < 1024) || dPort == 0)
	{
		MessageBox(_T("��������ȷIP��ַ��˿�"));
		return FALSE;
	}
	sa.sin_port = htons(dPort);
	char szIpAdd[32];
	USES_CONVERSION; //��������ʹ��T2A
	sprintf_s(szIpAdd,32,"%s",T2A(strIp));
	sa.sin_addr.S_un.S_addr = inet_addr(szIpAdd);
	if (SOCKET_ERROR == connect(m_ClientSock,(sockaddr *)&sa,sizeof(sa)))
	{
		MessageBox(_T("���ӿͻ��˴���,��������д��IP�Ͷ˿��Ƿ����"));
		return FALSE;
	}

	pClient->SetRevBoxText(_T("���ӷ������ɹ�-���ڼ����ŵ�......"));
	pClient->EnableWindow(IDC_BUTTONSTOP,TRUE);
	pClient->EnableWindow(IDC_BUTTONCONNECT,FALSE);
	isServerOn = TRUE;
	OnEnChangeEditsendbox();
	int flag;
	flag = 0;
	SEC_STATUS = FALSE;
	CString strMsg;
	Sa = NULL;
	//����ʱ������
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
		pClient->SetRevBoxText(_T("DH��Կ�����ɹ������밲ȫͨ��״̬"));
		//DESԴ��Կ�趨
		DES_Container.Set_The_Source_Key(Sa->toString());
		//DES����Կ����
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
				pClient->SetRevBoxText(strIp + _T(">>Hash������Ϣ���ܱ��۸ġ�") );
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
				//strMsg = szMsg;//���ﲻ�ø�
				if (!strcmp(hash_data, hash_new)){
					pClient->SetRevBoxText(strIp + _T("(δ�۸�)>>") + strMsg);
				}	
				
			} 
			else
			{
				//����Ϣʧ��  ��ֱ����ʾ
				pClient->SetRevBoxText(_T("�Ѷ��ߣ�����������"));
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


//�ͻ�����ʾ��Ϣ
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	EnableWindow(IDC_BUTTONCONNECT,TRUE);
	EnableWindow(IDC_BUTTONSEND,FALSE);
	EnableWindow(IDC_BUTTONSTOP,FALSE);
	closesocket(m_ClientSock);
	isServerOn = FALSE;
}


void Cxads_PCClientDlg::OnBnClickedButtonquit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnBnClickedButtonstop();
	SendMessage(WM_CLOSE);
}


void Cxads_PCClientDlg::OnBnClickedButtonsend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	USES_CONVERSION;
	CString strGetMsg;
	int iWrite;
	GetDlgItemText(IDC_EDITSENDBOX,strGetMsg);
	//#################����#############################
	//����ǰ���ܵ���ȷλ��
	//�����µ�CString����������ܵ���Ϣ�����ڷ���
	char hash_data[41];
	string Statement;
	Statement = CW2A(strGetMsg.GetString());
	My_HASH(Statement, hash_data);
	send(m_ClientSock,hash_data, 41, 0);
	string Encrypted_Data = DES_Container.Encryption(Statement);
	iWrite = send(m_ClientSock, Encrypted_Data.c_str(), Encrypted_Data.length(), 0);
	//////���ü��ܺ�������
	//#############################################
	//iWrite = send(m_ClientSock,szBuf,256,0);
	if(SOCKET_ERROR == iWrite){
		SetRevBoxText(_T("���ʹ���"));
	}
	SetRevBoxText(_T("�Լ� >>") + strGetMsg);
	SetDlgItemText(IDC_EDITSENDBOX,_T(""));
	return; 
}

void Cxads_PCClientDlg::OnEnChangeEditsendbox()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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




