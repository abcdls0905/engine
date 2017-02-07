#pragma once

#include "../visual/vis_utils.h"
#include "../visual/i_texture.h"
#include "../visual/i_context.h"

enum {
		FVF_POS		=	0x00000001, // 顶点位置信息标记
		FVF_TEX0	=	0x00000002	// 
};

class IUIVisNode
{
protected:
	FmMat4* m_mtxGlobal;
	FmMat4 m_mtxWorld;
	FmVec3 m_pos, m_angle, m_scale;
	virtual void OnChangedWorldMatrix(){};

	inline void SetGlobal(FmMat4* global) 
	{ 
		m_mtxGlobal = global; 
		BuildWorldMatrix(); 
	}

	inline void BuildWorldMatrix()
	{
		FmQuat rotate;
		FmQuatRotationYawPitchRoll(&rotate, m_angle.y, m_angle.x, m_angle.z);
		VisUtil_MatrixTransform(&m_mtxWorld, &m_scale, &rotate, &m_pos);
		if(m_mtxGlobal)
		{
			::FmMat4Multiply(&m_mtxWorld, &m_mtxWorld, m_mtxGlobal);
		}
		OnChangedWorldMatrix();
	}
public:
	IUIVisNode()
	:m_mtxGlobal(0)
	{
		m_pos = FmVec3(0,0,0);
		m_angle = FmVec3(0,0,0);
		m_scale = FmVec3(1, 1, 1);
		BuildWorldMatrix();
	}
	virtual ~IUIVisNode(){}

	// 位置
	virtual bool SetPosition(float x, float y, float z) { return SetPosition(FmVec3(x, y, z)); }

	virtual bool SetPosition(const FmVec3& value)
	{
		m_pos = value; 
		BuildWorldMatrix();
		return true;
	}

	virtual FmVec3 GetPosition() const { return m_pos; }
	// 方向
	virtual bool SetAngle(float x, float y, float z) { return SetAngle(FmVec3(x, y, z)); }
	virtual bool SetAngle(const FmVec3& value)
	{
		m_angle = value;
		BuildWorldMatrix();
		return true;
	}

	virtual FmVec3 GetAngle() const { return m_angle; }
	// 缩放系数
	virtual bool SetScale(float x, float y, float z) { return SetScale(FmVec3(x, y, z)); }
	virtual bool SetScale(const FmVec3& value)
	{
		m_scale = value;
		BuildWorldMatrix();
		return true;
	}

	virtual FmVec3 GetScale() const { return m_scale; }
};

class IUI3DNode : public IUIVisNode
{
public:
	virtual void Create(int fvf, char * vb_data, int vb_data_size, short* ib_data, int ib_data_size) = 0;
	virtual void SetTex0(ITexture* tex0) = 0;
	virtual bool Pick(IRenderContext* pContext, int screen_x, int screen_y, FmVec2& uv, float& t) = 0;
};

class IUI3DMesh : public IUIVisNode
{
public:
	virtual void Draw() = 0;

	virtual IUI3DNode* CreateNode(const char* ident ="") = 0;

	virtual IUI3DNode* GetNode(int index) = 0;

	virtual size_t GetNodeCount()const  = 0;

	virtual bool Pick(IRenderContext* pContext, int screen_x, int screen_y, FmVec2& uv, int& node_index) = 0;
};