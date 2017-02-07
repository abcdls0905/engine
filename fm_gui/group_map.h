//--------------------------------------------------------------------
// 文件名:		GroupMap.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年4月23日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GROUPMAP_H
#define _GROUPMAP_H

#include "../public/core_mem.h"
#include "../utils/pod_hash_map.h"
#include "i_control.h"
#include "label.h"
#include <string>
#include <vector>
//#include <map>

// 地图内对像显示组合

class GroupMap: public IControl
{
	enum
	{
		ShapeType_Rect = 1,		// 矩形
		ShapeType_Round = 2,	// 圆形
	};

	enum
	{
		PicType_Glyph = 0,		// 图片
		PicType_Animation = 1,	// 动画
		PicType_Max,
	};

public:
	GroupMap();
	virtual ~GroupMap();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 设置鼠标形状
	virtual int OnSetCursor(int x, int y);
	virtual int OnLButtonDown(int x, int y);
	// 鼠标移动
	virtual int OnMouseMove(int x, int y);
	
	virtual IControl* OnGetInWhat(int x, int y);

	// 背景填充
	void SetSolid(bool value);
	bool GetSolid() const;

	//// 是否透明
	//void SetTransparent(bool value);
	//bool GetTransparent() const;

	// 是否产生点击图片事件
	void SetClickEvent(bool value);
	bool GetClickEvent() const;

	// 设置控件形状(1表示矩形，2表示圆形)
	bool SetShape(int shape);
	// 初始化时添加，只添加不删除
	bool AddType(const char* type, const char* pic);
	// 初始化时添加，只添加不删除 增加图片类型
	bool AddTypeEx(const char* type, const char* pic, int pic_type, int w, int h);
	// 添加玩家可视对像的绑定主玩家需特殊处理
	bool AddMainPlayerBind(const char* pic, const PERSISTID& visobj);
	// 添加可视对像的绑定
	bool AddBind(const char* type, const PERSISTID& visobj);

	// 删除可视对像的绑定
	bool DelBind(const PERSISTID& visobj);

	// 清除所有对像的绑定
	bool Clear();
	
	// 控制可视对象的显示
	bool ShowType(int npc_type, bool flag);

	// 设置地图控件
	void SetMapControl(const PERSISTID& visobj);
	PERSISTID GetMapControl() const;

	// 初始化场景大小信息
	bool InitTerrain(float start_x, float start_z, float width, float height);
	// 场景中坐标转换到地图图片的坐标
	bool TransPosToMap(float x, float z, float& map_x, float& map_z);

	// 场景中坐标转换到地图图片的坐标
	void GetPosInMap(const IVarList& params, IVarList& result);

	// 设置玩家的偏移角度
	bool SetPlayerOffsetAngleY(float fAngleY);

protected:
	virtual bool RefreshInnerAbsPos();

	// 判断指定位置是否透明
	bool TestTransparent(int x, int y);
	int MouseMove(int x, int y);

	int GetCustomInt(IEntity* pVis, const char* szProp);

private:
	// 对像类型,不同显示对像不同图片
	struct ElementType
	{
		std::string type;
		std::string pic;
		int width;
		int height;
		PERSISTID pic_id;
		int pic_type;
	};

	// 绑定对像结构,位置根据可视对像的位置更新
	struct BindElement
	{
		size_t type_index;
		PERSISTID visual_obj;
		rect_t rect;
		int bind_type;	// NpcType
	};

	bool m_bSolid;
	//bool m_bTransparent;
	bool m_bClickEvent;
	int m_Shape;

	// 显示图片对像类型,一般不同类型图片不同
	std::vector<ElementType> m_vTypes;
	TStringPod<char, size_t, TStringTraits<char>, TCoreAlloc> m_mapTypes;
	// 对像列表
	std::vector<BindElement> m_vElements;

	// 地图控件 
	PERSISTID m_Control;

	// 场景信息
	float m_fTerrainStartX;
	float m_fTerrainStartZ;
	float m_fTerrainWidth;
	float m_fTerrainHeight;

	// 当前鼠标移入
	PERSISTID m_MouseInObj;

	//
	Label* m_pLblMainPlayer;
	PERSISTID m_MainPlayerID;

	// 玩家偏移角度
	float m_fOffsetAngleY;

	//std::map<int, bool> m_mapShows;
	TPodHashMap<int, bool, TPodTraits<int>, TCoreAlloc> m_mapShows;
};

#endif // _GROUPMAP_H

