//--------------------------------------------------------------------
// 文件名:		trigger.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月6日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TRIGGER_H
#define _TRIGGER_H

#include "../public/core_type.h"
#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_vis_base.h"

// 触发器

class Terrain;

class Trigger: public IVisBase
{
public:
	Trigger();
	virtual ~Trigger();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
    
	virtual void Update(float seconds);
	virtual void Realize(float offset_seconds);
	
	// 位置
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// 方向
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// 缩放 
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

	// 获得世界坐标的中心点
	virtual FmVec3 GetCenter();
	// 获得包围盒尺寸
	virtual FmVec3 GetBoxSize();
	// 获得半径
	virtual float GetRadius();

	// 碰撞测试详细信息
	virtual bool TraceDetail(const FmVec4& src, const FmVec4& dst, 
		trace_info_t& result);

	// 是否可见
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// 是否检测碰撞
	virtual void SetTraceEnable(bool value) { m_bTraceEnable = value; }
	virtual bool GetTraceEnable() const { return m_bTraceEnable; }

	// 边框尺寸
	void SetSizeX(float value);
	float GetSizeX() const; 
	void SetSizeY(float value);
	float GetSizeY() const; 
	void SetSizeZ(float value);
	float GetSizeZ() const; 

	// 设置所属的地形对象
	void SetTerrain(Terrain* pTerrain) { m_pTerrain = pTerrain; }
	void SetTerrainID(const PERSISTID& value);
	PERSISTID GetTerrainID() const;
	
	// 是否显示边框
	void SetShowBound(bool value) { m_bShowBound = value; }
	bool GetShowBound() const { return m_bShowBound; }
	
	// 是否显示包围盒
	void SetShowBoundBox(bool value) { m_bShowBoundBox = value; }
	bool GetShowBoundBox() const { return m_bShowBoundBox; }

	// 脚本文件名
	void SetScriptName(const char* value);
	const char* GetScriptName() const;

	// 事件参数
	void SetParameter(const char* value);
	const char* GetParameter() const;

	// 定时周期时长
	void SetPeriod(float value);
	float GetPeriod() const;

	// 绑定对象
	void SetBindID(const PERSISTID& value);
	PERSISTID GetBindID() const { return m_BindID; }
	
	// 绑定位置
	float GetBindPositionX() const { return m_vBindPos.x; }
	float GetBindPositionY() const { return m_vBindPos.y; }
	float GetBindPositionZ() const { return m_vBindPos.z; }

	// 绑定到其他对象
	bool SetBindPosition(float x, float y, float z);

	// 初始化事件
	bool CheckInit();
	// 检查定时事件
	bool CheckTimer(float seconds);
	// 检查玩家事件
	bool CheckPlayer(IVisBase* pPlayer);
	
private:
	// 玩家进入范围
	bool PlayerEntry(IVisBase* pPlayer);
	// 玩家离开范围
	bool PlayerLeave(IVisBase* pPlayer);
	// 更新绑定位置
	bool UpdateBind();

private:
	IRender* m_pRender;
	Terrain* m_pTerrain;
	FmMat4 m_mtxWorld;
	FmVec3 m_vBoxSize;
	PERSISTID m_BindID;
	FmVec3 m_vBindPos;
	bool m_bVisible;
	bool m_bTraceEnable;
	bool m_bShowBound;
	bool m_bShowBoundBox;
	core_string m_strScriptName;
	core_string m_strParameter;
	float m_fPeriod;
	float m_fCounter;
	PERSISTID m_EntryPlayerID;
};

#endif // _TRIGGER_H
