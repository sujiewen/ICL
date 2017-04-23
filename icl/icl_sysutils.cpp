///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
//
// �ļ�����: icl_sysutils.cpp
// ��������: ϵͳ�����
// ����޸�: 2005-12-06
///////////////////////////////////////////////////////////////////////////////

#include "icl_sysutils.h"
#include "icl_classes.h"

namespace icl
{

char Dec2hexChar(short int n)
{
    if ( 0 <= n && n <= 9 )
    {
        return char( short('0') + n );
    }
    else if ( 10 <= n && n <= 15 )
    {
        return char( short('A') + n - 10 );
    }
    else
    {
        return char(0);
    }
}

short int HexChar2dec(char c)
{
    if ( '0'<=c && c<='9' )
    {
        return short(c-'0');
    }
    else if ( 'a'<=c && c<='f' )
    {
        return ( short(c-'a') + 10 );
    }
    else if ( 'A'<=c && c<='F' )
    {
        return ( short(c-'A') + 10 );
    }
    else {
        return -1;
    }
}

string EscapeURL(const string &URL)
{
    string result = "";
    for ( unsigned int i=0; i<URL.size(); i++ )
    {
    char c = URL[i];
    if (( '0'<=c && c<='9' ) || ( 'a'<=c && c<='z' ) || ( 'A'<=c && c<='Z' ) ||c=='/' || c=='.')
    {
        result += c;
    }
    else
    {
        int j = (short int)c;
        if ( j < 0 )
        {
            j += 256;
        }
        int i1, i0;
        i1 = j / 16;
        i0 = j - i1*16;
        result += '%';
        result += Dec2hexChar(i1);
        result += Dec2hexChar(i0);
    }
    }
    return result;
}

string DeescapeURL(const string &URL)
{
    string result = "";
    for ( unsigned int i=0; i<URL.size(); i++ )
    {
        char c = URL[i];
        if ( c != '%' )
        {
        result += c;
        }
        else
        {
            char c1 = URL[++i];
            char c0 = URL[++i];
            int num = 0;
            num += HexChar2dec(c1) * 16 + HexChar2dec(c0);
            result += char(num);
        }
    }
    return result;
}

bool IsxDigit(char digit)
{
  return (bool)( (digit >= 0x30 && digit <= 0x39)    /* 0-9 */
              || (digit >= 0x41 && digit <= 0x46)    /* A-F */
              || (digit >= 0x61 && digit <= 0x66) ); /* a-f */
}

bool StrIsDigit(const string& str)
{
    if(str.length() == 0)
    {
        return false;
    }
    for(int i=0;i<str.length();i++)
    {
        if(!IsxDigit(str.at(i)))
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// �����
//int   main()
//  {
//  cout   <<   replace_all(string("12212"),"12","21")   <<   endl;
//  cout   <<   replace_all_distinct(string("12212"),"12","21")   <<   endl;
//  }

//output:
//22211
//21221

string ReplaceAll(const string& strSrc,const string& strOldValue,const string& strNewValue)
{
    string strRep =  strSrc;
    while(true)
    {
        string::size_type pos(0);
        if((pos=strRep.find(strOldValue))!=string::npos)
            strRep.replace(pos,strOldValue.length(),strNewValue);
        else   break;
    }
    return  strRep;
}
    
string ReplaceAllDistinct(const string& strSrc,const string& strOldValue,const string& strNewValue)
{
    string strRep =  strSrc;
    for(string::size_type pos(0);pos!=string::npos;pos+=strNewValue.length())
    {
        if((pos=strRep.find(strOldValue,pos))!=string::npos)
            strRep.replace(pos,strOldValue.length(),strNewValue);
        else   break;
    }
    return   strRep;
}

wstring ReplaceAllDistinct(const wstring& strSrc,const wstring& strOldValue,const wstring& strNewValue)
{
    wstring strRep =  strSrc;
    for(string::size_type pos(0);pos!=string::npos;pos+=strNewValue.length())
    {
        if((pos=strRep.find(strOldValue,pos))!=string::npos)
			strRep.replace(pos,strOldValue.length(),strNewValue);
        else   break;
    }
    return   strRep;
}

string WsToStr(const wstring& wstrValue)
{
    std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
    setlocale(LC_ALL, "chs");
    const wchar_t* _Source = wstrValue.c_str();
    size_t _Dsize = 2 * wstrValue.size() + 1;
    char *_Dest = new char[_Dsize];
    memset(_Dest,0,_Dsize);
    wcstombs(_Dest,_Source,_Dsize);
    std::string result = _Dest;
    delete []_Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

wstring StrToWs(const string& strValue)
{
    setlocale(LC_ALL, "chs"); 
    const char* _Source = strValue.c_str();
    size_t _Dsize = strValue.size() + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest,_Source,_Dsize);
    std::wstring result = _Dest;
    delete []_Dest;
    setlocale(LC_ALL, "C");
    return result;
}


char *WChToCh(const wchar_t *pWCh , char *pCh)
{
    *pCh++ = *pWCh >> 8 ;
    *pCh = *pWCh ;
    return 0 ;
} 
char *WStrToCStr(const wchar_t *pWStr , char *pCStr, size_t nLen)
{     
	char *pTemp = pCStr ;
    if(pWStr!=NULL && pCStr!=NULL)
   {  
		size_t wstrLen = wcslen(pWStr) ;
		nLen = (nLen > wstrLen ) ?    wstrLen : nLen ;
		while( nLen-- > 0)
		{
			WChToCh(pWStr , pCStr);
			pWStr++ ;
			pCStr +=2 ;      //�����ǿ�ʼ˵��һ��+2������+1
		}
		*pCStr = '\0';
		return pTemp ;
    }
    return 0 ;
}

//-----------------------------------------------------------------------------
// ����: ���Դ�Сд
//-----------------------------------------------------------------------------

char *StrIStr( char *pSrc, const char *pDst)
{
  unsigned long int len;
  unsigned long int i;

  len = strlen(pDst);
  i = 0;
  while (1){
	if (strlen(&pSrc[i]) < len)
	  break;
	if (memicmp(&pSrc[i], pDst, len) == 0)
	  return &pSrc[i];
	i ++;
  }
  return NULL;
}

char *stristr(const char *String, const char *Pattern)
{
      char *pptr, *sptr, *start;

      for (start = (char *)String; *start != NULL; start++)
      {
            /* find start of pattern in string */
            for ( ; ((*start!=NULL) && (toupper(*start) != toupper(*Pattern))); start++)
                  ;
            if (NULL == *start)
                  return NULL;

            pptr = (char *)Pattern;
            sptr = (char *)start;

            while (toupper(*sptr) == toupper(*pptr))
            {
                  sptr++;
                  pptr++;

                  /* if end of pattern then pattern was found */

                  if (NULL == *pptr)
                        return (start);
            }
      }
      return NULL;
}

bool EndSameText(const string& strValue, const string& strKey,bool bFlag)
{
	if(strValue.length() < strKey.length() || (strKey.length() <= 0 && strValue.length() > 0))	return false;
	if(bFlag)
	{
		#ifdef ICL_WIN32
			return stricmp((strValue.substr(strValue.length() - strKey.length(),strKey.length())).c_str(), strKey.c_str()) == 0;
		#endif
		#ifdef ICL_LINUX
			return strcasecmp((strValue.substr(strValue.length() - strKey.length(),strKey.length())), strKey.c_str()) == 0;
		#endif
	}
	else
	{
		#ifdef ICL_WIN32
			return strcmp((strValue.substr(strValue.length() - strKey.length(),strKey.length())).c_str(), strKey.c_str()) == 0;
		#endif
		#ifdef ICL_LINUX
			return strcmp((strValue.substr(strValue.length() - strKey.length(),strKey.length())), strKey.c_str()) == 0;
		#endif	
	}
}


bool StartSameText(const string& strValue, const string& strKey,bool bFlag)
{
	if(strValue.length() < strKey.length() || (strKey.length() <= 0 && strValue.length() > 0))	return false;
	if(bFlag)
	{
		#ifdef ICL_WIN32
			return stricmp((strValue.substr(0,strKey.length())).c_str(), strKey.c_str()) == 0;
		#endif
		#ifdef ICL_LINUX
			return strcasecmp((strValue.substr(0,strKey.length())), strKey.c_str()) == 0;
		#endif	
	}
	else
	{
		#ifdef ICL_WIN32
			return strcmp((strValue.substr(0,strKey.length())).c_str(), strKey.c_str()) == 0;
		#endif
		#ifdef ICL_LINUX
			return strcmp((strValue.substr(0,strKey.length())), strKey.c_str()) == 0;
		#endif		
	}
}
/*
bool StartWithStr(const string& strKey,const string& str,bool b)
{
    if(strKey.size() > str.size())  return false;
    if(b)
    {
		return strncmp(str.c_str(),strKey.c_str(),strKey.size()) == 0;
    }
    else
    {
        return strnicmp(str.c_str(),strKey.c_str(),strKey.size()) == 0;
    }
}

bool EndWithStr(const string& strKey,const string& str,bool b)
{
    if(strKey.length() > str.length())  return false;
    if(b)
    {
		int i = str.length()-1;
        int j = strKey.length()-1;
        while( j >=0)
        {
            if(strKey.at(j) != str.at(i))   return false;
            j--;
            i--;
        }
    }
    else
    {
        string strKeyTemp =  LowerCase(strKey);
        string strTemp =  LowerCase(str);
		int i = strTemp.length()-1;
		int j = strKeyTemp.length()-1;
        while( j >=0)
        {
            if(strKeyTemp.at(j) != strTemp.at(i))   return false;
            j--;
            i--;
        }
    }
    return true;
}
*/

//-----------------------------------------------------------------------------
// ����: �ж�һ���ַ����ǲ���һ������
//-----------------------------------------------------------------------------
bool IsInt(const string& str)
{
    bool bResult;
    int nLen = str.size();
    char *pStr = (char*)str.c_str();
    
    bResult = (nLen > 0) && !isspace(pStr[0]);

    if (bResult)
    {
        char *endp;
        strtol(pStr, &endp, 10);
        bResult = (endp - pStr == nLen);
    } 

    return bResult;
}

//-----------------------------------------------------------------------------
// ����: �ж�һ���ַ����ǲ���һ��������
//-----------------------------------------------------------------------------
bool IsFloat(const string& str)
{
    bool bResult;
    int nLen = str.size();
    char *pStr = (char*)str.c_str();
    char *endp;
    
    bResult = (nLen > 0) && !isspace(pStr[0]);

    if (bResult)
    {
        char *endp;
        strtod(pStr, &endp);
        bResult = (endp - pStr == nLen);
    } 

    return bResult;
}

//-----------------------------------------------------------------------------
// ����: �ַ���ת��������(��ת��ʧ�ܣ��򷵻� nDefault)
//-----------------------------------------------------------------------------
int StrToInt(const string& str, int nDefault)
{
    if (IsInt(str))
        return strtol(str.c_str(), NULL, 10);
    else
        return nDefault;
}

//-----------------------------------------------------------------------------
// ����: ����ת�����ַ���
//-----------------------------------------------------------------------------
string IntToStr(int nValue)
{
    char sTemp[64];
    sprintf(sTemp, "%d", nValue);
    return &sTemp[0];
}

//-----------------------------------------------------------------------------
// ����: �ַ���ת���ɸ�����(��ת��ʧ�ܣ��򷵻� fDefault)
//-----------------------------------------------------------------------------
double StrToFloat(const string& str, double fDefault)
{
    if (IsFloat(str))
        return strtod(str.c_str(), NULL);
    else
        return fDefault;
}

//-----------------------------------------------------------------------------
// ����: ������ת�����ַ���
//-----------------------------------------------------------------------------
string FloatToStr(double fValue, const char *sFormat)
{
    char sTemp[256];
    sprintf(sTemp, sFormat, fValue);
    return &sTemp[0];
}

//-----------------------------------------------------------------------------
// ����: ��ʽ���ַ��� (��FormatString��������)
//-----------------------------------------------------------------------------
void FormatStringV(string& strResult, const char *sFormatString, va_list argList)
{
    int nSize = 100;
    char *pBuffer = (char *)malloc(nSize);

    while (pBuffer)
    {
        int nChars;
        nChars = vsnprintf(pBuffer, nSize, sFormatString, argList);
        if (nChars > -1 && nChars < nSize)
            break;
        if (nChars > -1)
            nSize = nChars + 1;
        else
            nSize *= 2;
        pBuffer = (char *)realloc(pBuffer, nSize);
    }

    if (pBuffer)
    {
        strResult = pBuffer;
        free(pBuffer);
    }
    else
        strResult = "";
}

//-----------------------------------------------------------------------------
// ����: ��ʽ���ַ���
// ����:
//   strResult      - ��Ž����
//   sFormatString  - ��ʽ���ַ���
//   ...            - ��ʽ������
// ʾ��: 
//   FormatString(strResult, "ABC%sDEF%d", strTemp, nTemp);
//-----------------------------------------------------------------------------
void FormatString(string& strResult, const char *sFormatString, ...)
{
    va_list argList;
    va_start(argList, sFormatString);
    FormatStringV(strResult, sFormatString, argList);
    va_end(argList);
}

//-----------------------------------------------------------------------------
// ����: ���ظ�ʽ������ַ���
// ����:
//   sFormatString  - ��ʽ���ַ���
//   ...            - ��ʽ������
//-----------------------------------------------------------------------------
string FormatString(const char *sFormatString, ...)
{
    string strResult;

    va_list argList;
    va_start(argList, sFormatString);
    FormatStringV(strResult, sFormatString, argList);
    va_end(argList);

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �ж������ַ����Ƿ���ͬ (�����ִ�Сд)
//-----------------------------------------------------------------------------
bool SameText(const string& str1, const string& str2)
{
#ifdef ICL_WIN32
	return stricmp(str1.c_str(), str2.c_str()) == 0;
#endif
#ifdef ICL_LINUX
	return strcasecmp(str1.c_str(), str2.c_str()) == 0;
#endif
}

//-----------------------------------------------------------------------------
// ����: ȥ���ַ���ͷβ�Ŀհ��ַ� (ASCII <= 32)
//-----------------------------------------------------------------------------
string TrimString(const string& str)
{
    string strResult;
    int i, nLen;

    nLen = str.size();
    i = 0;
    while (i < nLen && (byte)str[i] <= 32) i++;
    if (i < nLen)
    {
        while ((byte)str[nLen-1] <= 32) nLen--;
        strResult = str.substr(i, nLen - i);
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �ַ������д
//-----------------------------------------------------------------------------
string UpperCase(const string& str)
{
    string strResult = str;
    int nLen = strResult.size();
    char c;

    for (int i = 0; i < nLen; i++)
    {
        c = strResult[i];
        if (c >= 'a' && c <= 'z')
            strResult[i] = c - 32;
    }
    
    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �ַ�����Сд
//-----------------------------------------------------------------------------
string LowerCase(const string& str)
{
    string strResult = str;
    int nLen = strResult.size();
    char c;

    for (int i = 0; i < nLen; i++)
    {
        c = strResult[i];
        if (c >= 'A' && c <= 'Z')
            strResult[i] = c + 32;
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �ַ����滻
// ����:
//   strSource        - Դ��
//   strOldPattern    - Դ���н����滻���ַ���
//   strNewPattern    - ȡ�� strOldPattern ���ַ���
//   bReplaceAll      - �Ƿ��滻Դ��������ƥ����ַ���(��Ϊfalse����ֻ�滻��һ��)
//   bCaseSensitive   - �Ƿ����ִ�Сд
// ����:
//   �����滻����֮����ַ���
//-----------------------------------------------------------------------------
string RepalceString(const string& strSource, const string& strOldPattern, 
    const string& strNewPattern, bool bReplaceAll, bool bCaseSensitive)
{
    string strResult = strSource;
    string strSearch, strPattern;
    int nOffset, nIndex, nOldPattLen, nNewPattLen;

    if (!bCaseSensitive)
    {
        strSearch = UpperCase(strSource);
        strPattern = UpperCase(strOldPattern);
    }
    else
    {
        strSearch = strSource;
        strPattern = strOldPattern;
    }

    nOldPattLen = strOldPattern.size();
    nNewPattLen = strNewPattern.size();
    nIndex = 0;

    while (nIndex < (int)strSearch.size())
    {
        nOffset = strSearch.find(strPattern, nIndex);
        if (nOffset == string::npos) break;  // ��û�ҵ�
     
        strSearch.replace(nOffset, nOldPattLen, strNewPattern);
        strResult.replace(nOffset, nOldPattLen, strNewPattern);
        nIndex = (nOffset + nNewPattLen);

        if (!bReplaceAll) break;
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �ָ��ַ���
// ����:
//   strSource  - Դ��
//   chSplitter - �ָ���
//   StrList    - ��ŷָ�֮����ַ����б�
// ʾ��:
//   ""          -> []
//   " "         -> [" "]
//   ","         -> ["", ""]
//   "a,b,c"     -> ["a", "b", "c"]
//   ",a,,b,c,"  -> ["", "a", "", "b", "c", ""]
//-----------------------------------------------------------------------------
void SplitString(const string& strSource, char chSplitter, StringArray& StrList)
{
    int nIndex = 0;
    int nOffset;

    StrList.clear();
    if (strSource.empty()) return;

    while (true)
    {
        nOffset = strSource.find(chSplitter, nIndex);
        if (nOffset == string::npos)   // ��û�ҵ�
        {
            StrList.push_back(strSource.substr(nIndex));
            break;
        }
        else
        {
            StrList.push_back(strSource.substr(nIndex, nOffset - nIndex));
            nIndex = nOffset + 1;
        }
    }
}

void SplitString(const string& strSource, const string& strSplitter, StringArray& StrList)
{
    int nIndex = 0;
    int nOffset;

    StrList.clear();
    if (strSource.empty()) return;

    while (true)
    {
        nOffset = strSource.find(strSplitter, nIndex);
        if (nOffset == string::npos)   // ��û�ҵ�
        {
            StrList.push_back(strSource.substr(nIndex));
            break;
        }
        else
        {
            StrList.push_back(strSource.substr(nIndex, nOffset - nIndex));
            nIndex = nOffset + strSplitter.length();
        }
    }
}

//-----------------------------------------------------------------------------
// ����: �ָ��ַ�����ת�����������б�
// ����:
//   strSource  - Դ��
//   chSplitter - �ָ���
//   IntList    - ��ŷָ�֮����������б�
//-----------------------------------------------------------------------------
void SplitStringToInt(const string& strSource, char chSplitter, IntegerArray& IntList)
{
    StringArray StrList;

    SplitString(strSource, chSplitter, StrList);
    IntList.clear();
    for (int i = 0; i < (int)StrList.size(); i++)
        IntList.push_back(atoi(StrList[i].c_str()));
}

//-----------------------------------------------------------------------------
// ����: ���ƴ� pSource �� pDest ��
// ��ע: 
//   1. ���ֻ���� nMaxBytes ���ֽڵ� pDest �У�����������'\0'��
//   2. ��� pSource ��ʵ�ʳ���(strlen)С�� nMaxBytes�����ƻ���ǰ������
//      pDest ��ʣ�ಿ���� '\0' ��䡣
//   3. ��� pSource ��ʵ�ʳ���(strlen)���� nMaxBytes������֮��� pDest û�н�������
//-----------------------------------------------------------------------------
char *StrNCopy(char *pDest, const char *pSource, int nMaxBytes)
{
    if (nMaxBytes > 0)
    {
        if (pSource)
            return strncpy(pDest, pSource, nMaxBytes);
        else
            return strcpy(pDest, "");
    }

    return pDest;
}

//-----------------------------------------------------------------------------
// ����: ���ƴ� pSource �� pDest ��
// ��ע: ���ֻ���� nDestSize ���ֽڵ� pDest �С����� pDest ������ֽ���Ϊ'\0'��
// ����: 
//   nDestSize - pDest�Ĵ�С
//-----------------------------------------------------------------------------
char *StrNZCopy(char *pDest, const char *pSource, int nDestSize)
{
    if (nDestSize > 0)
    {
        if (pSource)
        {
            char *p;
            p = strncpy(pDest, pSource, nDestSize);
            pDest[nDestSize - 1] = '\0';
            return p;
        }
        else
            return strcpy(pDest, "");
    }
    else
        return pDest;
}

//-----------------------------------------------------------------------------
// ����: ����ļ��Ƿ����
//-----------------------------------------------------------------------------
bool FileExists(const string& strFileName)
{
#ifdef ICL_WIN32
    HANDLE nHandle;
    WIN32_FIND_DATA FindData;
    bool bResult;

    nHandle = FindFirstFile(strFileName.c_str(), &FindData);
    bResult = (nHandle != INVALID_HANDLE_VALUE);
    if (bResult) FindClose(nHandle);
    return bResult;
#endif
#ifdef ICL_LINUX
    return (euidaccess(strFileName.c_str(), F_OK) == 0);
#endif
}

//-----------------------------------------------------------------------------
// ����: ���Ŀ¼�Ƿ����
//-----------------------------------------------------------------------------
bool DirectoryExists(const string& strDir)
{
#ifdef ICL_WIN32
    int nCode;
    nCode = GetFileAttributes(strDir.c_str());
    return (nCode != -1) && ((FILE_ATTRIBUTE_DIRECTORY & nCode) != 0);
#endif
#ifdef ICL_LINUX
    struct stat st;
    bool bResult;

    if (stat(strDir.c_str(), &st) == 0)
        bResult = ((st.st_mode & S_IFDIR) == S_IFDIR);
    else
        bResult = false;

    return bResult;
#endif
}

//-----------------------------------------------------------------------------
// ����: ����Ŀ¼
// ʾ��:
//   CreateDir("C:\\test");
//   CreateDir("/home/test");
//-----------------------------------------------------------------------------
bool CreateDir(const string& strDir)
{
#ifdef ICL_WIN32
    return CreateDirectory(strDir.c_str(), NULL);
#endif
#ifdef ICL_LINUX
    return mkdir(strDir.c_str(), (mode_t)(-1)) == 0;
#endif
}

//-----------------------------------------------------------------------------
// ����: ȡ���ļ��������һ���ָ�����λ��(0-based)����û�У��򷵻�-1
//-----------------------------------------------------------------------------
int GetLastDelimPos(const string& strFileName)
{
    int nResult = strFileName.size() - 1;
    string strDelims;

    strDelims += PATH_DELIM;
#ifdef ICL_WIN32
    strDelims += DRIVER_DELIM;
#endif

    for (; nResult >= 0; nResult--)
        if (strDelims.find(strFileName[nResult], 0) != string::npos)
            break;

    return nResult;
}

//-----------------------------------------------------------------------------
// ����: ���ļ����ַ�����ȡ���ļ�·��
// ����:
//   strFileName - ����·�����ļ���
// ����:
//   �ļ���·��
// ʾ��:
//   ExtractFilePath("C:\\MyDir\\test.c");         ����: "C:\\MyDir\\"
//   ExtractFilePath("C:");                        ����: "C:\\"
//   ExtractFilePath("/home/user1/data/test.c");   ����: "/home/user1/data/"
//-----------------------------------------------------------------------------
string ExtractFilePath(const string& strFileName)
{
    int nPos = GetLastDelimPos(strFileName);
    return PathWithSlash(strFileName.substr(0, nPos + 1));
}

//-----------------------------------------------------------------------------
// ����: ���ļ����ַ�����ȡ���������ļ���
// ����:
//   strFileName - ����·�����ļ���
// ����:
//   �ļ���
// ʾ��:
//   ExtractFileName("C:\\MyDir\\test.c");         ����: "test.c"
//   ExtractFilePath("/home/user1/data/test.c");   ����: "test.c"
//-----------------------------------------------------------------------------
string ExtractFileName(const string& strFileName)
{
    int nPos = GetLastDelimPos(strFileName);
    return strFileName.substr(nPos + 1, strFileName.size() - nPos - 1);
}

//-----------------------------------------------------------------------------
// ����: ǿ�ƴ���Ŀ¼
// ����: 
//   strDir - ��������Ŀ¼ (�����Ƕ༶Ŀ¼)
// ����:
//   true   - �ɹ�
//   false  - ʧ��
// ʾ��:
//   ForceDirectories("C:\\MyDir\\Test");
//   ForceDirectories("/home/user1/data");
//-----------------------------------------------------------------------------
bool ForceDirectories(string strDir)
{
    int nLen = strDir.length();

    if (strDir.empty()) return false;
    if (strDir[nLen-1] == PATH_DELIM)
        strDir.resize(nLen - 1);

#ifdef ICL_WIN32
    if (strDir.length() < 3 || DirectoryExists(strDir) ||
        ExtractFilePath(strDir) == strDir) return true;    // avoid 'xyz:\' problem.
#endif
#ifdef ICL_LINUX
    if (strDir.empty() || DirectoryExists(strDir)) return true;
#endif
    return ForceDirectories(ExtractFilePath(strDir)) && CreateDir(strDir);
}

//-----------------------------------------------------------------------------
// ����: ɾ���ļ�
//-----------------------------------------------------------------------------
bool DeleteFile(const string& strFileName)
{
#ifdef ICL_WIN32
    return ::DeleteFile(strFileName.c_str());
#endif
#ifdef ICL_LINUX
    return (unlink(strFileName.c_str()) != -1);
#endif
}

//-----------------------------------------------------------------------------
// ����: ȡ���ļ��Ĵ�С����ʧ���򷵻�-1
//-----------------------------------------------------------------------------
int64 GetFileSize(const string& strFileName)
{
    int64 nResult;

    try
    {
        CFileStream FileStream(strFileName, FM_OPEN_READ | FM_SHARE_DENY_NONE);
        nResult = FileStream.GetSize();
    }
    catch (CException& e)
    {
        nResult = -1;
    }

    return nResult;
}

//-----------------------------------------------------------------------------
// ����: ��ָ��·���²��ҷ����������ļ�
// ����:
//   strPath    - ָ�����ĸ�·���½��в��ң�������ָ��ͨ���
//   nAttr      - ֻ���ҷ��ϴ����Ե��ļ�
//   FindResult - ���ز��ҽ��
// ʾ��:
//   FindFiles("C:\\test\\*.*", FA_ANY_FILE & ~FA_HIDDEN, fr);
//   FindFiles("/home/*.log", FA_ANY_FILE & ~FA_SYM_LINK, fr); 
//-----------------------------------------------------------------------------
void FindFiles(const string& strPath, uint nAttr, FileFindResult& FindResult)
{
    const uint FA_SPECIAL = FA_HIDDEN | FA_SYS_FILE | FA_VOLUME_ID | FA_DIRECTORY;
    uint nExcludeAttr = ~nAttr & FA_SPECIAL;
    FindResult.clear();
    
#ifdef ICL_WIN32
    HANDLE nFindHandle;
    WIN32_FIND_DATA FindData;

    nFindHandle = FindFirstFile(strPath.c_str(), &FindData);
    if (nFindHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((FindData.dwFileAttributes & nExcludeAttr) == 0)
            {
                CFileFindRec Item;
                Item.nFileSize = FindData.nFileSizeHigh;
                Item.nFileSize = (Item.nFileSize << 32) | FindData.nFileSizeLow;
				//char pcstr[550];
				//memset(pcstr,0,550);
				//wstr2cstr(FindData.cFileName , pcstr , wcslen(FindData.cFileName)) ;
                Item.strFileName = FindData.cFileName;
                Item.nAttr = FindData.dwFileAttributes;

                FindResult.push_back(Item);
            }
        }
        while (FindNextFile(nFindHandle, &FindData));

        FindClose(nFindHandle);
    }
#endif

#ifdef ICL_LINUX
    string strPathOnly = ExtractFilePath(strPath);
    string strPattern = ExtractFileName(strPath);
    string strFileName;
    DIR *pDir;
    struct dirent DirEnt, *pDirEnt = NULL;
    struct stat StatBuf, LinkStatBuf;
    uint nFileAttr, nFileMode;

    if (strPathOnly.empty()) strPathOnly = "/";

    pDir = opendir(strPathOnly.c_str());
    if (pDir)
    {
        while ((readdir_r(pDir, &DirEnt, &pDirEnt) == 0) && pDirEnt)
        {
            if (!fnmatch(strPattern.c_str(), pDirEnt->d_name, 0) == 0) continue;

            strFileName = strPathOnly + pDirEnt->d_name;

            if (lstat(strFileName.c_str(), &StatBuf) == 0)
            {
                nFileAttr = 0;
                nFileMode = StatBuf.st_mode;

                if (S_ISDIR(nFileMode))
                    nFileAttr |= FA_DIRECTORY;
                else if (!S_ISREG(nFileMode))
                {
                    if (S_ISLNK(nFileMode))
                    {
                        nFileAttr |= FA_SYM_LINK;
                        if ((stat(strFileName.c_str(), &LinkStatBuf) == 0) &&
                            (S_ISDIR(LinkStatBuf.st_mode)))
                            nFileAttr |= FA_DIRECTORY;
                    }
                    nFileAttr |= FA_SYS_FILE;
                }

                if (pDirEnt->d_name[0] == '.' && pDirEnt->d_name[1])
                    if (!(pDirEnt->d_name[1] == '.' && !pDirEnt->d_name[2]))
                        nFileAttr |= FA_HIDDEN;

                if (euidaccess(strFileName.c_str(), W_OK) != 0)
                    nFileAttr |= FA_READ_ONLY;

                if ((nFileAttr & nExcludeAttr) == 0)
                {
                    CFileFindRec Item;
                    Item.nFileSize = StatBuf.st_size;
                    Item.strFileName = pDirEnt->d_name;
                    Item.nAttr = nFileAttr;

                    FindResult.push_back(Item);
                }
            }
        } // while

        closedir(pDir);
    }
#endif
}

//-----------------------------------------------------------------------------
// ����: ��ȫ·���ַ�������� "\" �� "/"
//-----------------------------------------------------------------------------
string PathWithSlash(const string& strPath)
{
    string strResult = TrimString(strPath);
    int nLen = strResult.size();
    if (nLen > 0 && strResult[nLen-1] != PATH_DELIM)
        strResult += PATH_DELIM;
    return strResult;
}

//-----------------------------------------------------------------------------
// ����: ȥ��·���ַ�������� "\" �� "/"
//-----------------------------------------------------------------------------
string PathWithoutSlash(const string& strPath)
{
    string strResult = TrimString(strPath);
    int nLen = strResult.size();
    if (nLen > 0 && strResult[nLen-1] == PATH_DELIM)
        strResult.resize(nLen - 1);
    return strResult;
}

//-----------------------------------------------------------------------------
// ����: ���ز���ϵͳ��������Ӧ�Ĵ�����Ϣ
//-----------------------------------------------------------------------------
string SysErrorMessage(int nErrorCode)
{
#ifdef ICL_WIN32
    char *pErrorMsg;
    
    pErrorMsg = strerror(nErrorCode);
    return pErrorMsg;
#endif
#ifdef ICL_LINUX
    const int ERROR_MSG_SIZE = 256;
    char sErrorMsg[ERROR_MSG_SIZE];
    string strResult;

    sErrorMsg[0] = 0;
    strerror_r(nErrorCode, sErrorMsg, ERROR_MSG_SIZE);
    if (sErrorMsg[0] == 0)
        FormatString(strResult, "System error: %d", nErrorCode);
    else
        strResult = sErrorMsg;

    return strResult;
#endif
}

//-----------------------------------------------------------------------------
// ����: ˯�� fSeconds �룬�ɾ�ȷ�����롣
// ����:
//   fSeconds       - ˯�ߵ���������ΪС�����ɾ�ȷ������ (ʵ�ʾ�ȷ��ȡ���ڲ���ϵͳ)
//   AllowInterrupt - �Ƿ������ź��ж�
//-----------------------------------------------------------------------------
void NanoSleep(double fSeconds, bool AllowInterrupt)
{
#ifdef ICL_WIN32
    Sleep((uint)(fSeconds * 1000));
#endif
#ifdef ICL_LINUX
    const uint NANO_PER_SEC = 1000000000;  // һ����ڶ�������
    struct timespec req, remain;
    int r;

    req.tv_sec = (uint)fSeconds;
    req.tv_nsec = (uint)((fSeconds - req.tv_sec) * NANO_PER_SEC);

    while (true)
    {
        r = nanosleep(&req, &remain);
        if (r == -1 && errno == EINTR && !AllowInterrupt)
            req = remain;
        else
            break;
    }
#endif
}

void USjwSleep(double fSeconds)
{
#ifdef ICL_WIN32
     WaitForSingleObject(NULL,fSeconds);
#endif
#ifdef ICL_LINUX
    usleep(fSeconds);
#endif	
}

//-----------------------------------------------------------------------------
// ����: ȡ�õ�ǰ Ticks����λ:����
//-----------------------------------------------------------------------------
uint GetCurTicks()
{
#ifdef ICL_WIN32
    return GetTickCount();
#endif
#ifdef ICL_LINUX
    timeval tv;
    gettimeofday(&tv, NULL);
    return int64(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
#endif
}

//-----------------------------------------------------------------------------
// ����: ȡ������ Ticks ֮��
//-----------------------------------------------------------------------------
uint GetTickDiff(uint nOldTicks, uint nNewTicks)
{
    if (nNewTicks >= nOldTicks)
        return (nNewTicks - nOldTicks);
    else
        return (uint(-1) - nOldTicks + nNewTicks);
}

///-----------------------------------------------------------------------------
// ����: ����ѹ�� (ZLIB)
//-----------------------------------------------------------------------------
bool ZlibCompress(CBuffer& Buffer)
{
    bool bResult = false;
    uint nSrcSize = Buffer.GetSize();

    if (nSrcSize > 0)
    {
        try
        {
            CBuffer DestBuffer(nSrcSize * 120 / 100 + 12);
            uint DestLen = DestBuffer.GetSize();

            bResult = (compress(
                (Bytef*)DestBuffer.Data(),  // dest buffer
                (uLongf*)&DestLen,          // dest size
                (Bytef*)Buffer.Data(),      // src buffer
                (uLong)nSrcSize)            // src size
                == Z_OK);

            if (bResult)
                Buffer.Assign(DestBuffer.Data(), DestLen);
        }
        catch (CException& e)
        {
            bResult = false;
        }
    }

    return bResult;
}

//-----------------------------------------------------------------------------
// ����: ���ݽ�ѹ (ZLIB)
//-----------------------------------------------------------------------------
bool ZlibUncompress(CBuffer& Buffer)
{
    bool bResult = false;
    uint nSrcSize = Buffer.GetSize();

    if (nSrcSize > 0)
    {
        try
        {
            CBuffer DestBuffer(nSrcSize * 5);
            uint DestLen = DestBuffer.GetSize();
            int r;

            while (true)
            {
                r = uncompress(
                    (Bytef*)DestBuffer.Data(),  // dest buffer
                    (uLongf*)&DestLen,          // dest size
                    (Bytef*)Buffer.Data(),      // src buffer
                    (uLong)nSrcSize);           // src size

                // �����ѹ�ɹ�
                if (r == Z_OK)
                {
                    bResult = true;
                    break;
                }
                // �������������ռ䲻��
                else if (r == Z_BUF_ERROR)
                {
                    bResult = false;
                    DestBuffer.SetSize(DestBuffer.GetSize() * 2);
                    DestLen = DestBuffer.GetSize();
                }
                else
                {
                    bResult = false;
                    break;
                }
            }

            if (bResult)
                Buffer.Assign(DestBuffer.Data(), DestLen);
        }
        catch (CException& e)
        {
            bResult = false;
        }
    }

    return bResult;
}


int HTTPGZibUncompress(char* pBuffer,long nSrcLen,long* pDestLen)
{
    int r = -1;
    unsigned long DestLen = (*pDestLen);
    char* pSjwBuffer = new char[*pDestLen];
    if(pSjwBuffer)
    {
        r = SjwHttpGzDeCompress((Byte*)pBuffer, nSrcLen,(Byte*)pSjwBuffer, &DestLen);
        memset(pBuffer,0,*pDestLen);
        memcpy(pBuffer,pSjwBuffer,DestLen);
        *pDestLen = DestLen;
        delete[] pSjwBuffer;
    }
    return r;
}

//-----------------------------------------------------------------------------
// ����: ���ݽ�ѹ (GZIB)
//-----------------------------------------------------------------------------
bool HTTPGZibUncompress(CBuffer& Buffer)
{
    bool bResult = false;
    unsigned long nSrcSize = Buffer.GetSize();

    if (nSrcSize > 0)
    {
        try
        {
            CBuffer DestBuffer(nSrcSize * 3);
            unsigned long DestLen = DestBuffer.GetSize();
            int r;

            while (true)
            {
                r = HttpGzDeCompress((Byte*)(Buffer.Data()), nSrcSize,(Byte*)(DestBuffer.Data()), &DestLen);
                // �����ѹ�ɹ�
                if (r == Z_OK)
                {
                    bResult = true;
                    break;
                }
                // �������������ռ䲻��
                else if (r == Z_BUF_ERROR)
                {
                    bResult = false;
                    DestBuffer.SetSize(DestBuffer.GetSize() * 2);
                    DestLen = DestBuffer.GetSize();
                }
                else
                {
                    bResult = false;
                    break;
                }
            }

            if (bResult)
                Buffer.Assign(DestBuffer.Data(), DestLen);
        }
        catch (CException& e)
        {
            bResult = false;
        }
    }

    return bResult;
}


bool HttpDeflateUnCompress(CBuffer& Buffer)
{
	bool bResult = false;
    unsigned long nSrcSize = Buffer.GetSize();

    if (nSrcSize > 0)
    {
        try
        {
            CBuffer DestBuffer(nSrcSize * 3);
            unsigned long DestLen = DestBuffer.GetSize();
            int r;

            while (true)
            {
                r = DeflateUnCompress((Byte*)(Buffer.Data()), nSrcSize,(Byte*)(DestBuffer.Data()), &DestLen);
                // �����ѹ�ɹ�
                if (r == Z_OK)
                {
                    bResult = true;
                    break;
                }
                // �������������ռ䲻��
                else if (r == Z_BUF_ERROR)
                {
                    bResult = false;
                    DestBuffer.SetSize(DestBuffer.GetSize() * 2);
                    DestLen = DestBuffer.GetSize();
                }
                else
                {
                    bResult = false;
                    break;
                }
            }

            if (bResult)
                Buffer.Assign(DestBuffer.Data(), DestLen);
        }
        catch (CException& e)
        {
            bResult = false;
        }
    }

    return bResult;	
}



int DeflateUnCompress(Byte *ddata, uLong nddata,Byte *data, uLong *ndata)
{
	z_stream d_stream;
	bool bStreamInitialized=false;
	bool bStreamEnded=false;
    int err;

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;

	if (inflateInit(&d_stream) != Z_OK)
		return -1;
	bStreamInitialized = true;

	d_stream.next_in = ddata;
	d_stream.avail_in = (uInt)nddata;

	for (;;)
	{
		d_stream.next_out = data;
		d_stream.avail_out = (uInt)(*ndata);
 
		int code = inflate(&d_stream, Z_NO_FLUSH);
		if (code == Z_STREAM_END)
		{                       
			inflateEnd(&d_stream);
			bStreamEnded = true;
			err = 1;
			break;
		}
		else if (code == Z_OK)
		{
			err = 0; 
			break;
		}
		else if (code == Z_BUF_ERROR)
		{
			err = Z_BUF_ERROR;
			break;
		}
		else if (code == Z_DATA_ERROR)
		{
			// some servers (notably Apache with mod_deflate) don't generate zlib headers
			// insert a dummy header and try again
			static char dummy_head[2] =
			{
                            0x8 + 0x7 * 0x10,
                            (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
			};
			inflateReset(&d_stream);
			d_stream.next_in = (Bytef*) dummy_head;
			d_stream.avail_in = sizeof(dummy_head);

			code = inflate(&d_stream, Z_NO_FLUSH);
			if (code != Z_OK)
			{
				err = -1;
				break;
			}

			// reset stream pointers to our original data
			d_stream.next_in = ddata;
			d_stream.avail_in = (uInt)nddata;
		}    
		else
		{
			err = -1;
			break;
		}
	} /* for */
	if ((bStreamInitialized == true) && (bStreamEnded == false))
		err = inflateEnd (&d_stream);
	return err;
}



/* Compress data */
int ZCompress(Bytef *data, uLong ndata, 
	Bytef *zdata, uLong *nzdata)
{
	z_stream c_stream;
	int err = 0;

	if(data && ndata > 0)
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		if(deflateInit(&c_stream, Z_DEFAULT_COMPRESSION) != Z_OK) return -1;
		c_stream.next_in  = data;
		c_stream.avail_in  = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out  = *nzdata;
		while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata) 
		{
			if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
        if(c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if(err != Z_OK) return -1;
		}
		if(deflateEnd(&c_stream) != Z_OK) return -1;
		*nzdata = c_stream.total_out;
		return 0;
	}
	return -1;
}

/* Compress gzip data */
int GZCompress(Bytef *data, uLong ndata,
	Bytef *zdata, uLong *nzdata)
{
	z_stream c_stream;
	int err = 0;

	if(data && ndata > 0)
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 
                    -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
		c_stream.next_in  = data;
		c_stream.avail_in  = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out  = *nzdata;
		while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata) 
		{
			if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
        if(c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if(err != Z_OK) return -1;
		}
		if(deflateEnd(&c_stream) != Z_OK) return -1;
		*nzdata = c_stream.total_out;
		return 0;
	}
	return -1;
}

/* Uncompress data */
int ZDeCompress(Byte *zdata, uLong nzdata,
        Byte *data, uLong *ndata)
{
	int err = 0;
	z_stream d_stream; /* decompression stream */

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;
    d_stream.next_in  = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	if(inflateInit(&d_stream) != Z_OK) return -1;
	while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if(err != Z_OK) return -1;
	}
	if(inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;
}

/* HTTP gzip decompress */
int HttpGzDeCompress(Byte *zdata, uLong nzdata,
        Byte *data, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = 
    {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in  = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    if((err = inflateInit2(&d_stream, 47)) != Z_OK) 
	{
		return err;
	}
    while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if(err != Z_OK )
        {
            if(err == Z_DATA_ERROR)
            {
                d_stream.next_in = (Bytef*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) 
                {
                    return err;
                }
            }
            else return err;
        }
    }
    if(d_stream.total_in < nzdata -1)
    {
        inflateEnd(&d_stream);
        return Z_BUF_ERROR;
    }
    if((err = inflateEnd(&d_stream)) != Z_OK ) return err;
    *ndata = d_stream.total_out;
    return err;
}

/* HTTP gzip decompress */
int SjwHttpGzDeCompress(Byte *zdata, uLong nzdata,
        Byte *data, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = 
    {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in  = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
    while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if(err != Z_OK )
        {
            if(err == Z_DATA_ERROR)
            {
                d_stream.next_in = (Bytef*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) 
                {
                    return -1;
                }
            }
            else return -1;
        }
    }
    if(inflateEnd(&d_stream) != Z_OK) return -1;
    *ndata = d_stream.total_out;
    return 0;
}

/* Uncompress gzip data */
int GzDeCompress(Byte *zdata, uLong nzdata,
        Byte *data, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = 
    {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in  = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    if(inflateInit2(&d_stream, -MAX_WBITS) != Z_OK) return -1;
    //if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
    while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if(err != Z_OK )
        {
            if(err == Z_DATA_ERROR)
            {
                d_stream.next_in = (Bytef*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) 
                {
                    return -1;
                }
            }
            else return -1;
        }
    }
    if(inflateEnd(&d_stream) != Z_OK) return -1;
    *ndata = d_stream.total_out;
    return 0;
}



//-----------------------------------------------------------------------------
// ����: ���� CRC32 ֵ
// ע��:
//   ���Ҫ���ô˺���ѭ������һ�����ݣ��Ƚ����� nCrc ��Ϊ 0xFFFFFFFF���´ε���
//   ʱ�����ϴεļ�����ȡ���ٴ��롣
//-----------------------------------------------------------------------------
uint Crc32(uint nCrc, void *pBuffer, int nSize)
{
    return crc32(nCrc, (unsigned char*)pBuffer, nSize);
}

//-----------------------------------------------------------------------------
// ����: ����һ�黺��� CRC32 ֵ
// ��ע:
//   �� pBuffer == NULL������ 0
//   �� nSize == 0������ -1
//-----------------------------------------------------------------------------
uint CalcBufferCrc32(void *pBuffer, int nSize)
{
    return Crc32(0xFFFFFFFF, pBuffer, nSize);
}


//-----------------------------------------------------------------------------
// ����: �����ݽ��� Base64 ����
// ����:
//   pData  - �����б��������
//   nSize  - pData�е��ֽ���
// ����:
//   �������ַ���(����'\0'�ַ�)
//-----------------------------------------------------------------------------
string StrToBase64(const char *pData, int nSize)
{
    static char *pTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    uint b;
    int i;
    char *d, *t;
    string strResult;

    if (!pData || nSize <= 0) return "";
    strResult.resize(nSize * 4 / 3 + 4);
    d = (char*)strResult.c_str();
    t = pTable;

    while (nSize > 0)
    {
        b = 0;
        for (i = 0; i <= 2; i++)
        {
            b = b << 8;
            if (nSize > 0)
            {
                b = b | (unsigned char)(*pData);
                pData++;
            }
            nSize--;
        }
        for (i = 3; i >= 0; i--)
        {
            if (nSize < 0)
            {
                d[i] = t[64];
                nSize++;
            }
            else
            {
                d[i] = t[b & 0x3F];
            }
            b = b >> 6;
        }
        d += 4;
    }

    strResult.resize(d - strResult.c_str());
    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �����ݽ��� Base16 ����
// ����:
//   pData  - �����б��������
//   nSize  - pData�е��ֽ���
// ����:
//   �������ַ���(����'\0'�ַ�)
//-----------------------------------------------------------------------------
string StrToBase16(const char *pData, int nSize)
{
    static char *pTable = "0123456789ABCDEF";
    char *d, *s;
    string strResult;

    if (!pData || nSize <= 0) return "";
    strResult.resize(nSize * 2);
    d = (char*)strResult.c_str();
    s = (char*)pData;

    while (nSize > 0)
    {
        *d++ = pTable[((*s) >> 4) & 0x0F];
        *d++ = pTable[(*s) & 0x0F];
        s++;
        nSize--;
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: ����� "���������"
//-----------------------------------------------------------------------------
void Randomize()
{
    srand((unsigned int)time(NULL));
}

//-----------------------------------------------------------------------------
// ����: ���� [nMin..nMax] ֮���һ��������������߽�
//-----------------------------------------------------------------------------
int GetRandom(int nMin, int nMax)
{
    return nMin + (int)((double)(nMax - nMin + 1) * rand() / (RAND_MAX + 1.0));
}

//-----------------------------------------------------------------------------
// ����: �ƶ��ڴ��
//-----------------------------------------------------------------------------
void MoveBuffer(const void *pSource, void *pDest, int nCount)
{
    memmove(pDest, pSource, nCount);
}

//-----------------------------------------------------------------------------
// ����: �ڴ����������
//-----------------------------------------------------------------------------
void ZeroBuffer(void *pBuffer, int nCount)
{
#ifdef ICL_WIN32
    ZeroMemory(pBuffer, nCount);
#endif
#ifdef ICL_LINUX
    bzero(pBuffer, nCount);
#endif
}

//-----------------------------------------------------------------------------
// ����: ����ڴ������
//-----------------------------------------------------------------------------
void FillBuffer(void *pBuffer, int nCount, byte nValue)
{
    memset(pBuffer, nValue, nCount);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace icl
