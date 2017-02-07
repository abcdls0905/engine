//--------------------------------------------------------------------
// �ļ���:		GroupMap.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��4��23��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
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

// ��ͼ�ڶ�����ʾ���

class GroupMap: public IControl
{
	enum
	{
		ShapeType_Rect = 1,		// ����
		ShapeType_Round = 2,	// Բ��
	};

	enum
	{
		PicType_Glyph = 0,		// ͼƬ
		PicType_Animation = 1,	// ����
		PicType_Max,
	};

public:
	GroupMap();
	virtual ~GroupMap();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// ���������״
	virtual int OnSetCursor(int x, int y);
	virtual int OnLButtonDown(int x, int y);
	// ����ƶ�
	virtual int OnMouseMove(int x, int y);
	
	virtual IControl* OnGetInWhat(int x, int y);

	// �������
	void SetSolid(bool value);
	bool GetSolid() const;

	//// �Ƿ�͸��
	//void SetTransparent(bool value);
	//bool GetTransparent() const;

	// �Ƿ�������ͼƬ�¼�
	void SetClickEvent(bool value);
	bool GetClickEvent() const;

	// ���ÿؼ���״(1��ʾ���Σ�2��ʾԲ��)
	bool SetShape(int shape);
	// ��ʼ��ʱ��ӣ�ֻ��Ӳ�ɾ��
	bool AddType(const char* type, const char* pic);
	// ��ʼ��ʱ��ӣ�ֻ��Ӳ�ɾ�� ����ͼƬ����
	bool AddTypeEx(const char* type, const char* pic, int pic_type, int w, int h);
	// �����ҿ��Ӷ���İ�����������⴦��
	bool AddMainPlayerBind(const char* pic, const PERSISTID& visobj);
	// ��ӿ��Ӷ���İ�
	bool AddBind(const char* type, const PERSISTID& visobj);

	// ɾ�����Ӷ���İ�
	bool DelBind(const PERSISTID& visobj);

	// ������ж���İ�
	bool Clear();
	
	// ���ƿ��Ӷ������ʾ
	bool ShowType(int npc_type, bool flag);

	// ���õ�ͼ�ؼ�
	void SetMapControl(const PERSISTID& visobj);
	PERSISTID GetMapControl() const;

	// ��ʼ��������С��Ϣ
	bool InitTerrain(float start_x, float start_z, float width, float height);
	// ����������ת������ͼͼƬ������
	bool TransPosToMap(float x, float z, float& map_x, float& map_z);

	// ����������ת������ͼͼƬ������
	void GetPosInMap(const IVarList& params, IVarList& result);

	// ������ҵ�ƫ�ƽǶ�
	bool SetPlayerOffsetAngleY(float fAngleY);

protected:
	virtual bool RefreshInnerAbsPos();

	// �ж�ָ��λ���Ƿ�͸��
	bool TestTransparent(int x, int y);
	int MouseMove(int x, int y);

	int GetCustomInt(IEntity* pVis, const char* szProp);

private:
	// ��������,��ͬ��ʾ����ͬͼƬ
	struct ElementType
	{
		std::string type;
		std::string pic;
		int width;
		int height;
		PERSISTID pic_id;
		int pic_type;
	};

	// �󶨶���ṹ,λ�ø��ݿ��Ӷ����λ�ø���
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

	// ��ʾͼƬ��������,һ�㲻ͬ����ͼƬ��ͬ
	std::vector<ElementType> m_vTypes;
	TStringPod<char, size_t, TStringTraits<char>, TCoreAlloc> m_mapTypes;
	// �����б�
	std::vector<BindElement> m_vElements;

	// ��ͼ�ؼ� 
	PERSISTID m_Control;

	// ������Ϣ
	float m_fTerrainStartX;
	float m_fTerrainStartZ;
	float m_fTerrainWidth;
	float m_fTerrainHeight;

	// ��ǰ�������
	PERSISTID m_MouseInObj;

	//
	Label* m_pLblMainPlayer;
	PERSISTID m_MainPlayerID;

	// ���ƫ�ƽǶ�
	float m_fOffsetAngleY;

	//std::map<int, bool> m_mapShows;
	TPodHashMap<int, bool, TPodTraits<int>, TCoreAlloc> m_mapShows;
};

#endif // _GROUPMAP_H

