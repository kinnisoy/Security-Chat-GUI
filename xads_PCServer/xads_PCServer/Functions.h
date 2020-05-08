#pragma once
#include "BigInteger.h"
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

using std::cout;
using std::string;

//初始化
bool Initialization();

bool DH_KeyExchange(SOCKET S_New);

//密钥生成
BigInteger My_CrpytGenKey();

//大数生成
string My_RandKeyStr();

//接收连接请求
SOCKET My_Accept(SOCKET* TEMP);

//密钥交换时使用的发送函数
bool My_SendKey(SOCKET S, const char Data[]);

//密钥交换时使用的接收函数
string My_RecvKey(SOCKET S);

//计算消息hash
bool My_HASH(string Statement, char* hash);