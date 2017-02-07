//--------------------------------------------------------------------
// �ļ���:		Path.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��7��4��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __PATH_H__
#define __PATH_H__

#include "../visual/i_visual.h"
#include <vector>

// ·��

class Path: public IVisual
{
private:
	// ·����ֵ��ʽ
	enum InterpolationMode
	{
		InterpolationModeLinear = 0,	// ���Բ�ֵ
		InterpolationModeCurve,			// ���߲�ֵ��Ŀǰ�õ���Catmull-Rom
	};
	// ����
	enum OrientationMode
	{
		OrientationModeNull = 0,		// ���޸ĳ���
		OrientationModeTarget,			// ����Ŀ�����
		OrientationModeForward,			// ����ǰ������
	};

	struct path_vertex_t 
	{
		FmVec3 pos;
		unsigned int color;
	};

	struct key_point_t
	{
		key_point_t() { time = 0.0f; }
		FmVec3 pos;
		float time;						// �ؼ�֡ʱ�䣬��λ����
	};

	enum { MAX_VERTEX_NUM = 8000 };

public:
	Path();
	virtual ~Path() {}

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize(float offset_seconds);

	// ����һ���ؼ���
	size_t AddKeyPoint(float x, float y, float z, float time);
	// ɾ��һ���ؼ���
	bool DelKeyPoint(size_t index);
	// �޸Ĺؼ��������
	bool SetKeyPointPosition(size_t index, float x, float y, float z);
	// ���ùؼ���Ĺؼ�֡ʱ��
	bool SetKeyPointTime(size_t index, float time);

	// ��ֵ�ܶȣ�����ֵ��ļ��
	void SetPointDistance(float value);
	float GetPointDistance() const { return m_fPointDistance; }

	// ��ֵ��ʽ
	void SetInterpolationModeString(const char* value);
	std::string GetInterpolationModeString() const;

	// ·�����ƵĶ���
	void SetBindObject(const PERSISTID& value);
	PERSISTID GetBindObject() const;

	// ����Ķ���
	void SetTargetObject(const PERSISTID& value);
	PERSISTID GetTargetObject() const;

	// ��ǰʱ��
	void SetCurTime(float value);
	float GetCurTime() const;

	// ��ʱ��
	void SetTotalTime(float value);
	float GetTotalTime() const;

	// �Ƿ���ͣ
	void SetPause(bool value);
	bool GetPause() const;

private:
	// ��ֵ��ʽ
	void InnerSetInterpolationMode(int value);
	// �������ɲ�ֵ��
	bool RebuildInterpolationPoint();
	// �ѵ���ӵ���������ͨ�ú���
	void InnerAddPoint(const FmVec3& pos, unsigned int uColor,
		std::vector<path_vertex_t>& v);
	// �ѵ���������ӵ���������ͨ�ú���
	void InnerAddPointCube(const FmVec3& pos, unsigned int uColor,
		float fCubeRadius, std::vector<path_vertex_t>& v);
	// ���°󶨶���
	void InnerUpdateObject();
	IVisBase* GetVisBase(const PERSISTID& id) const;

	// ���ݳ������Ƕ�
	FmVec3* GetFaceAngle(FmVec3* pOut, FmVec3* pEye, FmVec3* pAt);

private:
	// �ؼ���
	std::vector<key_point_t> m_keyPoints;

	// ���е㣬�����ؼ���Ͳ�ֵ��
	std::vector<path_vertex_t> m_points;
	// ��
	std::vector<path_vertex_t> m_lines;
	// �ؼ����С������ģ��
	std::vector<path_vertex_t> m_keyPointCube;

	// ��ֵ��ʽ
	int m_nInterpolationMode;
	// ��ֵ�ܶȣ�����ֵ��ļ��
	float m_fPointDistance;

	// ����ʽ
	int m_nOrientationMode;
	// ����Ķ���
	PERSISTID m_targetObject;

	FmMat4 m_matWorld;
	unsigned int m_uLineColor;
	unsigned int m_uPointColor;
	unsigned int m_uKeyPointCubeColor;

	// ·�����ƵĶ���
	PERSISTID m_bindObject;
	// ��ǰʱ��Ĺؼ���
	int m_nCurKeyPoint;
	// ��ǰʱ��
	float m_fCurTime;
	// ��ʱ��
	float m_fTotalTime;
	// �Ƿ���ͣ
	bool m_bPause;
};

#endif // __PATH_H__
