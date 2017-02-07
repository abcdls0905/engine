//--------------------------------------------------------------------
// �ļ���:		balloon_set.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��3��18��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BALLOON_SET_H
#define _BALLOON_SET_H

#include "i_control.h"
#include "../utils/array_pod.h"

// �����ڳ�����ģ�͵�����ؼ�����

class Balloon;

class BalloonSet: public IControl
{
public:
	BalloonSet();
	virtual ~BalloonSet();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	// ÿִ֡���߼����������ӵ����ж��У�
	virtual void Execute(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual void Paint(float seconds);

	// ��������
	void SetSceneID(const PERSISTID& id);
	PERSISTID GetSceneID() const;

	// �Ƿ���Ҫ����
	void SetSort(bool value);
	bool GetSort() const;

	// �Ƿ�ʹ�����
	void SetUseDepth(bool value);
	bool GetUseDepth() const;

	// �Ƿ�ʹ��Զ������
	void SetUseDepthScale(bool value);
	bool GetUseDepthScale() const;

	// �Ƿ񻺴沢��������
	void SetTempBalloons(bool value);
	bool GetTempBalloons() const;

	// ����������������
	void SetMaxBalloonType(int value);
	int GetMaxBalloonType() const;

	// ������ֵ
	void SetMaxDepthScale(float value);
	float GetMaxDepthScale() const;

	// ��С����ֵ
	void SetMinDepthScale(float value);
	float GetMinDepthScale() const;

	// ������ȣ�>�����ֵ����ʾ
	void SetFadeOutDepth(float value);
	float GetFadeOutDepth() const;

	// �������
	PERSISTID AddBalloon(const PERSISTID& id, const PERSISTID& model, int type);
	// ��������
	PERSISTID CreateBalloon(const char* control_name, 
		const PERSISTID& model, int type);

    // �Ƿ��п��е��Ѵ�������
    bool FindTempBalloon(int type);
    // ����п��е��Ѵ���������ֱ���ã�û�з��ؿ�
    PERSISTID AddTempBalloon(const PERSISTID& model, int type);

	// ɾ������
	bool DeleteBalloon(const PERSISTID& id);
	// ɾ������ָ��ģ�͵�����
	bool DeleteFromModel(const PERSISTID& id);
	// ɾ����������
	bool ClearBalloon();
	// ��������б�
	void GetBalloonList(const IVarList& args, IVarList& result);
	// ��ð���ָ��ģ�͵������б�
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

