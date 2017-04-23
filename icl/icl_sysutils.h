///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
// icl_sysutils.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _ICL_SYSUTILS_H_
#define _ICL_SYSUTILS_H_

#include "icl_config.h"
#include "zlib.h"

#ifdef ICL_WIN32
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <windows.h>
#endif

#ifdef ICL_LINUX
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#endif

#include "icl_types.h"

using namespace std; 

namespace icl
{

///////////////////////////////////////////////////////////////////////////////
// 提前声明

class CBuffer;

///////////////////////////////////////////////////////////////////////////////
// 常量定义

#ifdef ICL_WIN32
const char PATH_DELIM   = '\\';
const char DRIVER_DELIM = ':';
#endif
#ifdef ICL_LINUX
const char PATH_DELIM   = '/';
#endif

// 文件属性
const uint FA_READ_ONLY     = 0x00000001;
const uint FA_HIDDEN        = 0x00000002;
const uint FA_SYS_FILE      = 0x00000004;
const uint FA_VOLUME_ID     = 0x00000008;    // Win32 Only
const uint FA_DIRECTORY     = 0x00000010;
const uint FA_ARCHIVE       = 0x00000020;
const uint FA_SYM_LINK      = 0x00000040;    // Linux Only
const uint FA_ANY_FILE      = 0x0000003F;

///////////////////////////////////////////////////////////////////////////////
// 类型定义

// 文件查找记录
struct CFileFindRec
{
    int64 nFileSize;         // 文件大小
    string strFileName;      // 文件名(不含路径)
    uint nAttr;              // 文件属性
};

typedef vector<CFileFindRec> FileFindResult;

string EscapeURL(const string& strUrl);
string DeescapeURL(const string& strUrl);

bool IsxDigit(char nCh);
bool StrIsDigit(const string& strValue);

///////////////////////////////////////////////////////////////////////////////
// 杂项函数
string ReplaceAll(const string& strSrc,const string& strOldValue,const string& strNewValue);
string ReplaceAllDistinct(const string& strSrc,const string& strOldValue,const string& strNewValue);
wstring ReplaceAllDistinct(const wstring& strSrc,const wstring& strOldValue,const wstring& strNewValue);
wstring StrToWs(const string& strValue);
string WsToStr(const wstring& wstrValue);

char *WChToCh(const wchar_t *pWCh , char *pCh);
char *WStrToCStr(const wchar_t *pWStr , char *pCStr, size_t nLen);

//-----------------------------------------------------------------------------
//-- 字符串函数:
char *stristr(const char *String, const char *Pattern);
char *StrIStr( char *pSrc, const char *pDst);
bool EndSameText(const string& strValue, const string& strKey,bool bFlag=true);
bool StartSameText(const string& strValue, const string& strKey,bool bFlag=true);

bool IsInt(const string& str);
bool IsFloat(const string& str);

int StrToInt(const string& str, int nDefault = 0);
string IntToStr(int nValue);
double StrToFloat(const string& str, double fDefault = 0);
string FloatToStr(double fValue, const char *sFormat = "%f");

void FormatStringV(string& strResult, const char *sFormatString, va_list argList);
void FormatString(string& strResult, const char *sFormatString, ...);
string FormatString(const char *sFormatString, ...);

bool SameText(const string& str1, const string& str2);
string TrimString(const string& str);
string UpperCase(const string& str);
string LowerCase(const string& str);
string RepalceString(const string& strSource, const string& strOldPattern, 
	const string& strNewPattern, bool bReplaceAll = false, bool bCaseSensitive = true);
void SplitString(const string& strSource, const string& strSplitter, StringArray& StrList);
void SplitString(const string& strSource, char chSplitter, StringArray& StrList);
void SplitStringToInt(const string& strSource, char chSplitter, IntegerArray& IntList);
char* StrNCopy(char *pDest, const char *pSource, int nMaxBytes);
char* StrNZCopy(char *pDest, const char *pSource, int nDestSize);

//-----------------------------------------------------------------------------
//-- 文件和目录:

bool FileExists(const string& strFileName);
bool DirectoryExists(const string& strDir);
bool CreateDir(const string& strDir);
string ExtractFilePath(const string& strFileName);
string ExtractFileName(const string& strFileName);
bool ForceDirectories(string strDir);
bool DeleteFile(const string& strFileName);
int64 GetFileSize(const string& strFileName);
void FindFiles(const string& strPath, uint nAttr, FileFindResult& FindResult);
string PathWithSlash(const string& strPath);
string PathWithoutSlash(const string& strPath);

//-----------------------------------------------------------------------------
//-- 系统相关:

string SysErrorMessage(int nErrorCode);
void NanoSleep(double fSeconds, bool AllowInterrupt = true);
void USjwSleep(double fSeconds);
uint GetCurTicks();
uint GetTickDiff(uint nOldTicks, uint nNewTicks);

//-----------------------------------------------------------------------------
//-- 算法相关:

bool ZlibCompress(CBuffer& Buffer);
bool ZlibUncompress(CBuffer& Buffer);
bool HTTPGZibUncompress(CBuffer& Buffer);
int HTTPGZibUncompress(char* ,long,long*);

bool HttpDeflateUnCompress(CBuffer& Buffer);
int DeflateUnCompress(Byte *zdata, uLong nzdata,Byte *data, uLong *ndata);

/* Compress data */
int ZCompress(Bytef *data, uLong ndata,Bytef *zdata, uLong *nzdata);
/* Compress gzip data */
int GZCompress(Bytef *data, uLong ndata,Bytef *zdata, uLong *nzdata);

/* Uncompress data */
int ZDeCompress(Byte *zdata, uLong nzdata,Byte *data, uLong *ndata);

/* HTTP gzip decompress */
int HttpGzDeCompress(Byte *zdata, uLong nzdata,Byte *data, uLong *ndata);

/* HTTP gzip decompress */
int SjwHttpGzDeCompress(Byte *zdata, uLong nzdata,Byte *data, uLong *ndata);

/* Uncompress gzip data */
int GzDeCompress(Byte *zdata, uLong nzdata,Byte *data, uLong *ndata);

uint Crc32(uint nCrc, void *pBuffer, int nSize);
uint CalcBufferCrc32(void *pBuffer, int nSize);

string StrToBase64(const char *pData, int nSize);
string StrToBase16(const char *pData, int nSize);

//-----------------------------------------------------------------------------
//-- 其它函数:

void Randomize();
int GetRandom(int nMin, int nMax);

void MoveBuffer(const void *pSource, void *pDest, int nCount);
void ZeroBuffer(void *pBuffer, int nCount);
void FillBuffer(void *pBuffer, int nCount, byte nValue);

template <typename T>
inline const T& Min(const T& a, const T& b) { return ((a < b)? a : b); }

template <typename T>
inline const T& Max(const T& a, const T& b) { return ((a < b)? b : a); }

template <typename T>
inline const T& TrimValue(const T& Value, const T& Min, const T& Max)
    { return (Value > Max) ? Max : (Value < Min ? Min : Value); }

///////////////////////////////////////////////////////////////////////////////

} // namespace icl

#endif // _ICL_SYSUTILS_H_
