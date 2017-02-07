//--------------------------------------------------------------------
// �ļ���:		KeyFrameController.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��8��6��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __KEY_FRAME_CONTROLLER_H__
#define __KEY_FRAME_CONTROLLER_H__

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"
#include "../utils/array_pod.h"
#include "../math/fm_vector.h"
#include "../extern/rapidxml/rapidxml.hpp"

class ITimeAxis;

// ֡����������
enum eKeyControllerType
{
	// ����Ƕȿ�����
	CONTROLLER_TYPE_POS_X = 0,
	CONTROLLER_TYPE_POS_Y = 1,
	CONTROLLER_TYPE_POS_Z = 2,
	CONTROLLER_TYPE_ANGLE_X = 3,
	CONTROLLER_TYPE_ANGLE_Y = 4,
	CONTROLLER_TYPE_ANGLE_Z = 5,
	CONTROLLER_TYPE_FOV_X = 6,

	// ����ؿ�����
	CONTROLLER_TYPE_WOBBLE_X = 50,
	CONTROLLER_TYPE_WOBBLE_Y = 51,
	CONTROLLER_TYPE_WOBBLE_Z = 52,
	CONTROLLER_TYPE_WOBBLE_F = 53,
	CONTROLLER_TYPE_WOBBLE_RX = 54,
	CONTROLLER_TYPE_WOBBLE_RY = 55,
	CONTROLLER_TYPE_WOBBLE_RZ = 56,

	// ����Ƕ���Ϣid
	CONTROLLER_TYPE_POSITION_ANGLE = 100,


};

// ������ʹ�÷�ʽ
enum eControllerUsageMode
{
	CONTROLLER_USAGE_MODE_BEZIER = 0,		// ����������
	CONTROLLER_USAGE_MODE_ONCE,				// ÿ���ؼ�ִֻ֡��һ��				
	CONTROLLER_USAGE_MODE_LINEAE,			// ���Բ�ֵ�����ñ���������ʵ�ֵ����Ը���
	CONTROLLER_USAGE_MODE_SCALE,			// �����ؼ�֡

};

enum eKeyChangeType
{
	CHANGE_ADD_KEY = 1,
	CHANGE_DLETEKEY,
	CHANGE_COPY,
	CHANGE_MOVE_KEY,
	CHANGE_MOVE_ALL_KEY,
	
	// BEZIER
	CHANGE_MOVE_ALL_VALUE,
	CHANGE_COMPUTE_CURVE,
	CHANGE_ADD_BEZIER_KEY,
};
// BEZIER,���ɿ��Ƶ������

//SHOW_CURVE1
/*
1 ���ڻ������������,�������ˮƽ
2 ����β����,�������ڵ����ߵ�ƽ������Ϊ������
3 ���Ƶ㳬��ǰ���,��ȡ���ڵ��ֵ
4 �˵�ˮƽ
*/

//SHOW_CURVE2
/*
1 ����β����,�������ڵ����ߵ�ƽ������Ϊ������
2 ������б�������ڵ��������Ա仯
3 �˵������ڵ�Ƚ�
*/
enum
{
	SHOW_DOC,     // ���ⲿ��õ�����ģʽ
	SHOW_CURVE1,  // �Ƚ���ʵ
	SHOW_CURVE2,  // �Ƚ�ƽ��
	SHOW_LINE,    // ֱ��ģʽ
	SHOW_EDITE1,  // �༭ģʽ
	SHOW_EDITE2,  // �༭ģʽ

	SHOW_COUNT,
};

struct sKey
{

	virtual  const sKey* Copy(const sKey* pData);

	float	time;
	
};

struct Vec2
{
    float x;
    float y;
};

// Bezier
struct sKeyBezier : public sKey
{
	sKeyBezier();
	virtual  const sKey* Copy(const sKey* pData);
	FmVec2 GetSelfPoint() const { return FmVec2(time, value);}

	float	value;

