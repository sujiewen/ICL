///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
//
// 文件名称: icl_assert.cpp
// 功能描述: 断言支持
// 最后修改: 2005-12-19
///////////////////////////////////////////////////////////////////////////////

#include "icl_assert.h"
#include "icl_classes.h"
#include "icl_sysutils.h"

namespace icl
{

///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// 描述: 内部使用的断言函数。输出日志，并抛出异常。
// 注意: 缺省情况下，全局对象 Logger 并未初始化日志文件名，应用程序须自行初始化。
//-----------------------------------------------------------------------------
void InternalAssert(char *pCondition, char *pFileName, int nLineNumber)
{
    CException e(FormatString("Assertion failed: %s", pCondition).c_str(), pFileName, nLineNumber);
    Logger.Write(e);
    throw e;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace icl
