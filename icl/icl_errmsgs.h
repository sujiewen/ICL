///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
//
// 文件名称: icl_errmsgs.h
// 功能描述: ICL错误信息定义
// 最后修改: 2006-01-09
///////////////////////////////////////////////////////////////////////////////

#ifndef _ICL_ERRMSGS_H_
#define _ICL_ERRMSGS_H_

///////////////////////////////////////////////////////////////////////////////
// 错误信息 (Iris Error Message)

const char* const IEM_OUT_OF_MEMORY               = "out of memory";
const char* const IEM_INVALID_DATETIME_STR        = "invalid datetime string";
const char* const IEM_CANNOT_CREATE_FILE          = "cannot create file '%s'. %s.%d.%d";
const char* const IEM_CANNOT_OPEN_FILE            = "cannot open file '%s'. %s.%d.%d";
const char* const IEM_SET_FILE_STREAM_SIZE_ERR    = "error setting file stream size";
const char* const IEM_LIST_INDEX_ERROR            = "list index error";
const char* const IEM_LIST_CAPACITY_ERROR         = "list capacity error";
const char* const IEM_LIST_COUNT_ERROR            = "list count error";
const char* const IEM_THREAD_ALREADY_RUNNING      = "thread already running";

///////////////////////////////////////////////////////////////////////////////

#endif // _ICL_ERRMSGS_H_ 
