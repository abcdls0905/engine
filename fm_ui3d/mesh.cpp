#include "../visual/i_render.h"
#include "../visual/vis_utils.h"
#include "mesh.h"

_UI3D_BEGIN

//==============================================
// MESH
//==============================================
Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	for(size_t i = 0; i < m_nodes.size(); i++)
		delete(m_nodes[i]);
}

IUI3DNode* Mesh::CreateNode(const char* ident)
{
	Node* pNode = new Node();
	pNode->SetGlobal(&m_mtxWorld);
	m_nodes.push_back(pNode);
	return pNode;
}

void Mesh::Draw()
{
	for(size_t i = 0; i < m_nodes.size(); i++)
	{
		m_nodes[i]->Draw();
	}
}

bool Mesh::Pick(IRenderContext* pContext, int screen_x, int screen_y, FmVec2& uv, int& node_index)
{
	float maxt = 10000.0f;
	int obj =-1;
	FmVec2 _uv;
	float t;
	for(size_t i = 0; i < m_nodes.size(); i++)
	{
		if(m_nodes[i]->Pick(pContext, screen_x, screen_y, uv, t))
		{
			if(t < maxt)
			{
				maxt = t;
				_uv = uv;
				obj = i;
			}
			
		}
	}

	if(obj > -1)
	{
		uv  = _uv;
		node_index = obj;
		return true;
	}

	return false;
}

void Mesh::OnChangedWorldMatrix()
{
	for(size_t i = 0; i < this->m_nodes.size(); i++)
	{
		m_nodes[i]->SetGlobal(&m_mtxWorld);
	}
}

_UI3D_END