//--------------------------------------------------------------------
// 文件名:		var_type.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_VAR_TYPE_H
#define _PUBLIC_VAR_TYPE_H

// 变量类型

enum VARIANT_TYPE_ENUM
{
	VTYPE_UNKNOWN,	// 未知
	VTYPE_BOOL,		// 布尔
	VTYPE_INT,		// 32位整数
	VTYPE_INT64,	// 64位整数
	VTYPE_FLOAT,	// 单精度浮点数
	VTYPE_DOUBLE,	// 双精度浮点数
	VTYPE_STRING,	// 字符串
	VTYPE_WIDESTR,	// 宽字符串
	VTYPE_OBJECT,	// 对象号
	VTYPE_POINTER,	// 指针
	VTYPE_USERDATA,	// 用户数据
    VTYPE_TABLE,	// 表
	VTYPE_MAX,
};

#endif // _PUBLIC_VAR_TYPE_H
