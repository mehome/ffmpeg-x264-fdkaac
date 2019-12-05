//
// Created by 杨将 on 2017/8/16.
//

#ifndef __IENDECRYPT_H__
#define __IENDECRYPT_H__

class IEnDeCrypt
{
public:
	IEnDeCrypt() { }
	virtual ~IEnDeCrypt() { }

public:
	virtual int init(const unsigned char * key, int size) = 0;
	virtual void encrypt(unsigned char * input,
						 unsigned char * output, int length, int offset) = 0;
	virtual void decrypt(unsigned char * input,
						 unsigned char * output, int length, int offset) = 0;
	virtual void uninit() = 0;
};

#endif //__IENDECRYPT_H__
