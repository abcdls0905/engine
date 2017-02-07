//--------------------------------------------------------------------
// �ļ���:		i_whip_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2014��4��11��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _I_WHIP_MANAGER_H
#define _I_WHIP_MANAGER_H

#include "../visual/i_vis_base.h"

// ���ӹ�����

enum WhipType
{
    WHIPTYPE_INVALID            = 0,
    WHIPTYPE_ROPE               = 1,    // ����
    WHIPTYPE_SIN_WHIP           = 2,    // SIN���ߵı���
    WHIPTYPE_CIRCLE_WHIP        = 3,    // Բ�εı���
    WHIPTYPE_SIN_WHIP_2         = 4,    // SIN���ߵı���2
    WHIPTYPE_CIRCLE_WHIP_2      = 5,    // Բ�εı���2
    WHIPTYPE_CIRCLE_SABER       = 6,    // Բ�ι켣�ĵ���
    WHIPTYPE_SIMI_CIRCLE_SABER  = 7,    // ��Բ�ι켣�ĵ���
};

class IWhipManager: public IVisBase
{
public:
	// �˶���ͼ�ļ�·��
	virtual void SetTexturePath(const char* value) = 0;
	virtual const char* GetTexturePath() const = 0;

    // ����ģ����Ϣ������β
    virtual PERSISTID CreateFromModel(
        WhipType type, 
        const FmVec3& vLeft,            // ���ӵ��нǷ���,�������˶�ƽ��
        const char* szTexName,          // ��ͼ
        float inTime,                   // ����ʱ��
        float pauseTime,                // ��ͣʱ��
        float outTime,                  // �˳�ʱ��
        unsigned int colorStart,        // ��ʼ��ɫ
        unsigned int colorEnd,          // ������ɫ
        bool highlight,                 // �Ƿ����
        float bladeWidth,               // ���
        FmVec4 params,                  // ��������(params.x���Ʊ��ӻ���,params.y���ƽǶ�,params.w����ʱ������)
        const PERSISTID& attack_id, const char* attack_label, 
        const PERSISTID& suffer_id, const char* suffer_label) = 0; 

	// ���������ļ�������β
    virtual PERSISTID CreateFromIni(
        const FmVec3& vLeft,            // ���ӵ��нǷ���,�������˶�ƽ��
        const PERSISTID& attack_id, const char* attack_label, 
        const PERSISTID& suffer_id, const char* suffer_label,
		const char* ini_name, const char* arc_name) = 0;

	// ɾ������
	virtual bool Delete(const PERSISTID& id) = 0;
};

#endif // _I_WHIP_MANAGER_H
