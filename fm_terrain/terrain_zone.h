//--------------------------------------------------------------------
// �ļ���:		terrain_zone.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_ZONE_H
#define _TERRAIN_ZONE_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../public/i_interface.h"
#include "../visual/i_res_base.h"
#include "../visual/i_res_loader.h"
#include "terrain_chunk.h"
// ��������������Ҫ��ͷ�ļ�
//#include "../physics/physics_macros.h"
//#include <xmmintrin.h>

inline int FloatToInt(float val)
{
	//return _mm_cvtt_ss2si(_mm_load_ss(&val));
	return int(val);
}

// �������򣨰���һ��ֿ飩

class Terrain;
class CQuadNode;
class CTerrainFactory;
class CTerrainChunk;
class CTerrainHeight;
class CTerrainLight;
class CTerrainVisuals;
class CTerrainWalkable;
class CTerrainRegion;
#ifdef PHYSICS_HAVOK_TERRAIN
class CTerrainRigid;
#endif // PHYSICS_HAVOK_TERRAIN
class CHeightLoader;
class CLightLoader;
class CAlphaLoader;
class CVisualLoader;
class CWalkLoader;
class CRegionLoader;
#ifdef PHYSICS_HAVOK_TERRAIN
class CRigidLoader;
#endif // PHYSICS_HAVOK_TERRAIN

class CTerrainZone: public IResBase
{
public:
	// ������Դ����
	enum
	{
		RES_CATEGORY_HEIGHT,	// ����߶���Ϣ
		RES_CATEGORY_LIGHT,		// ������Ϣ
		RES_CATEGORY_ALPHA,		// ��ͼ�����Ϣ
		RES_CATEGORY_VISUAL,	// ����������Ϣ
		RES_CATEGORY_WALK,		// ������Ϣ
		RES_CATEGORY_REGION,	// �����Ϣ
#ifdef PHYSICS_HAVOK_TERRAIN
		RES_CATEGORY_RIGID,		// ���������Ϣ
#endif // PHYSICS_HAVOK_TERRAIN
	};

public:
	CTerrainZone(Terrain* pTerrain, CTerrainFactory* pFactory,
		size_t index, size_t row, size_t col);
	~CTerrainZone();

	// �ͷ�
	virtual void Release();
	
	Terrain* GetTerrain() const { return m_pTerrain; }

	// ָ���Ӧ���Ĳ����ڵ�
	void SetQuadNode(CQuadNode* value) { m_pQuadNode = value; }
	CQuadNode* GetQuadNode() const { return m_pQuadNode; }

	// �����
	PERSISTID GetID() const { return m_PersistID; }
	void SetID(const PERSISTID& value) { m_PersistID = value; }

	// ��������
	void SetName(const char* value)
	{ 
		Assert(value != NULL);

		m_strName = value; 
	}

	// �������
	const char* GetName() const
	{
		return m_strName.c_str();
	}

	// ����
	size_t GetIndex() const { return m_nIndex; }
	// ��
	size_t GetRow() const { return m_nRow; }
	// ��
	size_t GetCol() const { return m_nCol; }
	
	// ����߶�
	CTerrainHeight* GetHeight() const { return m_pHeight; }
	
	// ����
	CTerrainLight* GetLight() const { return m_pLight; }

	// ��������
	CTerrainVisuals* GetVisuals() const { return m_pVisuals; }
	// ��������Ϣ
	CTerrainWalkable* GetWalkable() const { return m_pWalkable; }
	// �����Ϣ
	CTerrainRegion* GetRegion() const { return m_pRegion; }
#ifdef PHYSICS_HAVOK_TERRAIN
	// ���������Ϣ
	CTerrainRigid* GetRigid() const { return m_pRigid; };
#endif // PHYSICS_HAVOK_TERRAIN
	
	// ��ʼ��
	bool Init();
	
	// �����������еĸ�����ʼ����
	size_t GetGlobalRow() const { return m_nGlobalRow; }
	size_t GetGlobalCol() const { return m_nGlobalCol; }
	
	// �����������е�ALPHAֵ��ʼ����
	size_t GetAlphaRow() const { return m_nAlphaRow; }
	size_t GetAlphaCol() const { return m_nAlphaCol; }

	// ��ʼ����
	float GetLeft() const { return m_fLeft; }
	float GetTop() const { return m_fTop; }
	float GetRight() const { return m_fRight; }
	float GetBottom() const { return m_fBottom; }

	// ����ʾ��Ĭʱ��
	void SetSilenceTime(float value) { m_fSilenceTime = 0.0F; }
	float GetSilenceTime() const { return m_fSilenceTime; }
	float IncSilenceTime(float d) { m_fSilenceTime += d; return m_fSilenceTime; }
	
