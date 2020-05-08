#include "stdafx.h"
#include "DES.h"
#include"DES_Global_Variable.h"


DES::DES(){}

DES::~DES() {}

bitset<64> DES::CharToBit(const char str[8]) //�ַ�תbit����
{
	bitset<64> key;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			key[i * 8 + 7 - j] = ((str[i] >> j) & 1);
		}
	}
	return key;
}

bitset<28> DES::Left_Shift(bitset<28> temp, int shift)	//ѭ�����ƺ���
{
	bitset<28> key_28;
	for (int i = 0; i < 28; i++)
	{
		if (i - shift < 0)
		{
			key_28[i] = temp[i - shift + 28];
		}
		else
		{
			key_28[i] = temp[i - shift];
		}
	}
	return key_28;
}

bitset<48> DES::Compression_Permutation(bitset<56> temp)//ѹ������48λ����Կ 
{
	bitset<48> subkey;
	for (int i = 0; i < 48; i++)
	{
		subkey[i] = temp[PC_2[i] - 1];
	}
	return subkey;
}

void DES::Set_The_Source_Key(string Source_Key)			//Դ��Կ���ú���
{
	if (Source_Key.length() != 8)
	{
		cout << "Source Key Lengh Error!!" << endl;
		cin.get();
		exit(1);
	}

	The_source_key = CharToBit(Source_Key.c_str());
}

void DES::Subkey_Generation()			//����Կ���ɺ���
{
	bitset<56> key_56;
	bitset<28> key_28_before;
	bitset<28> key_28_after;

	for (int i = 0; i < 56; i++)		//ȥ��У��λ
	{
		key_56[i] = The_source_key[PC_1[i] - 1];
	}

	for (int i = 0; i < 28; i++)		//Cn
	{
		key_28_before[i] = key_56[i];
	}

	for (int i = 0; i < 28; i++)		//Dn
	{
		key_28_after[i] = key_56[i + 28];
	}

	for (int n = 0; n < 16; n++)			//16������Կ����
	{
		key_28_before = Left_Shift(key_28_before, shiftBits[n]);  //Cnѭ������
		key_28_after = Left_Shift(key_28_after, shiftBits[n]);	  //Dnѭ������

		for (int i = 0; i < 56; i++)		//Cn��Dn�ϲ�
		{
			if (i < 28)
			{
				key_56[i] = key_28_before[i];
			}
			else
			{
				key_56[i] = key_28_after[i - 28];
			}
		}
		K1_To_Kn[n] = Compression_Permutation(key_56);
	}
}

void DES::Initial_Permutation()			//��ʼIP�û�����
{
	bitset<64> temp = plain_or_ciphertext;
	for (int i = 0; i < 64; i++)
	{
		plain_or_ciphertext[i] = temp[IP[i] - 1];
	}
}

void DES::Initial_Permutation_Reverse() //���ʼIP�û�����
{
	bitset<64> temp = plain_or_ciphertext;
	for (int i = 0; i < 64; i++)
	{
		plain_or_ciphertext[i] = temp[IP_R[i] - 1];
	}
}

bitset<48> DES::E(bitset<32> R)      //λ��չ����
{
	bitset<48> temp;
	for (int i = 0; i < 48; i++)
	{
		temp[i] = R[E_[i] - 1];
	}
	return temp;
}

bitset<32> DES::F(bitset<32> R, bitset<48> K)	//F����
{
	bitset<48> temp;
	bitset<6> S[8];
	bitset<32> Rx;
	bitset<32> Rx_temp;
	bitset<4> Integer[8];

	temp = E(R);					    //����λ��չ

	for (int i = 0; i < 48; i++)		//��λ���
	{
		temp[i] = (temp[i] ^ K[i]);
	}

	for (int i = 0; i < 8; i++)			//6bitһ�飬�ֳ�8��
	{
		for (int j = 0; j < 6; j++)
		{
			S[i][j] = temp[i * 6 + j];
		}
	}

	for (int i = 0; i < 8; i++)		    //S��������bits
	{
		Integer[i] = S_BOX[i][S[i][0] * 2 + S[i][5]][S[i][1] * 8 + S[i][2] * 4 + S[i][3] * 2 + S[i][4]];
	}

	for (int i = 0; i < 8; i++)		    //S���������λ��32λ
	{
		for (int j = 0; j < 4; j++)
		{
			Rx[i * 4 + j] = Integer[i][j];
		}
	}

	Rx_temp = Rx;						//P�û�
	for (int i = 0; i < 32; i++)
	{
		Rx[i] = Rx_temp[P[i] - 1];
	}

	return Rx;
}

