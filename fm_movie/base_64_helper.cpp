//--------------------------------------------------------------------
// 文件名:		Base64Helper.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年9月11日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "base_64_helper.h"


static const char* s_strBase64EncodeChars = 
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char s_strBase64DecodeChars[] = 
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

result_string Base64Helper::Encode(const char* in_str)
{
	result_string out_str;
	unsigned char c1, c2, c3;
	size_t i = 0;
	size_t len = strlen(in_str);

	while (i < len)
	{
		// read the first byte
		c1 = in_str[i++];

		// pad with "="
		if (i == len)
		{
			out_str += s_strBase64EncodeChars[c1 >> 2];
			out_str += s_strBase64EncodeChars[(c1 & 0x3) << 4];
			out_str += "==";

			break;
		}

		// read the second byte
		c2 = in_str[i++];

		// pad with "="
		if (i == len)
		{
			out_str += s_strBase64EncodeChars[c1 >> 2];
			out_str += s_strBase64EncodeChars[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];
			out_str += s_strBase64EncodeChars[(c2 & 0xF) << 2];
			out_str += "=";

			break;
		}

		// read the third byte
		c3 = in_str[i++];

		// convert into four bytes string
		out_str += s_strBase64EncodeChars[c1 >> 2];
		out_str += s_strBase64EncodeChars[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];
		out_str += s_strBase64EncodeChars[((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6)];
		out_str += s_strBase64EncodeChars[c3 & 0x3F];
	}

	return out_str;
}

result_string Base64Helper::Decode(const char* in_str)
{
	result_string out_str;
	char c1, c2, c3, c4;
	size_t i = 0;
	size_t len = strlen(in_str);

	while (i < len)
	{
		// read the first byte
		do
		{
			c1 = s_strBase64DecodeChars[in_str[i++]];
		} while (i < len && c1 == -1);

		if (c1 == -1)
		{
			break;
		}

		// read the second byte
		do
		{
			c2 = s_strBase64DecodeChars[in_str[i++]];
		} while (i < len && c2 == -1);

		if (c2 == -1)
		{
			break;
		}

		// assamble the first byte
		out_str += char((c1 << 2) | ((c2 & 0x30) >> 4));

		// read the third byte
		do
		{
			c3 = in_str[i++];

			// meet with "=", break
			if (c3 == 61)
			{
				return out_str;
			}

			c3 = s_strBase64DecodeChars[c3];
		} while (i < len && c3 == -1);

		if (c3 == -1)
		{
			break;
		}

		// assamble the second byte
		out_str += char(((c2 & 0XF) << 4) | ((c3 & 0x3C) >> 2));

		// read the fourth byte
		do
		{
			c4 = in_str[i++];

			// meet with "=", break
			if (c4 == 61)
			{
				return out_str;
			}

			c4 = s_strBase64DecodeChars[c4];
		} while (i < len && c4 == -1);

		if (c4 == -1)
		{
			break;
		}

		// assamble the third byte
		out_str += char(((c3 & 0x03) << 6) | c4);
	}

	return out_str;
}