	// ��÷ֿ�����
	CTerrainChunk* GetChunks() { return m_pChunks; }
	size_t GetChunkCount() const { return m_nChunkNum; }
	size_t GetChunkDims() const { return m_nChunkDims; }

	// ���ָ�������ֿ�
	CTerrainChunk* GetChunkByIndex(size_t index)
	{
		Assert(index < m_nChunkNum);

		return m_pChunks + index;
	}
	
	// ���ָ��λ�÷ֿ�
	CTerrainChunk* GetChunk(size_t row, size_t col)
	{
		Assert(row < m_nChunkDims);
		Assert(col < m_nChunkDims);

		return m_pChunks + row * m_nChunkDims + col;
	}
	
	// ������ڵķֿ�
	CTerrainChunk* GetLeftChunk(CTerrainChunk* chunk)
	{
		Assert(chunk != NULL);

		int col = chunk->GetCol();

		if ((col - 1) >= 0)
		{
			return m_pChunks + chunk->GetIndex() - 1;
		}

		return NULL;
	}

	CTerrainChunk* GetRightChunk(CTerrainChunk* chunk)
	{
		Assert(chunk != NULL);

		size_t col = chunk->GetCol();

		if ((col + 1) < m_nChunkDims)
		{
			return m_pChunks + (chunk->GetIndex() + 1);
		}

		return NULL;
	}

	CTerrainChunk* GetUpChunk(CTerrainChunk* chunk)
	{
		Assert(chunk != NULL);

		int row = chunk->GetRow();

		if ((row - 1) >= 0)
		{
			return m_pChunks + (chunk->GetIndex() - m_nChunkDims);
		}

		return NULL;
	}

	CTerrainChunk* GetDownChunk(CTerrainChunk* chunk)
	{
		Assert(chunk != NULL);

		size_t row = chunk->GetRow();

		if ((row + 1) < m_nChunkDims)
		{
			return m_pChunks + (chunk->GetIndex() + m_nChunkDims);
		}

		return NULL;
	}

	// ��ñ�Ե�ķֿ�
	CTerrainChunk* GetLeftEdgeChunk(size_t row)
	{
		Assert(row < m_nChunkDims);

		return m_pChunks + row * m_nChunkDims;
	}

	CTerrainChunk* GetRightEdgeChunk(size_t row)
	{
		Assert(row < m_nChunkDims);

		return m_pChunks + (row * m_nChunkDims + m_nChunkDims - 1);
	}

	CTerrainChunk* GetUpEdgeChunk(size_t col)
	{
		Assert(col < m_nChunkDims);

		return m_pChunks + col;
	}

	CTerrainChunk* GetDownEdgeChunk(size_t col)
	{
		Assert(col < m_nChunkDims);

		return m_pChunks + ((m_nChunkDims - 1) * m_nChunkDims + col);
	}

	// �������ڷֿ�
	void UpdateChunkSibling();

	// ָ��λ�������ĸ��ֿ�
	CTerrainChunk* GetInChunk(float x, float z)
	{
		int chunk_r = FloatToInt((z - m_fTop) * m_fInvChunkSize);
		int chunk_c = FloatToInt((x - m_fLeft) * m_fInvChunkSize);
		size_t chunk_dims = m_nChunkDims;

		if ((size_t(chunk_r) >= chunk_dims) || (size_t(chunk_c) >= chunk_dims))
		{
			return NULL;
		}

		return &m_pChunks[chunk_r * chunk_dims + chunk_c];
	}

	// �����ֿ����ײ��Ϣ
	bool CreateChunkCollide(size_t index);
	// �ͷŷֿ����ײ��Ϣ
	bool ReleaseChunkCollide(size_t index);

	// �����ײ����
	size_t GetCollideNum() const { return m_nCollideNum; }
	size_t GetCollideDims() const { return m_nCollideDims; }
	CQuadGround* GetCollides() { return m_pCollides; }

	// �ͷ����пɼ����弰����ײ��Ϣ
	void ReleaseVisualObjects();
	// �ͷ����зֿ����ײ��Ϣ
	void ReleaseChunkObjects();
	// �������зֿ�Ķ��㻺��
	void ReleaseChunkVB();
	// �ͷ����зֿ��AlphaMap
	void ReleaseChunkAlphaMap();
	// �ͷ����зֿ��AlphaMapռ�õ���ͼ��Դ
	void ReleaseChunkAlphaTex();
	// �ͷ����зֿ�Ļ��ݶ��㻺��
	void ReleaseChunkGrassVB();
	// �ͷ����м�����
	void ReleaseAllLoader();
	
	// ������зֿ��AlphaMap����
	size_t GetChunkAlphaMapCount();

	// �������зֿ鲻�ɼ�
	void SetChunkInvisible();
	
