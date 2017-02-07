//--------------------------------------------------------------------
// �ļ���:		aux_tool.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��4��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _AUX_H
#define _AUX_H

#include "../math/fm_math.h"

void aux_initialize();
void aux_release();
void aux_flush();
void aux_draw_line3(const FmVec3* start, const FmVec3* end, 
	unsigned int cr_start, unsigned int cr_end);
void aux_draw_bound_box(const FmVec3* min, const FmVec3* max, 
	unsigned int cr_start, unsigned int cr_end, float scale);
void aux_draw_point(const FmVec3* position, unsigned int cr, 
	float size, bool balance);

#endif // _AUX_H