	union
	{
		struct
		{
			// ǰһ�����Ƶ�
			float	left_time;
			float	left_value;

			// ��һ�����Ƶ�
			float	right_time;
			float	right_value;
		};

		struct
		{
			Vec2 left;
			Vec2 right;
		};

	};

	
	// �����ߵ�б��
	float       kl;
	float       kr;
	// �����ߵĳ��ȱ�
	float       sl;
	float       sr;
	// �����ߵ�ģʽ
	char        mode;

	// ͻ��
	//bool	bStep;
};

// Once
struct sKeyOnce : public sKey
{
	virtual  const sKey* Copy(const sKey* pData);

	// �ַ�������
	result_string str_param;			
};

// Line
struct sKeyLine : public sKey
{
	virtual  const sKey* Copy(const sKey* pData);

	// �ؼ�֡��ֵ
	float value;					
};

// Scale
struct sKeyScale : public sKey
{
	virtual  const sKey* Copy(const sKey* pData);

	result_string str_param;		// �ַ�������
	float time_length;				// �����ؼ�֡ʱ��
	float speed;					// �����ؼ�֡�����ٶ�
};


// �ؼ�֡������
struct sObjectData;
class TiXmlElement;
class KeyFrameController
{

	typedef TArrayPod<sKey *, 1, TCoreAlloc> Keys;
	
public:
	static void SetTimeAxis(ITimeAxis *pEntiy);
	
	KeyFrameController();
	KeyFrameController(int controller_type, int usage_mode, sObjectData* pOwner);
	~KeyFrameController();
	KeyFrameController(const KeyFrameController& right);
	KeyFrameController& operator=(const KeyFrameController& right);

	// ֡����������
	int GetControllerType() const { return m_nControllerType; }

	// ������ʹ�÷�ʽ
	int GetUsageMode() const { return m_nUsageMode; }

	float GetBegin() const;
	float GetEnd() const;

	void AddBezierKey(float time, float value);
	int GetFrontBezierKey(float time, float &value) const;
	void GetAdjacencyBezierKey(float time, IVarList& result) const;
	const sKey* GetKeyFrameByTime(float time, int* index) const;

	// ���ùؼ�֡��ֵ
	// �����û������ؼ�֡���Զ����
	bool SetKeyFrameBezierMode(float time, float value, 
		float left_time, float left_value, 
		float right_time, float right_value, bool bCallback = true);
	bool SetKeyFrameLinearMode(float time, float value, bool bCallback = true);
	bool SetKeyFrameOnceMode(float time, const char* param, bool bCallback = true);
	bool SetKeyFrameScaleMode(float time, const char* action_name, 
		float time_length, float speed, bool bCallback = true);

	// ɾ���ؼ�֡
	void DeleteControllerKey(float time);
	// ��չؼ�֡
	void ClearKeyFrames();
	// �ؼ�֡����
	size_t GetKeyCount() const { return m_keyFrames.size(); }

	bool GetKeyInfo(IVisBase* pObject, float time, IVarList &result);

	// ���ָ��֡��ֵ
	float GetValueBezierMode(float time) const;
	float GetValueLinearMode(float time) const;
	const char* GetValueOnceMode(float time) const;
	bool GetValueScaleMode(float time, core_string& action_name, 
		float& action_time, float& speed);
	// ��ȡ��ĳһ�ؼ�֡������
	void GetKeyFrameValue(float time, IVarList& result) const;

	// ���ؿ�����
	bool LoadController(rapidxml::xml_node<>* xml_controller);
	// ���������
	bool SaveController(rapidxml::xml_node<>* xml_controller);

	// ��ùؼ�֡�б�
	void GetControllerKeyTime(IVarList& result) const;
	int GetKeyFrameListBezierMode(IVarList& result) const;
	int GetKeyFrameListOnceMode(IVarList& result) const;
	int GetKeyFrameListLinearMode(IVarList& result) const;
	int GetKeyFrameListScaleMode(IVarList& result) const;