	// ͬ������
	bool CreateHeight();
	// ��ʼ�첽���� 
	bool BeginCreateHeight();
	
	// ͬ������
	bool CreateLight();
	// ��ʼ�첽���� 
	bool BeginCreateLight();

	// ͬ������
	bool CreateAlpha();
	// ��ʼ�첽���� 
	bool BeginCreateAlpha();

	// ͬ������
	bool CreateVisual();
	// ��ʼ�첽���� 
	bool BeginCreateVisual();
	
	// ͬ������
	bool CreateWalk();
	// ��ʼ�첽���� 
	bool BeginCreateWalk();

	// ͬ������
	bool CreateRegion();
	// ��ʼ�첽���� 
	bool BeginCreateRegion();

#ifdef PHYSICS_HAVOK_TERRAIN
	// ͬ������
	bool CreateRigid();
	// ��ʼ�첽���� 
	bool BeginCreateRigid();
#endif // PHYSICS_HAVOK_TERRAIN

	// �����첽���� 
	bool EndCreate(int category, bool succeed);

	// ����첽������
	IResLoader* GetHeightLoader();
	IResLoader* GetLightLoader();
	IResLoader* GetAlphaLoader();
	IResLoader* GetVisualLoader();
	IResLoader* GetWalkLoader();
	IResLoader* GetRegionLoader();
#ifdef PHYSICS_HAVOK_TERRAIN
	IResLoader* GetRigidLoader();
#endif // PHYSICS_HAVOK_TERRAIN

	// ������ɱ�־
	void SetHeightLoaded(bool value) { m_bHeightLoaded = value; }
	bool GetHeightLoaded() const { return m_bHeightLoaded; }
	void SetLightLoaded(bool value) { m_bLightLoaded = value; }
	bool GetLightLoaded() const { return m_bLightLoaded; }
	void SetAlphaLoaded(bool value) { m_bAlphaLoaded = value; }
	bool GetAlphaLoaded() const { return m_bAlphaLoaded; }
	void SetVisualLoaded(bool value) { m_bVisualLoaded = value; }
	bool GetVisualLoaded() const { return m_bVisualLoaded; }
	void SetWalkLoaded(bool value) { m_bWalkLoaded = value; }
	bool GetWalkLoaded() const { return m_bWalkLoaded; }
	void SetRegionLoaded(bool value) { m_bRegionLoaded = value; }
	bool GetRegionLoaded() const { return m_bRegionLoaded; }
#ifdef PHYSICS_HAVOK_TERRAIN
	void SetRigidLoaded(bool value) { m_bRigidLoaded = value; }
	bool GetRigidLoaded() const { return m_bRigidLoaded; }
#endif // PHYSICS_HAVOK_TERRAIN
	bool GetCollideLoaded() const { return m_nLoadCollideNum == m_nChunkNum; }
	
	// ������Դ�������
	bool GetAllLoaded() const
	{
#ifdef PHYSICS_HAVOK_TERRAIN
		bool rigid_loaded = true;

		if (m_pRender->GetSupportPhysics())
		{
			rigid_loaded = m_bRigidLoaded;
		}
#endif // PHYSICS_HAVOK_TERRAIN

		//return m_bHeightLoaded && m_bLightLoaded && m_bAlphaLoaded 
		return m_bHeightLoaded && m_bAlphaLoaded 
			&& m_bVisualLoaded && m_bWalkLoaded	&& m_bRegionLoaded
#ifdef PHYSICS_HAVOK_TERRAIN
			&& rigid_loaded
#endif // PHYSICS_HAVOK_TERRAIN
			&& (m_nLoadCollideNum == m_nChunkNum);
	}

	// �����ɼ��صı���
	float GetLoadPercent() const;

	// �����Ƿ񱻱༭���ı�
	void SetHeightChanged(bool value) { m_bHeightChanged = value; }
	bool GetHeightChanged() const { return m_bHeightChanged; }
	void SetAlphaChanged(bool value) { m_bAlphaChanged = value; }
	bool GetAlphaChanged() const { return m_bAlphaChanged; }
	void SetLightChanged(bool value) { m_bLightChanged = value; }
	bool GetLightChanged() const { return m_bLightChanged; }
	void SetVisualChanged(bool value) { m_bVisualChanged = value; }
	bool GetVisualChanged() const { return m_bVisualChanged; }
	void SetWidgetChanged(bool value) { m_bWidgetChanged = value; }
	bool GetWidgetChanged() const { return m_bWidgetChanged; }
	void SetHelperModelChanged(bool value) { m_bHelperModelChanged = value; }
	bool GetHelperModelChanged() const { return m_bHelperModelChanged; }
	void SetCollideInfoChanged(bool value) { m_bCollideInfoChanged = value; }
	bool GetCollideInfoChanged() const { return m_bCollideInfoChanged; }
	void SetDescChanged(bool value) { m_bDescChanged = value; }
	bool GetDescChanged() const { return m_bDescChanged; }
	void SetSoundChanged(bool value) { m_bSoundChanged = value; }
	bool GetSoundChanged() const { return m_bSoundChanged; }
	void SetAreaChanged(bool value) { m_bAreaChanged = value; }
	bool GetAreaChanged() const { return m_bAreaChanged; }
	void SetRegionChanged(bool value) { m_bRegionChanged = value; }
	bool GetRegionChanged() const { return m_bRegionChanged; }
	// �༭��������ָ�������Ƿ�ж��
	void SetNoUnload(bool value) { m_bNoUnload = value; }
	bool GetNoUnload() const { return m_bNoUnload; }
	