string DES::Encryption(string Statement)		//���ܺ���
{
	bitset<32> Ln;
	bitset<32> Rn;
	bitset<32> temp;
	bitset<32> temp_x;

	string Str_Container_Eight;
	string Encrypted_Statement;

	while (!Statement.empty())
	{
		if (Statement.length() >= 8)
		{
			for (int i = 0; i < 8; i++)
				Str_Container_Eight.push_back(Statement[i]);
			Statement.erase(0, 8);
		}
		else
		{
			for (int i = 0; i < Statement.length(); i++)
				Str_Container_Eight.push_back(Statement[i]);
			Statement.erase(0, Statement.length());
		}

		strcpy((char*)&plain_or_ciphertext, Str_Container_Eight.c_str());

		Initial_Permutation();              //��ʼ�û�IP

		for (int i = 0; i < 32; i++)		//����L0
			Ln[i] = plain_or_ciphertext[i];

		for (int i = 0; i < 32; i++)		//����R0
			Rn[i] = plain_or_ciphertext[i + 32];

		for (int n = 0; n < 16; n++)		//����16��
		{
			temp_x = Rn;
			temp = F(Rn, K1_To_Kn[n]);
			for (int i = 0; i < 32; i++)
				Rn[i] = (Ln[i] ^ temp[i]);
			Ln = temp_x;
		}

		for (int i = 0; i < 32; i++)
			plain_or_ciphertext[i] = Rn[i];

		for (int i = 0; i < 32; i++)
			plain_or_ciphertext[i + 32] = Ln[i];

		Initial_Permutation_Reverse();

		Encrypted_Statement += (char*)&plain_or_ciphertext;

		Str_Container_Eight.clear();
		plain_or_ciphertext.reset();        //����
	}

	return Encrypted_Statement;
}

string DES::Decryption(string Statement)					//���ܺ���
{
	bitset<32> Ln;
	bitset<32> Rn;
	bitset<32> temp;
	bitset<32> temp_x;

	string Str_Container_Eight;
	string Decrypted_Statement;

	while (!Statement.empty())
	{
		if (Statement.length() >= 8)
		{
			for (int i = 0; i < 8; i++)
				Str_Container_Eight.push_back(Statement[i]);
			Statement.erase(0, 8);
		}
		else
		{
			for (int i = 0; i < Statement.length(); i++)
				Str_Container_Eight.push_back(Statement[i]);
			Statement.erase(0, Statement.length());
		}

		strcpy((char*)&plain_or_ciphertext, Str_Container_Eight.c_str());

		Initial_Permutation();              //��ʼ�û�IP

		for (int i = 0; i < 32; i++)		//����L0
			Ln[i] = plain_or_ciphertext[i];

		for (int i = 0; i < 32; i++)		//����R0
			Rn[i] = plain_or_ciphertext[i + 32];

		for (int n = 0; n < 16; n++)		//����16��
		{
			temp_x = Rn;
			temp = F(Rn, K1_To_Kn[15 - n]);
			for (int i = 0; i < 32; i++)
			{
				Rn[i] = (Ln[i] ^ temp[i]);
			}
			Ln = temp_x;
		}

		for (int i = 0; i < 32; i++)
			plain_or_ciphertext[i] = Rn[i];

		for (int i = 0; i < 32; i++)
			plain_or_ciphertext[i + 32] = Ln[i];

		Initial_Permutation_Reverse();

		Decrypted_Statement += (char*)&plain_or_ciphertext;
		Str_Container_Eight.clear();
		plain_or_ciphertext.reset();        //����
	}

	return Decrypted_Statement;
}