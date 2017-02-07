//--------------------------------------------------------------------
// 文件名:		render_draw_op.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年7月16日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _RENDER_DRAW_OP_H_
#define _RENDER_DRAW_OP_H_



#include "../visual/i_resource.h"

#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "render.h"

#define MAX_VERTEX_ATTERIB   32

// Clearing the Buffers
class CClearBufferOp
{
public:
	void SetClearColor(const FmVec4& vec4Color)
	{
		m_vec4Color = vec4Color;
	}

	bool EqualClearColor(const FmVec4& vec4Color)
	{
		return m_vec4Color == vec4Color;
	}

	void SetClearDepth(float fDepth)
	{
        m_fDepth = fDepth;
	}

	bool EqualClearDepth(float fDepth)
	{
		return m_fDepth == fDepth;
	}

	void SetClearStencil(int s)
	{
		m_nStencil = s;
	}

	bool EqualClearStencil(int s)
	{
		return m_nStencil == s;
	}

	// 设置清楚操作标记
	void SetClear(fm_uint Flat)
	{
        m_nFlat = Flat;
	}
private:
    FmVec4 m_vec4Color;
	float  m_fDepth;
	int    m_nStencil;
	fm_uint m_nFlat;
};

// Vertices Op
class CVertexOp
{
public:

	CVertexOp()
	{
		for(size_t i = 0; i < MAX_VERTEX_ATTERIB; i ++)
		{
			m_bVertexAtteribEnable[i] = false;
		}
	}

	//顶点属性描述器
	class CVertexAtterib
	{
	public:
		CVertexAtterib()
		{
			m_index = 0;
			m_size = 0;
			m_type = IRenderDrawOp::VERTEX_DATA_FLOAT;
			m_stride = 0;
			m_ptr = 0;
			m_bindVB = 0;
		}
		void SetVertexAttribPointer(unsigned int index,int size,IRenderDrawOp::VERTEX_DATA_TYPE type, fm_sizei stride, const void* ptr,fm_uint nBindVB)
		{
			m_index = index;
			m_size = size;
			m_type = type;
			m_stride = stride;
			m_ptr = ptr;
		}

		bool EqualVertexAttribPointer(unsigned int index,int size,IRenderDrawOp::VERTEX_DATA_TYPE type, fm_sizei stride, const void* ptr)
		{
			return (m_index == index) &&(m_size == size) && (m_type == type) && ( m_stride == stride) && (m_ptr == ptr);
		}
	private:
		unsigned int m_index;
		int m_size;
		IRenderDrawOp::VERTEX_DATA_TYPE m_type;
		fm_sizei m_stride;
		const void* m_ptr;
		fm_uint m_bindVB;
	};
public:
	// Set Vertex Attrib Format Info
	void SetVertexAttribPointer(unsigned int index,int size,IRenderDrawOp::VERTEX_DATA_TYPE type, fm_sizei stride, const void* ptr ,fm_uint nBindVB)
	{
		if(index < MAX_VERTEX_ATTERIB)
		{
            m_VertexAtterib[index].SetVertexAttribPointer(index,size,type,stride,ptr,nBindVB);
		}
	}

	bool EqualVertexAttribPointer(unsigned int index,int size,IRenderDrawOp::VERTEX_DATA_TYPE type, fm_sizei stride, const void* ptr )
	{
  		if(index >= MAX_VERTEX_ATTERIB)
		{     
			return false;
		}

        return m_VertexAtterib[index].EqualVertexAttribPointer(index,size,type,stride,ptr);
	}

	void EnableVertexAttribArray(unsigned int index,bool bEnable)
	{
		if(index < MAX_VERTEX_ATTERIB)
		{
           m_bVertexAtteribEnable[index] = bEnable;
		}
	}
	bool IsEnableVertexAttribArray(unsigned int index)
	{
   		if(index >= MAX_VERTEX_ATTERIB)
		{     
			return false;
		}
        return m_bVertexAtteribEnable[index];  
	}
private:
	CVertexOp::CVertexAtterib m_VertexAtterib[MAX_VERTEX_ATTERIB];
	bool m_bVertexAtteribEnable[MAX_VERTEX_ATTERIB];
};

// Draw Op
class CDrawOp
{
public:
    enum DRAW_TYPE
	{
		DRAW_ARRAY,
		DRAW_INDEX,
	};

