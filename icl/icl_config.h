///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
//
// �ļ�����: icl_config.h
// ��������: ȫ������
// ����޸�: 2005-12-25
///////////////////////////////////////////////////////////////////////////////

#ifndef _ICL_CONFIG_H_
#define _ICL_CONFIG_H_

///////////////////////////////////////////////////////////////////////////////
// ƽ̨����

#if defined(__BORLANDC__)
#define ICL_WIN32
#endif

#if defined(_MSC_VER)
#define ICL_WIN32
#endif

#if defined(__GNUC__)
#define ICL_LINUX
#endif

///////////////////////////////////////////////////////////////////////////////
// ���Կ���

#define ICL_DEBUG

#include "icl_assert.h"

///////////////////////////////////////////////////////////////////////////////
// ����

// ��ֹ winsock2.h �� winsock.h ͬʱ���������ͻ
#ifdef ICL_WIN32
#define _WINSOCKAPI_
#endif

// �Ƿ�ʹ�� "�Ǳ�׼STL"
#define USING_EXT_STL

///////////////////////////////////////////////////////////////////////////////

#endif // _ICL_CONFIG_H_ 