//--------------------------------------------------------------------
// 文件名:		Base64Helper.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年9月11日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BASE64HELPER_H
#define _BASE64HELPER_H

#include "../public/fast_str.h"

class Base64Helper
{
public:
	// convert from Base64 to ANSI
	static result_string Encode(const char* in_str);

	// convert from ANSI to Base64
	static result_string Decode(const char* in_str);
};

#endif  // _BASE64HELPER_H
