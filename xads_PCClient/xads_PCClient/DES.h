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
	bitset<64> plain_or_ciphertext;						//明文或密文
	bitset<64> The_source_key;							//用户输入的源密钥
	bitset<48> K1_To_Kn[16];							//16轮子密钥

	bitset<64> CharToBit(const char str[8]);			//字符转bit函数
	bitset<28> Left_Shift(bitset<28> temp, int shift);	//循环左移函数
	bitset<48> Compression_Permutation(bitset<56> temp);//压缩生成48位密钥
	bitset<48> E(bitset<32> temp);                      //位扩展函数

	void Initial_Permutation();							//初始IP置换函数
	void Initial_Permutation_Reverse();					//逆初始IP置换函数
	bitset<32> F(bitset<32> R, bitset<48> K);		    //F函数

public:
	void Set_The_Source_Key(string Source_Key);			//源密钥设置函数
	void Subkey_Generation();							//子密钥生成函数
	string Encryption(string Statement);					//加密函数
	string Decryption(string Statement);					//解密函数
	DES();
	~DES();
};