	class CDrawArray
	{
	public:
		void SetDrawArrays(IRenderDrawOp::DRAW_MODE draw_mode,int first,fm_sizei count)
		{
            m_draw_mode = draw_mode;
			m_first = first;
			m_count = count;
		}
	private:
        IRenderDrawOp::DRAW_MODE m_draw_mode;
		int m_first;
        fm_sizei m_count;
	};

	class CDrawIndex
	{
	public:
		void DrawIndex(IRenderDrawOp::DRAW_MODE draw_mode,fm_sizei count,IRenderDrawOp::VERTEX_INDEX_TYPE index_type,void* indices)
		{
			m_draw_mode = draw_mode;
            m_count = count;
            m_index_type = index_type;
            m_indices = indices;
		}
	private:
        IRenderDrawOp::DRAW_MODE m_draw_mode;
        fm_sizei m_count;
        IRenderDrawOp::VERTEX_INDEX_TYPE m_index_type;
        void* m_indices;
	};
public:
	void DrawArrays(IRenderDrawOp::DRAW_MODE draw_mode,int first,fm_sizei count)
	{
		m_draw_type = DRAW_ARRAY;
		m_draw_array.SetDrawArrays(draw_mode,first,count);
	}

	void DrawIndex(IRenderDrawOp::DRAW_MODE draw_mode,fm_sizei count,IRenderDrawOp::VERTEX_INDEX_TYPE index_type,void* indices)
	{
        m_draw_type = DRAW_INDEX;
		m_draw_index.DrawIndex(draw_mode,count,index_type,indices);
	}

	CDrawOp::DRAW_TYPE GetDrawType()
	{
		return m_draw_type;
	}

	// set vb
	void SetVB(fm_uint vb)
	{
		m_vb = vb;
	}

    fm_uint GetVB() const
	{
		return m_vb;
	} 

	// set ib
	void SetIB(fm_uint ib)
	{
		m_ib = ib;
	}

	fm_uint GetIB() const
	{
		return m_ib;
	}
private:
	fm_uint m_vb;
	fm_uint m_ib;

	CDrawOp::CDrawArray m_draw_array;
	CDrawOp::CDrawIndex m_draw_index;

	DRAW_TYPE m_draw_type;
};

// 渲染绘制相关操作函数
class CRenderDrawOp :public IRenderDrawOp
{
public:
	static IRenderDrawOp* NewInstance(Render* pRender);  
	// 释放
	void Release();

	CRenderDrawOp(Render* pRender);

	CRenderDrawOp(){};
public:
    // clear buffer op
	virtual void SetClearColor(const FmVec4& vec4Color);
	virtual void SetClearDepth(float fDepth);
	virtual void SetClearStencil(int s);
	virtual void SetClear(fm_uint Flat);

	// vertex op
	// Set Vertex Attrib Format Info
	virtual void SetVertexAttribPointer(unsigned int index,int size,IRenderDrawOp::VERTEX_DATA_TYPE type, fm_sizei stride, const void* ptr );

	virtual void EnableVertexAttribArray(unsigned int index,bool bEnable = true);
	virtual bool IsEnableVertexAttribArray(unsigned int index) ;

	// draw call func
	virtual void DrawArrays(IRenderDrawOp::DRAW_MODE draw_mode,int first,fm_sizei count);
	virtual void DrawIndex(IRenderDrawOp::DRAW_MODE draw_mode,fm_sizei count,IRenderDrawOp::VERTEX_INDEX_TYPE index_type,void* indices);

	// set vb
	virtual void SetVB(fm_uint vb);
	// set ib
	virtual void SetIB(fm_uint ib);

	void Print();
private:
    //提供一次draw的数据内容
	void  CaptureDrawCall();

	unsigned int ClearBufferFlatToGLClearBufferFlat(fm_uint Flat);
	unsigned int DrawModeToGLDrawMode(IRenderDrawOp::DRAW_MODE draw_mode);
    unsigned int VertexIndexTypeToGLVertexIndexType(IRenderDrawOp::VERTEX_INDEX_TYPE index_type);
	unsigned int VertexDataTypeToGLVertexDataType(IRenderDrawOp::VERTEX_DATA_TYPE data_type);
private:
    CClearBufferOp m_cClearBufferOp;//buffer清除操作
	CVertexOp      m_cVertexOp;
    CDrawOp        m_cDrawOp;

	bool           m_bSetVB;

	Render* m_pRender;
};


#endif