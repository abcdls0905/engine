#ifndef _MODEL_VF_H
#define _MODEL_VF_H

#include <assert.h>

class ModelVF
{
public:
	enum 
	{
		VERTEX,
		TEXCOORD,
		TEXCOORD2,
		TEXCOORD_FILTER,
		NORMAL,
		BONE,
		WEIGHT,
		TANGENT,
		BINORMAL,
		ILLUM,
		DIFFUSE,
		VERTEX_ANI,
		NORMAL_ANI,
		DIFFUSE_ANI,
		TANGENT_ANI,
		ILLUM_ANI,
		BINORMAL_ANI,
		MAX
	};

	int m_vf[MAX];
	int m_offset;

	inline void SetVF(int vf)
	{
		m_vf[vf] = m_offset;
		m_offset ++;
	}

public:
	ModelVF()
	{
		Clear();
	}

	inline void Clear()
	{
		for(int i = 0; i < MAX; i++)
			m_vf[i] = -1;
		m_offset = 0;
	}

	inline long GetVF(int vf)const
	{
		assert(m_vf[vf] > -1);
		return m_vf[vf];
	}

	ModelVF& Inst()
	{
		static ModelVF s_inst;
		return s_inst;
	}

	inline void GetNames(const char** ptr, int& len)
	{
		/*
		enum 
		{
			VERTEX,
			TEXCOORD,
			TEXCOORD2,
			NORMAL,
			BONE,
			WEIGHT,
			TANGENT,
			BINORMAL,
			ILLUM,
			DIFFUSE,
			VERTEX_ANI,
			NORMAL_ANI,
			DIFFUSE_ANI,
			TANGENT_ANI,
			ILLUM_ANI,
			BINORMAL_ANI,
			MAX
		};*/

		static const char* nameses[] = 
		{
			"iPos", "iTex0", "iTex1", "iTexFilter", "iNormal", "iBone", "iWeight", "iTangent", "iBinormal", "iLlum", "iDiffuse", 
			"iPosAni", "iNormalAni", "iDiffuseAni", "iTangentAni", "iLlumAni", "iBinormalAni"
		};
		
		len = 0;
		for(int i = 0; i < MAX; i++)
		{
			if(m_vf[i] < 0)
				continue;
			assert(m_vf[i] >=0 && m_vf[i] < MAX);
			ptr[m_vf[i]] = nameses[i];
			len ++;
		}
	}

	void Parse(int nMaterialInfo, int nMaterialInfoEx, int nModelInfo, int nType, bool lod = false)
	{
		Clear();
		int offset = 0;
		//POS
		if(nModelInfo & MODEL_POSITION_INFO)
		{
			SetVF(VERTEX);
		}
		else if(nModelInfo & MODEL_POSITION_ANI)
		{
			SetVF(VERTEX);
			SetVF(VERTEX_ANI);
		}

		//NORAML
		if (nModelInfo & MODEL_NORMAL_INFO)
		{
			SetVF(NORMAL);
		}
		else if(nModelInfo& MODEL_NORMAL_ANI)
		{
			SetVF(NORMAL);
			SetVF(NORMAL_ANI);
		}
		
		//COLOR
		// Color
		if (nModelInfo & MODEL_COLOR_INFO)
		{
			SetVF(DIFFUSE);
		}
		else if(nModelInfo & MODEL_COLOR_ANI)
		{
			SetVF(DIFFUSE_ANI);
		}

		// Diffuse texture
		if (nMaterialInfo & (MATERIAL_DIFFUSE_MAP_INFO | MATERIAL_BUMP_MAP_INFO
			| MATERIAL_SPECULAR_MAP_INFO | MATERIAL_SELF_ILLUMINATION_INFO))
		{
			SetVF(TEXCOORD);
			if (nMaterialInfo & MATERIAL_LIGHTMAP_INFO)
			{
				SetVF(TEXCOORD2);
			}
			if(nMaterialInfoEx & MATERIAL_FILTER_MAP_INFO)
			{
				SetVF(TEXCOORD_FILTER);
			}
		}

		//BONEID && WEIGHT
		if ((nType != FXNODE_TYPE_MESH) && !lod)
		{
			SetVF(BONE);
			SetVF(WEIGHT);
		}

		// Illum
		if (nModelInfo & MODEL_ILLUM_INFO)
		{
			SetVF(ILLUM);
		}
		else if(nModelInfo & MODEL_ILLUM_ANI)
		{
			SetVF(ILLUM_ANI);
		}

		// Tangent
		if (nModelInfo & MODEL_TANGENT_INFO)
		{
			SetVF(TANGENT);
		}
		else if(nModelInfo & MODEL_TANGENT_ANI)
		{
			SetVF(TANGENT_ANI);
		}
		
		// Binormal
		if (nModelInfo & MODEL_BINORMAL_INFO)
		{
			SetVF(BINORMAL);
		}
		else if(nModelInfo& MODEL_BINORMAL_ANI)
		{
			SetVF(BINORMAL_ANI);
		}
	}
};

#endif