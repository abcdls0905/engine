//--------------------------------------------------------------------
// �ļ���:		whip_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2014��4��11��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WHIP_MANAGER_H
#define _WHIP_MANAGER_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_render.h"
#include "../visual/shader_list.h"
#include "../visual/i_whip_manager.h"

// ���ӹ�����

#define MAX_WHIP_COUNT 32

class Whip;

class TWhipShaderAlloc
{
public:
    TWhipShaderAlloc() {}
    ~TWhipShaderAlloc() {}

    // �����ڴ�
    void* Alloc(size_t size) { return CORE_ALLOC(size); }
    // �ͷ��ڴ�
    void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
    // ����
    void Swap(TWhipShaderAlloc& src) {}
};

struct whip_filter_shaderhandle_t
{
    IShaderProgram* pShader;
    fm_int hMtxWVP;
    fm_int hMtxWorld; 
    fm_int hEyePos; 
    fm_int hColorStart;  
    fm_int hColorEnd;    
    fm_int hParams;

    fm_int hFogParam;
    fm_int hFogColor;
    fm_int hFogExpParam;
    fm_int hTex;
};

class WhipManager: public IWhipManager
{
public:
	WhipManager();
	virtual ~WhipManager();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// ���ó���������
	virtual void SetContext(IRenderContext* value);

	virtual void Realize();

	// �˶���ͼ�ļ�·��
	void SetTexturePath(const char* value);
	const char* GetTexturePath() const;

	// ����ģ����Ϣ������β
	PERSISTID CreateFromModel(
        WhipType type, 
        const FmVec3& vLeft,
        const char* szTexName,      // ��ͼ
        float inTime,               // ����ʱ��
        float pauseTime,            // ��ͣʱ��
        float outTime,              // �˳�ʱ��
        unsigned int colorStart,    // ��ʼ��ɫ
        unsigned int colorEnd,      // ������ɫ
        bool highlight,             // �Ƿ����
        float bladeWidth,           // ���
        FmVec4 params,              // ��������
        const PERSISTID& attack_id, const char* attack_label, 
        const PERSISTID& suffer_id, const char* suffer_label); 

	// ���������ļ�������β
    PERSISTID CreateFromIni(const FmVec3& vLeft,
        const PERSISTID& attack_id, const char* attack_label, 
        const PERSISTID& suffer_id, const char* suffer_label,
		const char* ini_name, const char* arc_name);

	// ɾ����β
	bool Delete(const PERSISTID& id);

    whip_filter_shaderhandle_t* GetShaderHander(unsigned int flags);

private:
    bool LoadShader();

    int GetEmptyWhip();

private:
    IRender* m_pRender;
	PERSISTID m_Whips[MAX_WHIP_COUNT];
	bool m_bAsyncLoad;
	core_string m_strTexturePath;

    TShaderList<IVertexShader, 4> m_VSList;
    TShaderList<IPixelShader, 4> m_PSList;

    TPodHashMap<unsigned int, whip_filter_shaderhandle_t*, TPodTraits<unsigned int>,TWhipShaderAlloc> m_ShaderMap;
};

#endif // _WHIP_MANAGER_H
