//--------------------------------------------------------------------
// �ļ���:		Base64Helper.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��9��11��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
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
