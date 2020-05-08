#include "stdafx.h"
#include "Functions.h"
#include "DES.h"
#include <stdlib.h>
#include <time.h>
#include "windows.h"
#include "Wincrypt.h"

#pragma comment(lib,"crypt32.lib")


bool Initialization()
{
	//设置时间种子
	srand((unsigned int)time(NULL));

	//初始化动态链接库
	WSADATA WSAData;
	if (WSAStartup(2.2, &WSAData))
	{
		return FALSE;
	}
	return TRUE;
}





SOCKET My_Accept(SOCKET* TEMP)
{
	//创建套接字
	SOCKET S = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);
	if (S == INVALID_SOCKET)
	{
		cout << GetLastError();
		return FALSE;
	}

	sockaddr_in Addr;
	ZeroMemory(&Addr, sizeof(sockaddr_in));
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(8888);
	Addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (bind(S, (sockaddr*)&Addr, sizeof(Addr)) != SOCKET_ERROR)
	{
		if (listen(S, 1) != SOCKET_ERROR)
		{
			sockaddr_in Client_Addr;
			int size = sizeof(sockaddr_in);
			ZeroMemory(&Client_Addr, sizeof(sockaddr_in));
			SOCKET S_New = accept(S, (sockaddr*)&Client_Addr, &size);
			if (S_New != INVALID_SOCKET)
			{
				*TEMP = S;
				return S_New;
			}
		}
	}

	closesocket(S);
	return FALSE;
}


bool My_SendKey(SOCKET S, const char Data[])
{
	if (send(S, Data, strlen(Data), 0) != SOCKET_ERROR)
		return TRUE;
	else
		return FALSE;
}


string My_RecvKey(SOCKET S)
{
	string KeyStr;
	char buffer[9] = { 0 };
	recv(S, buffer, 9, 0);
	if (strlen(buffer) == 8)
	{
		for (int index = 0; index < 8; index++)
		{
			KeyStr.push_back(buffer[index]);
		}
	}
	return KeyStr;
}


BigInteger My_CrpytGenKey()
{
	while (1)
	{
		int Num_OF_Access = 0;

		//费马素性检验
		string Hex_Number_P = My_RandKeyStr();
		BigInteger P(Hex_Number_P);
		for (int i = 0; i < 20; i++)
		{
			string Hex_Number_A = My_RandKeyStr();
			BigInteger A(Hex_Number_A);
			if ((A >= A.TWO) && (A <= P - P.TWO))
			{
				BigInteger Container(A.modInverse(P));
				if (Container != Container.ZERO)
				{
					BigInteger Test(A.modPow(P - P.ONE, P));
					if (Test == Test.ONE)
						Num_OF_Access++;
					else
						break;
				}
				else
					break;
			}
			else
				i--;
		}
		if (Num_OF_Access == 20)
			return P;
	}
}


string My_RandKeyStr()
{
	//随机生成16进制字符串用于生成大数P
	string Hex_Str;
	string Hex_Number = "0123456789ABCDEF";

	for (int i = 0; i < 8; i++)
	{
		unsigned int index = rand() % 16;
		Hex_Str.push_back(Hex_Number[index]);
	}
	return Hex_Str;
}


bool My_HASH(string Statement, char* hash)
{
	string Hash_256;
	HCRYPTPROV CSP;
	HCRYPTHASH HHash;

	char hash_data[41];
	DWORD hash_len = 41;
	//CryptAcquireContextA(&CSP, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	if (!CryptAcquireContextA(&CSP, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		return FALSE;
	else if (!CryptCreateHash(CSP, CALG_SHA1, 0, 0, &HHash))
		return FALSE;
	else if (!CryptHashData(HHash, (const BYTE*)Statement.c_str(), Statement.length(), 0))
		return FALSE;
	else if (!CryptGetHashParam(HHash, HP_HASHVAL, (BYTE*)hash_data, &hash_len, 0))
		return FALSE;

	for (int i = 0; i <= hash_len - 1; i++)
	{
		int hash_bit = hash_data[i];
		int first = (hash_bit & 0xf0) >> 4;
		int second = hash_bit & 0x0f;
		char tmp[2];
		itoa(first, tmp, 16);
		hash[i * 2] = tmp[0];
		itoa(second, tmp, 16);
		hash[i * 2 + 1] = tmp[0];
	}
	hash[40] = '\0';

	return TRUE;
}