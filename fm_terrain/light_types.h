//--------------------------------------------------------------------
// 文件名:		light_types.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#ifndef _LIGHT_TYPES_H
#define _LIGHT_TYPES_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/dx_header.h"

// 光源
struct light_t
{
	enum Type
	{
		t_none,
		t_amb,
		t_sun,
		t_sky,
		t_point,
		t_spot,
		t_box,
	};

	float red;			// 颜色分量
	float green;
	float blue;

	FmVec4 color;	// 颜色
	FmVec4 pos;	// 位置
	FmVec4 dir;	// 朝向
	FmVec4 size;	// 盒子光源尺寸
	float att0;			// 衰减系数0
	float att1;			// 衰减系数1
	float att2;			// 衰减系数2
	float range;		// 范围
	float inner;		// 聚光灯内角弧度
	float outer;		// 聚光灯外角弧度
	float falloff;		// 聚光灯FALLOFF
	core_string group; // 名称

	float cosine;		// 角度系数
	float shadow;		// 阴影扩散
	float bright;		// 亮度
	float contr;		// 对比度
	float gamma;		// Gamma
	float curgm;		// 当前Gamma
	float curct;		// 当前对比度

	Type type;			// 类型
	bool enable;		// 是否启用

	FmMat4 mtxWorldInverse;
};

// 投影
struct shadow_t
{
	//double v;			// 影响系数
	//double nrm;			// 面积系数
	//double sm;			// 投影系数
	float v;			// 影响系数
	float nrm;			// 面积系数
	float sm;			// 投影系数
	float cs;			// 角度
	float att;			// 衰减
	float dst;			// 距离
	float csatt;		// 角度乘衰减
	float shw;			// 阴影强度
	//FmVec4 c;		// 颜色
};

// 顶点
struct vertex_t
{
	enum Flag
	{
		f_zero = 0,			
		f_inv = 1,			
		f_set = 2,			
		f_bug = 4,			
	};

	FmVec4 p;		// 位置
	FmVec4 n;		// 法线
	FmVec4 c;		// 颜色
	FmVec4 bc;		// 模糊
	FmVec4 mc;		// 模型颜色
	FmVec4 ao;		// 环境遮挡颜色
	unsigned int alpha;	// 透明度
	int flags;			// 标志
	shadow_t* shadow;	// 投影数据
};

// 面
struct triangle_t
{
	FmVec4 p;		// 中心点坐标
	FmVec4 n;		// 法线
	float sq;			// 法线长度
	int i[3];			// 顶点索引
};

// 查找顶点结果
struct oct_find_vertex_t
{
	vertex_t* v;		// 顶点
	float r2;			// 距离的平方
};

#endif // _LIGHT_TYPES_H
*/