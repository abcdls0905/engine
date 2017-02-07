#include "ui3d_helper.h"

#include "../visual/i_ui3d_mesh.h"

int GetStride(int fvf)
{
	int offset = 0;

	//FVF
	if(fvf & FVF_POS)
	{
		offset += sizeof(float)*3;
	}
	if(fvf & FVF_TEX0)
	{
		offset += sizeof(float)*2;
	}
	return offset;
}

IUI3DNode* BuildPlane(IUI3DNode* pNode, float w, float h, int fvf, float maxu = 1.0f, float maxv = 1.0f)
{
	int stride = GetStride(fvf);
	
	float half_w = w/2.0f;
	float half_h = h/2.0f;

	FmVec3 v1(-half_w, half_h, 0.0f);
	FmVec3 v2(-half_w, -half_h, 0.0f);
	FmVec3 v3(half_w, half_h, 0.0f);
	FmVec3 v4(half_w, -half_h, 0.0f);

	int size = stride*6;
	char* pp = new char[size];
	char* p = pp;
	(*(FmVec3*)p) = v2;
	p+= stride;
	(*(FmVec3*)p) = v1;
	p+= stride;
	(*(FmVec3*)p) = v4;
	p+= stride;
	(*(FmVec3*)p) = v4;
	p+= stride;
	(*(FmVec3*)p) = v1;
	p+= stride;
	(*(FmVec3*)p) = v3;
	p+= stride;

	if(fvf & FVF_TEX0)
	{
	//	float maxv = 1.0f, maxu = 1.0f;
		FmVec2 uv1(0.0f, 1-0.0f);
		FmVec2 uv2(0.0f, 1-maxv);
		FmVec2 uv3(maxu, 1-0.0f);
		FmVec2 uv4(maxu, 1-maxv);

		p = pp + sizeof(float)*3;
		(*(FmVec2*)p) = uv2;
		p+= stride;
		(*(FmVec2*)p) = uv1;
		p+= stride;
		(*(FmVec2*)p) = uv4;
		p+= stride;
		(*(FmVec2*)p) = uv4;
		p+= stride;
		(*(FmVec2*)p) = uv1;
		p+= stride;
		(*(FmVec2*)p) = uv3;
		p+= stride;
	}

	pNode->Create(fvf, pp, size, 0, 0);
	return pNode;
}

IUI3DMesh * BuildCylinder(IUI3DMesh* mesh, int n, float w, float h, int fvf, float maxu, float maxv)
{
	float t = FM_PI - (n-2)*FM_PI/(2*n) - FM_PI/2;
	float r = w/2.0f/tanf(t);
	
	for(int i = 0; i < n; i++)
	{
		float t = (i+1)/float(n) * FM_TWO_PI;
		float x = sinf(t);
		float z = cosf(t);
		FmVec3 angle(x, 0.0, z);
		FmVec3Normalize(&angle, &angle);
		FmVec3 pos = angle*-r;
		IUI3DNode* pNode = mesh->CreateNode();
		BuildPlane(pNode, w, h, fvf, maxu, maxv);
		pNode->SetPosition(FmVec3(pos.x, 0, pos.z));
		pNode->SetAngle(FmVec3(0, t, 0));
	}
	return 0;
}

