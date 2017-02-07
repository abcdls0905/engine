//一个网格管理类，用来蒙皮2dui来实现3d控制效果
#pragma once
#include <vector>
#include "../math/fm_math.h"
#include "../visual/i_resource.h"
#include "defines.h"

#include "Node.h"

_UI3D_BEGIN

class Mesh : public IUI3DMesh
{
	std::vector<Node*> m_nodes;
	void BuildWorldMatrix();
	void OnChangedWorldMatrix();
public:
	Mesh();

	~Mesh();

	void Draw();

	IUI3DNode* CreateNode(const char* ident ="");

	IUI3DNode* GetNode(int index){ return m_nodes[index]; }

	size_t GetNodeCount()const { return m_nodes.size(); }

	bool Pick(IRenderContext* pContext, int screen_x, int screen_y, FmVec2& uv, int& node_index);
};

_UI3D_END