	// �������йؼ�������ҿ��Ƶ�
	int CreateAllControlPoints();
	// �ı���Ƶ�ļ��㷽ʽ
	void ChangeComputeTypeKey(float time, int type);
	// �ƶ��ؼ�֡
	void MoveAllKeyTime(float time_add);
	// ƽ�ƹؼ�֡��ֵ
	void MoveAllKeyValue(float value_add);
	// ���ƹؼ�֡
	void CopyKey(float time, float new_time);
	// �ƶ�ĳ���ؼ�֡
	void MoveKey(float time, float new_time);

	// �ռ�λ�ƹؼ�֡�Ĺؼ�֡ʱ��
	void CollectPosKeyTime(TArrayPod<float, 1> &array) const;

	// ��ȡ֡
	sKeyBezier GetBezierKey(size_t index) const;

	// ����֡
	bool SetBezierKey(size_t index, const sKeyBezier& key);
private:
	// ����ĳʱ�̵Ĺؼ�֡, ���ؿ��Բ����λ��.
	sKey* FindKeyFrame(float fTime, int* pIndex = NULL) const;
	sKey* AddKey(float fTime, int nIndex = -1);
	void  DeleteKey(sKey *p);

	// ���㱴������ֵ
	float GetBezierValue(float x, float c0x, float c0y, float c1x, float c1y, 
		float c2x, float c2y, float c3x, float c3y) const;


	// ����CONTROLLER_USAGE_MODE_BEZIER������
	bool InnerLoadBezierController(rapidxml::xml_node<>* xml_controller);
	// ����CONTROLLER_USAGE_MODE_LINEAE������
	bool InnerLoadLinearController(rapidxml::xml_node<>* xml_controller);
	// ����CONTROLLER_USAGE_MODE_ONCE������
	bool InnerLoadOnceController(rapidxml::xml_node<>* xml_controller);
	// ����CONTROLLER_USAGE_MODE_SCALE������
	bool InnerLoadScaleController(rapidxml::xml_node<>* xml_controller);

	// ����CONTROLLER_USAGE_MODE_BEZIER������
	bool InnerSaveBezierController(rapidxml::xml_node<>* xml_controller);
	// ����CONTROLLER_USAGE_MODE_LINEAE������
	bool InnerSaveLinearController(rapidxml::xml_node<>* xml_controller);
	// ����CONTROLLER_USAGE_MODE_ONCE������
	bool InnerSaveOnceController(rapidxml::xml_node<>* xml_controller);
	// ����CONTROLLER_USAGE_MODE_SCALE������
	bool InnerSaveScaleController(rapidxml::xml_node<>* xml_controller);

private:

	// ����ĳ�ؼ���Ŀ��Ƶ�
	int CreateOneControlPoint(int index, bool send_msg = true);

	// ����ǰ��ؼ���,�����Լ������ҿ��Ƶ�
	int ComputLeftAndRightPoint(sKeyBezier &front, sKeyBezier &self, sKeyBezier &next);
	// ��ǰһ���ؼ���Ŀ��Ƶ�,����һ���ؼ���Ŀ��Ƶ�,(�������Ƶ�������ؼ����д��߶Գ�)
	void GetSymmetryPoint(FmVec2 &out, FmVec2 &self, FmVec2 &other, FmVec2 &other_control);

	// ���͸���ĳ���ؼ�֡����Ϣ
	void SendUpdateBezierKey(int nIndex);

private:
	static ITimeAxis* s_pTimeAxis;
	sObjectData*	m_pObject;

	int			m_nControllerType;
	int			m_nUsageMode;

	Keys		m_keyFrames;
	float		m_fLastTime;

	// �Ƿ��Ѿ�������,�Ҷ˵��ʱ������
	bool		m_bFirstFrame;
	bool		m_bLastFrame;
};

#endif // __KEY_FRAME_CONTROLLER_H__
