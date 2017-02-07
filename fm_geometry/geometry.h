//--------------------------------------------------------------------
// 文件名:		geometry.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年11月25日
// 创建人:		叶峰
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"
#include "../visual/shader_list.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "../visual/i_geometry.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"

class Geometry : public IGeometry
{
private:
	struct geometry_vertex_t 
	{
		FmVec3 pos;
		FmVec3 nor;
	};

	// 批次渲染数据
	struct BatchData
	{
		IShaderProgram* pShader;
		IStaticVB* pVBuffer;
		IStaticIB* pIBuffer;

		FmVec4 c_vDiffuse;
		FmVec4 c_vLightDirection;
		FmMat4 c_matWVP;
		FmMat4 c_matWorld;

		fm_int hDiffuse;
		fm_int hLightDirection;
		fm_int hMatWVP;
		fm_int hMatWorld;

        bool bZEnable;			//是否深度写入
		bool bLineMode;

        BatchData* pNext;

        BatchData()
        {
            pShader = NULL;
            pVBuffer = NULL;
            pIBuffer = NULL;
            pNext = NULL;
        }
	};

	// 渲染函数
	static void RenderGeometry(void* pdata);

public:
	Geometry();
	virtual ~Geometry();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// 绘制
	virtual void Realize();

    // 设置当前场景视图
    void SetSceneView(ISceneView* pSceneView);

    // 获得当前场景视图
    ISceneView* GetSceneView();

    // 渲染
    virtual void Draw(const FmMat4& mtxWorld);

	// 位置
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// 方向
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// 缩放
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

	// 变换矩阵
	virtual bool SetTransform(const FmMat4& mtx);
	virtual const FmMat4& GetTransform() const;

	// 颜色
	virtual void SetColor(unsigned int value);
	virtual unsigned int GetColor() const;

	// 是否可见
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// 是否画线模式
	virtual void SetLineMode(bool value);
	virtual bool GetLineMode() const;

	// 是否使用深度测试
	void SetZEnable(bool value);
	bool GetZEnable() const;

	// 创建
	bool Build(const YsGraphDesc& graphDesc);
	void Unbuild();

	// 是否有效
	bool IsReady() const;

private:
	void UpdateTransform();

    BatchData* GetBatchData();

    void UpdateBatchData(BatchData* pBatchData, const FmMat4& mtxWorld);

private:
    IRender* m_pRender;
    ISceneView* m_pSceneView;

	IStaticVB* m_pVB;
	IStaticIB* m_pTrianglesIB;
	IStaticIB* m_pLinesIB;

	fm_int m_hDiffuse;
	fm_int m_hLightDirection;
	fm_int m_hMatWVP;
	fm_int m_hMatWorld;

	FmVec3 m_vPosition;
	FmVec3 m_vAngle;
	FmVec3 m_vScale;
	FmMat4 m_matWorld;

	FmVec4 m_vColor;

	IShaderProgram* m_pShaderProgram;

    // 渲染批次
    BatchData* m_listBatchData;
    long m_nBatchCount;

	bool m_bVisible;
	bool m_bLineMode;
	bool m_bZEnable;
};

#endif // _GEOMETRY_H
