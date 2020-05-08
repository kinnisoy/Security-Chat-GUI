#pragma once
#include<iostream>
#include<string>
#include<bitset>
#include<fstream>

using std::string;
using std::cin;
using std::endl;
using std::cout;
using std::fstream;
using std::bitset;
using std::ios_base;

class DES
{
private:
	bitset<64> plain_or_ciphertext;						//���Ļ�����
	bitset<64> The_source_key;							//�û������Դ��Կ
	bitset<48> K1_To_Kn[16];							//16������Կ

	bitset<64> CharToBit(const char str[8]);			//�ַ�תbit����
	bitset<28> Left_Shift(bitset<28> temp, int shift);	//ѭ�����ƺ���
	bitset<48> Compression_Permutation(bitset<56> temp);//ѹ������48λ��Կ
	bitset<48> E(bitset<32> temp);                      //λ��չ����

	void Initial_Permutation();							//��ʼIP�û�����
	void Initial_Permutation_Reverse();					//���ʼIP�û�����
	bitset<32> F(bitset<32> R, bitset<48> K);		    //F����

public:
	void Set_The_Source_Key(string Source_Key);			//Դ��Կ���ú���
	void Subkey_Generation();							//����Կ���ɺ���
	string Encryption(string Statement);					//���ܺ���
	string Decryption(string Statement);					//���ܺ���
	DES();
	~DES();
};

