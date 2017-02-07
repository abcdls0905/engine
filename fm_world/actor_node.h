//--------------------------------------------------------------------
// 文件名:		actor_node.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年2月3日
// 创建人:		惠军超
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ACTOR_NODE_H
#define _ACTOR_NODE_H

#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/i_model_system.h"
#include "../utils/array_pod.h"
#include "../visual/i_vis_base.h"

// 新组合模型节点

class Actor;

class CActorNode
{
public:
	CActorNode();
	~CActorNode();

	// 获得包围盒
	bool GetBoundingBox(FmVec3& v_min, FmVec3& v_max);

	// 释放自身及所有子节点
	void ReleaseAll();

	// 设置组合模型对象指针
	void SetActor(Actor* value) { m_pActor = value; }

	// 获得节点物体
	IVisBase* GetVisBase() const
	{
		return (IVisBase*)g_pCore->GetEntity(m_VisualID);
	}

	void GetWorldMatrix(FmMat4& mat) const;
	
	// 名称
	void SetName(const char* value);
	const char* GetName() const { return m_strName.c_str(); }

	// 点名称
	void SetPoint(const char* value);
	const char* GetPoint() const { return m_strPoint.c_str(); }

	// 物体对象号
	void SetVisualID(const PERSISTID& value);
	PERSISTID GetVisualID() const { return m_VisualID; }

	// 获取当前模型是否存在主模（裸模）
	bool ExistMainModel();
	// 指定当前模型仅对主模（裸模）进行碰撞检测
	void SetTraceMainModel(bool value);
	// 获取当前模型是否仅对主模（裸模）进行碰撞检测
	bool GetTraceMainModel();

	// 设置是否显示包围盒
	void SetShowBoundBox(bool value);

	// 设置残影ID
	void SetBrokenShadowID(const PERSISTID& value);
	PERSISTID GetBrokenShadowID() const { return m_BrokenShadow; }

	// 取得存在可视对象的节点列表
	void GetVisualNodeList(IVarList& result);

	// 获得可见对象 
	void GetVisualList(IVarList& result);

	// 设置动画播放器
	bool SetActionPlayer(IActionPlayer* ani_player);

	// 父亲节点
	void SetParentNode(CActorNode* pNode);
	CActorNode* GetParentNode() const { return m_pParentNode; }

	// 添加子节点
	CActorNode* AddChild(const char* name, const char* point, 
		const PERSISTID& id);
	// 清空子节点
	bool ClearChild();
	// 删除子节点
	bool DeleteChild(const char* pName);
	// 获得子节点数量
	size_t GetChildCount() const;
	// 获得子节点实体对象数
	size_t GetVisBaseCount() const;
	// 获得指定索引子节点
	CActorNode* GetChildByIndex(size_t index) const;
	// 获得指定名字的节点
	CActorNode* GetNode(const char* name);
	
	void Update(float seconds);
	void Realize();
	void RealizeShadowMap();

	// 更新对象位置
	void UpdateMatrix(IVisBase* pVisBase);
	// 更新节点(包括其子节点)
	void UpdateNode();

	// 加载数据是否成功
	bool GetLoaded();
	// 是否加载成功
	bool GetLoadFinish();
    // 是否有效
    bool IsValid();

	// 连接位置
	bool SetLocalPosition(float x, float y, float z);
	FmVec3 GetLocalPosition();

	// 连接方向
	bool SetLocalAngle(float x, float y, float z);
	FmVec3 GetLocalAngle();

	// 连接缩放系数
	bool SetLocalScale(float x, float y, float z); 
	FmVec3 GetLocalScale();

	// 设置在水下
	bool SetUnderWater(bool value);
	// 设置输出到速度纹理
	bool SetWriteVelocity(int value);
	// 设置颜色
	bool SetColor(unsigned int value);
	// 设置父颜色
	bool SetParentColor(unsigned int value);
	// 设置细节等级
	bool SetLevelOfDetail(float value);

	// 碰撞测试详细信息
	bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// 碰撞测试是否有阻挡
	bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// 球体范围碰撞
	bool TraceSphere(const FmVec3& center, float radius);
	// 三角形碰撞
    bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2);
	// 拣选
	bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// 保持连接对象方向是否保持绑定方向
	void SetAngleKeep(bool value);
	bool GetAngleKeep() const { return m_bAngleKeep; }

	// 设置是否是参与碰撞和剪裁的实体
	void SetIsEsseObj(bool value);

	// 设置子节点是否可见
	void SetNodeVisible(const char* name, bool value);
	bool GetNodeVisible(const char* name);

	// 设置是否是参与碰撞和剪裁的实体
	void SetEsseObj(const char* name, bool value);
	bool GetEsseObj(const char* name);

private:
	CActorNode(const CActorNode&);
	CActorNode& operator=(const CActorNode&);

	// 获得指定名字的节点
	CActorNode* GetNode(const char* name, const unsigned int hash);

private:
	Actor* m_pActor;
	core_string m_strName;
	unsigned int m_nHash;
	core_string m_strPoint;
	PERSISTID m_VisualID;
	PERSISTID m_BrokenShadow;
	CActorNode* m_pParentNode;
	FmMat4 m_LocalMatrix;
	FmMat4 m_mtxWorld;
	TArrayPod<CActorNode*, 8, TCoreAlloc> m_Children;
	bool m_bAngleKeep;
	bool m_bEsseObj;
	bool m_bNodeVisible;
	unsigned int m_nColor;
};

#endif // _ACTOR_NODE_H
