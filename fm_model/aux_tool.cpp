//--------------------------------------------------------------------
// 文件名:		aux_tool.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "aux_tool.h"
#include "array.h"
#include "../visual/i_render.h"

enum {
	DX_PRIMITIVE_LINELIST,
	DX_PRIMITIVE_TRIANGLELIST,
};

extern IRender* g_pRender;

struct aux_vertex_t
{
	float x;
	float y;
	float z;
	unsigned int diffuse;
};

static array_t* s_pAuxVB = NULL;
static int	s_nAuxDrawType = DX_PRIMITIVE_LINELIST;
static unsigned int	s_nLimitAuxBufferSize = 1024 * 1024;

void aux_initialize()
{
	s_pAuxVB = array_create(sizeof(aux_vertex_t));
}

void aux_release()
{
	if (s_pAuxVB != NULL)
	{
		array_release(s_pAuxVB);
		s_pAuxVB = NULL;
	}
}

void aux_flush()
{
	int n = array_get_size(s_pAuxVB);

	if (n == 0)
	{
		return;
	}

	int nPrimitive;

	switch (s_nAuxDrawType)
	{
	case DX_PRIMITIVE_LINELIST:
		nPrimitive = n / 2;
		break;
	case DX_PRIMITIVE_TRIANGLELIST:
		nPrimitive = n / 3;
		break;
	default:
		return;
	}

	FmMat4 mtxWorld;
	
	FmMatrixIdentity(&mtxWorld);

	if (nPrimitive != 0)
	{
		g_pRender->DrawDesignLine(mtxWorld, s_nAuxDrawType, nPrimitive, 
			array_get_buffer(s_pAuxVB), sizeof(aux_vertex_t));
		array_clean(s_pAuxVB);
	}
}

void aux_draw_line3(const FmVec3* start, const FmVec3* end, 
	unsigned int cr_start, unsigned int cr_end)
{
	aux_vertex_t vertex;

	if (s_nAuxDrawType != DX_PRIMITIVE_LINELIST)
	{
		aux_flush();
		s_nAuxDrawType = DX_PRIMITIVE_LINELIST;
	}

	// 保证辅助缓冲区不要占用过多的内存，如果发现它使用了很多内存了，立刻Flush
	// 另外，这行也可防止用户在进行辅助绘制(例如调用:aux_draw_line3)时忘记Flush而导致
	// 内存用尽的Bug
	if (array_get_size(s_pAuxVB) * array_get_element_size(s_pAuxVB) 
		>= s_nLimitAuxBufferSize)
	{
		aux_flush();
	}

	vertex.x = start->x;
	vertex.y = start->y;
	vertex.z = start->z;
	vertex.diffuse = cr_start;

	array_push_back(s_pAuxVB, &vertex);

	vertex.x = end->x;
	vertex.y = end->y;
	vertex.z = end->z;
	vertex.diffuse = cr_end;

	array_push_back(s_pAuxVB, &vertex);
}

void aux_draw_triangle(const FmVec3* p0, const FmVec3* p1, 
	const FmVec3* p2, unsigned int cr)
{
	aux_vertex_t vertex;

	if (s_nAuxDrawType != DX_PRIMITIVE_TRIANGLELIST)
	{
		aux_flush();
		s_nAuxDrawType = DX_PRIMITIVE_TRIANGLELIST;
	}

	// 保证辅助缓冲区不要占用过多的内存，如果发现它使用了很多内存了，立刻Flush
	// 另外，这行也可防止用户在进行辅助绘制(例如调用:aux_draw_line3)时忘记Flush而导致
	// 内存用尽的Bug
	if (array_get_size(s_pAuxVB) * array_get_element_size(s_pAuxVB)
		>= s_nLimitAuxBufferSize)
	{
		aux_flush();
	}

	vertex.x = p0->x;
	vertex.y = p0->y;
	vertex.z = p0->z;
	vertex.diffuse = cr;

	array_push_back(s_pAuxVB, &vertex);

	vertex.x = p1->x;
	vertex.y = p1->y;
	vertex.z = p1->z;
	vertex.diffuse = cr;

	array_push_back(s_pAuxVB, &vertex);

	vertex.x = p2->x;
	vertex.y = p2->y;
	vertex.z = p2->z;
	vertex.diffuse = cr;

	array_push_back(s_pAuxVB, &vertex);
}