	// �Ƿ��б��ı����Ŀ
	bool GetAnyChanged() const
	{ 
		return m_bHeightChanged || m_bAlphaChanged || m_bLightChanged 
			|| m_bVisualChanged || m_bWidgetChanged || m_bHelperModelChanged 
			|| m_bCollideInfoChanged || m_bDescChanged || m_bSoundChanged 
			|| m_bAreaChanged || m_bRegionChanged;
	}

	// �Ƿ���ˮ��
	void SetHasWater(bool value) { m_bHasWater = value; }
	bool GetHasWater() const { return m_bHasWater; }

	// ������һ���ֿ����ײ��Ϣ
	bool LoadNextCollide();
	
private:
	CTerrainZone();
	CTerrainZone(const CTerrainZone&);
	CTerrainZone& operator=(const CTerrainZone&);

private:
	IRender* m_pRender;
	Terrain* m_pTerrain;
	CQuadNode* m_pQuadNode;
	CTerrainFactory* m_pFactory;
	PERSISTID m_PersistID;
	core_string m_strName;
	size_t m_nIndex;		// ��������
	size_t m_nRow;			// ���к�
	size_t m_nCol;
	size_t m_nGlobalRow;	// ȫ�ֶ�����ʼ���к�
	size_t m_nGlobalCol;
	size_t m_nAlphaRow;		// ȫ��ALPHA��ʼ���к�
	size_t m_nAlphaCol;
	float m_fLeft;			// �������
	float m_fTop;
	float m_fRight;
	float m_fBottom;
	float m_fSilenceTime;	// ��Ĭʱ��
	
	CTerrainHeight* m_pHeight;
	CTerrainLight* m_pLight;
	CTerrainVisuals* m_pVisuals;		// �����ڵĿɼ�����
	CTerrainWalkable* m_pWalkable;
	CTerrainRegion* m_pRegion;
#ifdef PHYSICS_HAVOK_TERRAIN
	CTerrainRigid* m_pRigid;
#endif // PHYSICS_HAVOK_TERRAIN
	
	CHeightLoader* m_pHeightLoader;
	CLightLoader* m_pLightLoader;
	CAlphaLoader* m_pAlphaLoader;
	CVisualLoader* m_pVisualLoader;
	CWalkLoader* m_pWalkLoader;
	CRegionLoader* m_pRegionLoader;
#ifdef PHYSICS_HAVOK_TERRAIN
	CRigidLoader* m_pRigidLoader;
#endif // PHYSICS_HAVOK_TERRAIN

	bool m_bHeightLoaded;
	bool m_bLightLoaded;
	bool m_bAlphaLoaded;
	bool m_bVisualLoaded;
	bool m_bWalkLoaded;
	bool m_bRegionLoaded;
#ifdef PHYSICS_HAVOK_TERRAIN
	bool m_bRigidLoaded;
#endif // PHYSICS_HAVOK_TERRAIN

	bool m_bHeightChanged;
	bool m_bAlphaChanged;
	bool m_bLightChanged;
	bool m_bVisualChanged;
	bool m_bWidgetChanged;
	bool m_bHelperModelChanged;
	bool m_bCollideInfoChanged;
	bool m_bDescChanged;
	bool m_bSoundChanged;
	bool m_bAreaChanged;
	bool m_bRegionChanged;
	bool m_bNoUnload;
	bool m_bHasWater;
	
	float m_fInvChunkSize;
	CTerrainChunk* m_pChunks;	// �����ڵķֿ�
	size_t m_nChunkDims;
	size_t m_nChunkNum;
	size_t m_nLoadCollideNum;	// �Ѽ�����ײ��Ϣ�ķֿ�����
	size_t m_nCollideDims;
	size_t m_nCollideNum;		
	CQuadGround* m_pCollides;
};

#endif // _TERRAIN_ZONE_H
