//
// Created by 杨将 on 2017/7/27.
//

#include "FileProtocol.h"
#include "SMDecoderError.h"
#include "common.h"
#include "IEnDeCrypt.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>

CFileProtocol::CFileProtocol()
{
	fp = NULL;
	m_crypt = NULL;
    m_fileInfo = NULL;
}

CFileProtocol::~CFileProtocol()
{
	close();
}

int CFileProtocol::open(const char * url, IEnDeCrypt * crypt)
{
	fp = fopen(url, "rb");
	if(NULL == fp)
	{
		return SM_PROTOCOL_PATH_OPEN;
	}

	m_crypt = crypt;

	return SM_PROTOCOL_SUCESS;
}
void CFileProtocol::setFileInfo(SMFileInfo *fileInfo){
    if (NULL == m_fileInfo) {
        m_fileInfo = (SMFileInfo *)malloc(sizeof(SMFileInfo));
    }
    if (NULL == fileInfo) {
        m_fileInfo->fileSize = 0;
        m_fileInfo->encrypt_method = 0;
    }else{
        m_fileInfo->fileSize = fileInfo->fileSize;
        m_fileInfo->encrypt_method = fileInfo->encrypt_method;
    }
}
SMFileInfo *CFileProtocol::getFileInfo()
{
    return m_fileInfo;
}
int CFileProtocol::read(unsigned char * buf, int size)
{
	int offset = ftell(fp);
	int num = fread(buf, 1, (size_t)size, fp);

	if(m_crypt)
	{
		m_crypt->decrypt(buf, buf, num, offset);
	}

	return num;
}

int CFileProtocol::write(const unsigned char * buf, int size)
{
	return 0;
}

int CFileProtocol::seek(int offset, int whence)
{
	if(0 != fseek(fp, offset, whence))
	{
		return -1;
	}
	else
	{
		return ftell(fp);
	}
}

void CFileProtocol::close()
{
	SAFE_CLOSE_FILE(fp);
	m_crypt = NULL;
    SAFE_FREE(m_fileInfo);
    m_fileInfo = NULL;
}
