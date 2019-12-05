//
// Created by 杨将 on 2017/7/27.
//

#ifndef __FILEPROTOCOL_H__
#define __FILEPROTOCOL_H__

#include "IProtocol.h"
#include <stdio.h>

class CFileProtocol : public IProtocol
{
public:
	CFileProtocol();
	virtual ~CFileProtocol();

public:
	virtual int open(const char * url, IEnDeCrypt * crypt);
	virtual int read(unsigned char * buf, int size);
	virtual int write(const unsigned char * buf, int size);
	virtual int seek(int offset, int whence);
	virtual void close();
    
    virtual void setFileInfo(SMFileInfo *fileInfo);
    virtual SMFileInfo *getFileInfo(void);
private:
	FILE * fp;
	IEnDeCrypt * m_crypt;
    SMFileInfo * m_fileInfo;
};

#endif //__FILEPROTOCOL_H__