void aux_draw_bound_box(const FmVec3* min, const FmVec3* max, 
	unsigned int cr_start, unsigned int cr_end, float scale)
{
	Assert(min != NULL);
	Assert(max != NULL);

	FmVec3 sub, temp, tmin, tmax, tstart;

	tmin = FmVec3(min->x - scale, min->y - scale, min->z - scale);
	tmax = FmVec3(max->x + scale, max->y + scale, max->z + scale);

	sub.x = tmax.x - tmin.x;
	sub.y = tmax.y - tmin.y;
	sub.z = tmax.z - tmin.z;

	temp = FmVec3(tmin.x, tmin.y, tmin.z + sub.z * 0.2F);		// Max
	aux_draw_line3(&tmin, &temp, cr_start, cr_end);
	temp = FmVec3(tmin.x, tmin.y + sub.y* 0.2F, tmin.z);
	aux_draw_line3(&tmin, &temp, cr_start, cr_end);
	temp = FmVec3(tmin.x + sub.x* 0.2F, tmin.y, tmin.z);
	aux_draw_line3(&tmin, &temp, cr_start, cr_end);

	temp = FmVec3(tmax.x, tmax.y, tmax.z - sub.z * 0.2F);		// Min
	aux_draw_line3(&tmax, &temp, cr_start, cr_end);
	temp = FmVec3(tmax.x, tmax.y - sub.y* 0.2F, tmax.z);
	aux_draw_line3(&tmax, &temp, cr_start, cr_end);
	temp = FmVec3(tmax.x - sub.z* 0.2F, tmax.y, tmax.z);
	aux_draw_line3(&tmax, &temp, cr_start, cr_end);

	tstart = FmVec3(tmin.x, tmin.y, tmax.z);					// x min, y min, z max
	temp = FmVec3(tmin.x, tmin.y, tmax.z - sub.z * 0.2F);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmin.x, tmin.y + sub.y* 0.2F, tmax.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmin.x + sub.x* 0.2F, tmin.y, tmax.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);

	tstart = FmVec3(tmin.x, tmax.y, tmin.z);					// x min, y max, z min
	temp = FmVec3(tmin.x, tmax.y, tmin.z + sub.z * 0.2F);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmin.x, tmax.y - sub.y* 0.2F, tmin.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmin.x + sub.x* 0.2F, tmax.y, tmin.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);

	tstart = FmVec3(tmax.x, tmin.y, tmin.z);					// x max, y min, z min
	temp = FmVec3(tmax.x, tmin.y, tmin.z + sub.z * 0.2F);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmax.x, tmin.y + sub.y* 0.2F, tmin.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmax.x - sub.x* 0.2F, tmin.y, tmin.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);

	tstart = FmVec3(tmin.x, tmax.y, tmax.z);					// x min, y max, z max
	temp = FmVec3(tmin.x, tmax.y, tmax.z - sub.z * 0.2F);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmin.x, tmax.y - sub.y* 0.2F, tmax.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmin.x + sub.x* 0.2F, tmax.y, tmax.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);

	tstart = FmVec3(tmax.x, tmax.y, tmin.z);					// x min, y max, z max
	temp = FmVec3(tmax.x, tmax.y, tmin.z + sub.z * 0.2F);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmax.x, tmax.y - sub.y* 0.2F, tmin.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmax.x - sub.x* 0.2F, tmax.y, tmin.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);

	tstart = FmVec3(tmax.x, tmin.y, tmax.z);					// x max, y min, z max
	temp = FmVec3(tmax.x, tmin.y, tmax.z - sub.z * 0.2F);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmax.x, tmin.y + sub.y* 0.2F, tmax.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);
	temp = FmVec3(tmax.x - sub.x* 0.2F, tmin.y, tmax.z);
	aux_draw_line3(&tstart, &temp, cr_start, cr_end);

	// 马上就画
	aux_flush();
}

void aux_draw_point(const FmVec3* position, unsigned int cr, float size, 
	bool balance)
{
	Assert(position != NULL);

	FmVec3 start, end;

	if (!balance)
	{
		end = start = *position;
		start.x = position->x - size;
		end.x = position->x + size;

		aux_draw_line3(&start, &end, cr, cr);

		end = start = *position;
		start.y = position->y - size;
		end.y = position->y + size;

		aux_draw_line3(&start, &end, cr, cr);

		end = start = *position;
		start.z = position->z - size;
		end.z = position->z + size;

		aux_draw_line3(&start, &end, cr, cr);
	}
	else
	{
	}

	// 马上就画
	aux_flush();
}
