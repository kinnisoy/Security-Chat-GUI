#pragma once
#include "BigInteger.h"
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

using std::cout;
using std::string;

//��ʼ��
bool Initialization();

bool DH_KeyExchange(SOCKET S_New);

//��Կ����
BigInteger My_CrpytGenKey();

//��������
string My_RandKeyStr();

//������������
SOCKET My_Accept(SOCKET* TEMP);

//��Կ����ʱʹ�õķ��ͺ���
bool My_SendKey(SOCKET S, const char Data[]);

//��Կ����ʱʹ�õĽ��պ���
string My_RecvKey(SOCKET S);

//������Ϣhash
bool My_HASH(string Statement, char* hash);