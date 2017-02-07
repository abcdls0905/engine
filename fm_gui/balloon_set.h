//--------------------------------------------------------------------
// 文件名:		balloon_set.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月18日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BALLOON_SET_H
#define _BALLOON_SET_H

#include "i_control.h"
#include "../utils/array_pod.h"

// 依附于场景内模型的气球控件集合

class Balloon;

class BalloonSet: public IControl
{
public:
	BalloonSet();
	virtual ~BalloonSet();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	// 每帧执行逻辑（如果已添加到运行队列）
	virtual void Execute(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual void Paint(float seconds);

	// 场景对象
	void SetSceneID(const PERSISTID& id);
	PERSISTID GetSceneID() const;

	// 是否需要排序
	void SetSort(bool value);
	bool GetSort() const;

	// 是否使用深度
	void SetUseDepth(bool value);
	bool GetUseDepth() const;

	// 是否使用远近放缩
	void SetUseDepthScale(bool value);
	bool GetUseDepthScale() const;

	// 是否缓存并复用汽球
	void SetTempBalloons(bool value);
	bool GetTempBalloons() const;

	// 最大的汽球类型数量
	void SetMaxBalloonType(int value);
	int GetMaxBalloonType() const;

	// 最大放缩值
	void SetMaxDepthScale(float value);
	float GetMaxDepthScale() const;

	// 最小放缩值
	void SetMinDepthScale(float value);
	float GetMinDepthScale() const;

	// 淡出深度，>此深度值不显示
	void SetFadeOutDepth(float value);
	float GetFadeOutDepth() const;

	// 添加气球
	PERSISTID AddBalloon(const PERSISTID& id, const PERSISTID& model, int type);
	// 创建气球
	PERSISTID CreateBalloon(const char* control_name, 
		const PERSISTID& model, int type);

    // 是否有空闲的已创建汽球
    bool FindTempBalloon(int type);
    // 如果有空闲的已创建汽球，则直接用，没有返回空
    PERSISTID AddTempBalloon(const PERSISTID& model, int type);

	// 删除气球
	bool DeleteBalloon(const PERSISTID& id);
	// 删除绑定于指定模型的气球
	bool DeleteFromModel(const PERSISTID& id);
	// 删除所有气球
	bool ClearBalloon();
	// 获得气球列表
	void GetBalloonList(const IVarList& args, IVarList& result);
	// 获得绑定于指定模型的气球列表
	void GetModelBalloonList(const IVarList& args, IVarList& result);
	
private:
    bool TryAddTempBalloon(Balloon* pBalloon);
	void ReleaseAll();
    void ReleaseAllTemp();
	void CheckAll();
	void CheckOrder();
	Balloon* NewBalloon(IControl* pControl, const PERSISTID& model, const int type = -1);
	bool PaintBalloon(Balloon* pBalloon, float seconds, float win_width, 
		float win_height, const FmMat4& mtxView, const FmMat4& mtxViewProj, 
		const FmVec3& cam_pos);

private:
	Balloon* m_pFirst;
	Balloon* m_pLast;
	PERSISTID m_SceneID;
	bool m_bSort;
	bool m_bUseDepth;
    bool m_bUseDepthScale;
    float m_fMaxDepthScale;
    float m_fMinDepthScale;
    float m_fFadeOutDepth;

    bool m_bTempBalloons;
    int m_nMaxBalloonType;
    TArrayPod<Balloon*, 8, TCoreAlloc> m_vTempTypeArray;
};

#endif // _BALLOONSET_H

