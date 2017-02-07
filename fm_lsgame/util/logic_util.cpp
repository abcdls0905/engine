
#include <stdarg.h>
#include <stdio.h>
#include <time.h>


#include "logic_util.h"

#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../public/auto_mem.h"

static char g_buf_trace[1024] = { 0 };

bool TraceLog(LogLevelEnum level, const char* format, ...)
{
	result_string strFormat;
	switch(level)
	{
		//PC下打印调试信息
#ifdef SHOW_INFO_LOG
	case LOG_TRACE:
		strFormat = "[Trace-Logic]";
		break;

	case LOG_WARNING:
		strFormat = "[Warning-Logic]";
		break;
#endif

	case LOG_INFO:
		strFormat = "[Info-Logic]";
		break;

	case LOG_ERROR:
		strFormat = "[Error-Logic]";
		break;

	case LOG_CRITICAL:
		strFormat = "[Critical-Logic]";
		break;

	default:
		return false;
	}
	strFormat += format;

	va_list args;

	va_start(args, format);

	vsprintf(g_buf_trace, strFormat.c_str(), args); // C4996

	//strcat(g_buf_trace, "\r\n");

	CORE_TRACE(g_buf_trace);


	return true;
}

// 加载XML文件
bool load_xml_doc(TiXmlDocument& doc, const char* file_name)
{
	FILE* fp = core_file::fopen(file_name, "rb");

	if (NULL == fp)
	{
		return false;
	}

	core_file::fseek(fp, 0, SEEK_END);

	size_t file_size = core_file::ftell(fp);

	core_file::fseek(fp, 0, SEEK_SET);

	TAutoMem<char, 256> auto_buf(file_size + 1);

	char* buffer = auto_buf.GetBuffer();

	if (core_file::fread(buffer, sizeof(char), file_size, fp) != file_size)
	{
		core_file::fclose(fp);
		return false;
	}

	core_file::fclose(fp);

	buffer[file_size] = 0;

	doc.Parse(buffer);

	if (doc.Error())
	{
		return false;
	}

	return true;
}