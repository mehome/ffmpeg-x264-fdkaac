//
// Created by 杨将 on 2017/7/25.
//

#ifndef __IPROTOCOL_H__
#define __IPROTOCOL_H__

typedef struct _SMFileInfo{
    int  encrypt_method;
    int  fileSize;
}SMFileInfo;
class IEnDeCrypt;

class IProtocol
{
public:
	IProtocol() { }
	virtual ~IProtocol() { }

public:
	//打开一个url地址，获取数据，返回错误码
	virtual int open(const char * url, IEnDeCrypt * crypt) = 0;
	//读取数据，返回读取到多少数据，如果出错或者读到结尾则返回0
	virtual int read(unsigned char * buf, int size) = 0;
	//写数据，返回还有多少数据没写，如果出错则返回-1
	virtual int write(const unsigned char * buf, int size) = 0;
	//跳转，返回跳转后的位置，如果失败则返回-1
	virtual int seek(int offset, int whence) = 0;
	//关闭url链接
	virtual void close() = 0;
    
    virtual void setFileInfo(SMFileInfo *fileInfo) = 0;
    virtual SMFileInfo *getFileInfo(void) = 0;
};

#endif //__IPROTOCOL_H__
