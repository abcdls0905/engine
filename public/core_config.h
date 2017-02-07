//--------------------------------------------------------------------
// 文件名:		core_config.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年5月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_CORE_CONFIG_H
#define _PUBLIC_CORE_CONFIG_H

#include "fast_str.h"

#define CORE_FLAG_USE_MEMORY_POOL 0x2 // 是否使用内存池
#define CORE_FLAG_MEMORY_VALIDATE 0x4 // 是否校验内存

// 创建核心配置

class CCoreConfig
{
public:
	CCoreConfig()
	{
		m_nSleep = 0;
		m_nFlags = 0;
	}
	
	// 主循环休眠时间
	int m_nSleep;
	// 标志
	int m_nFlags;
	// 工作路径
	TFastStr<char, 128> m_strWorkPath;
	// 程序资源路径
	TFastStr<char, 128> m_strProgramPath;
	// 资源文件路径
	TFastStr<char, 128> m_strResourcePath;
	// 文件系统
	TFastStr<char, 128> m_strFileSystem;
	// 文件系统配置信息
	TFastStr<char, 128> m_strFileConfig;
	// 主实体名称
	TFastStr<char, 128> m_strMainEntity;
};

#endif // _PUBLIC_CORE_CONFIG_H
