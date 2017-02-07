//--------------------------------------------------------------------
// 文件名:		log_file.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _LOG_FILE_H
#define _LOG_FILE_H

#include "../public/macros.h"
#include "../public/fast_str.h"

// 写日志文件的类

class CLogFile
{
public:
	CLogFile();
	CLogFile(const char* filename);
	~CLogFile();

	// 设置文件名
	void SetFileName(const char* filename);
	// 获得文件名
	const char* GetFileName() const;

	// 写日志信息（自动加时间）
	bool SaveLog(const char* log) const;
	// 清除日志
	bool Clear() const;

private:
	CLogFile(const CLogFile&);
	CLogFile& operator=(const CLogFile&);

private:
	TFastStr<char, 128> m_strFileName;
};

#endif // _LOG_FILE_H
