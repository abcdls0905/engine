//--------------------------------------------------------------------
// �ļ���:		light_types.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��11��5��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
/*
#ifndef _LIGHT_TYPES_H
#define _LIGHT_TYPES_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/dx_header.h"

// ��Դ
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

	float red;			// ��ɫ����
	float green;
	float blue;

	FmVec4 color;	// ��ɫ
	FmVec4 pos;	// λ��
	FmVec4 dir;	// ����
	FmVec4 size;	// ���ӹ�Դ�ߴ�
	float att0;			// ˥��ϵ��0
	float att1;			// ˥��ϵ��1
	float att2;			// ˥��ϵ��2
	float range;		// ��Χ
	float inner;		// �۹���ڽǻ���
	float outer;		// �۹����ǻ���
	float falloff;		// �۹��FALLOFF
	core_string group; // ����

	float cosine;		// �Ƕ�ϵ��
	float shadow;		// ��Ӱ��ɢ
	float bright;		// ����
	float contr;		// �Աȶ�
	float gamma;		// Gamma
	float curgm;		// ��ǰGamma
	float curct;		// ��ǰ�Աȶ�

	Type type;			// ����
	bool enable;		// �Ƿ�����

	FmMat4 mtxWorldInverse;
};

// ͶӰ
struct shadow_t
{
	//double v;			// Ӱ��ϵ��
	//double nrm;			// ���ϵ��
	//double sm;			// ͶӰϵ��
	float v;			// Ӱ��ϵ��
	float nrm;			// ���ϵ��
	float sm;			// ͶӰϵ��
	float cs;			// �Ƕ�
	float att;			// ˥��
	float dst;			// ����
	float csatt;		// �Ƕȳ�˥��
	float shw;			// ��Ӱǿ��
	//FmVec4 c;		// ��ɫ
};

// ����
struct vertex_t
{
	enum Flag
	{
		f_zero = 0,			
		f_inv = 1,			
		f_set = 2,			
		f_bug = 4,			
	};

	FmVec4 p;		// λ��
	FmVec4 n;		// ����
	FmVec4 c;		// ��ɫ
	FmVec4 bc;		// ģ��
	FmVec4 mc;		// ģ����ɫ
	FmVec4 ao;		// �����ڵ���ɫ
	unsigned int alpha;	// ͸����
	int flags;			// ��־
	shadow_t* shadow;	// ͶӰ����
};

// ��
struct triangle_t
{
	FmVec4 p;		// ���ĵ�����
	FmVec4 n;		// ����
	float sq;			// ���߳���
	int i[3];			// ��������
};

// ���Ҷ�����
struct oct_find_vertex_t
{
	vertex_t* v;		// ����
	float r2;			// �����ƽ��
};

#endif // _LIGHT_TYPES_H
*/