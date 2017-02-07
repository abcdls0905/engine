//--------------------------------------------------------------------
// 文件名:		fm_math.h
// 内  容:		
// 说  明:		
// 创建日期:	2014年3月14日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
// 备注内容:

/*
一个平台的快速日志库
支持多线程
*/
//--------------------------------------------------------------------


#ifndef _FM_LOG_H_
#define _FM_LOG_H_


/*
保留一个版本，该函数功能已经移植到i_core.h
中作为引擎核心日志功能的一个扩展性功能

如果还有什么地方包含该地方，请移出此头文件包含
后面会有一个版本考虑清理该头文件
*/

/*
#include "fm_mem_log.h"

  
//写入一行日志
inline bool fm_tracelog(const char * info,LOG_TYPE log_type = LOG_LOGIC )
{
	return CMemLog::instrance(log_type)->Write(info);
}


//读取一行日志
inline char* fm_logread(LOG_TYPE log_type = LOG_LOGIC )
{
	return CMemLog::instrance(log_type)->Read();
}
*/




#endif