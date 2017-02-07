
/****************************************************************

  Author      :  叶峰
  Version     :  2.0
  Create Date :  2010/01/08   
  Description :  几何图形数据的创建

*****************************************************************/

// INCLUDES -----------------------------------------------------------------------------

#include "geometry_builder.h"

// --------------------------------------------------------------------------------------

#define YD_REAL_PI                      3.14159265358979f               // 圆周率
#define YD_REAL_HALF_PI                 1.57079632679490f               // PI / 2
#define YD_REAL_QUARTER_PI              0.78539816339745f               // PI / 4
#define YD_REAL_TWAIN_PI                6.28318530717958f               // PI * 2

#define YD_MAX_UNSIGNED_INT16           0xffff                          // max possible unsigned short value

#define YD_RADIAN_VS_DEGREE             57.2957795130824f               // 180 / PI
#define YD_DEGREE_VS_RADIAN             0.01745329251994f               // PI / 180

#define YD_RADIAN_TO_DEGREE(x)          ( (x) * YD_RADIAN_VS_DEGREE )   // 弧度转角度
#define YD_DEGREE_TO_RADIAN(x)          ( (x) * YD_DEGREE_VS_RADIAN )   // 角度转弧度

// --------------------------------------------------------------------------------------

// 三角面片的索引结构
struct YsTriIndex16
{
	unsigned short      index0;
	unsigned short      index1;
	unsigned short      index2;
};

struct YsTriIndex32
{
	unsigned int        index0;
	unsigned int        index1;
	unsigned int        index2;
};

// 线的索引结构
struct YsLineIndex16
{
	unsigned short      index0;
	unsigned short      index1;
};

struct YsLineIndex32
{
	unsigned int        index0;
	unsigned int        index1;
};

struct YsVector3
{
	float 				x;
	float 				y;
	float 				z;

	YsVector3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	YsVector3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};

// --------------------------------------------------------------------------------------

bool                        YfCalculateGeometryBufferSize
(
    YeGraphType             graphType,      
    unsigned int            slices, 
    unsigned int            stacks,           
    unsigned int&			numVertices,        // 顶点数目
    unsigned int&			numTriangles,       // 三角形数目
    unsigned int&			numLines            // 线段数目
)
{
    switch (graphType)
    {
    case YE_GRAPH_PLANE:
        {
            if (slices < 2 || stacks < 2)
            {
                return false;
            }
            numVertices  = slices * stacks;
            numTriangles = (slices - 1) * (stacks - 1) * 2;
            numLines     = slices * (stacks - 1) +(slices - 1) * stacks;
        }
        break;

    case YE_GRAPH_BOX:
        {
            numVertices  = 8;
            numTriangles = 12;
            numLines     = 12;
        }
        break;

    case YE_GRAPH_SPHERE:
        {
            if (slices < 2 || stacks < 3)
            {
                return false;
            }
            numVertices  = slices * (stacks - 2) + 2;
            numTriangles = slices * (stacks - 2) * 2;
            numLines     = slices * (stacks - 2) + slices * (stacks - 1);
        }
        break;

    case YE_GRAPH_DRUM:
        {
            if (slices < 2 || stacks < 3)
            {
                return false;
            }
            numVertices  = slices * stacks + 2;
            numTriangles = slices * stacks * 2;
            numLines     = slices * stacks + slices * (stacks + 1);
        }
        break;

    case YE_GRAPH_HEMISPHERE:
        {
            if (slices < 2 || stacks < 3)
            {
                return false;
            }
            numVertices  = slices * (stacks - 1) + 1;
            numTriangles = slices * (stacks - 2) * 2 + slices;
            numLines     = slices * (stacks - 1) * 2;
        }
        break;

    case YE_GRAPH_CONE:
        {
            if (slices < 2)
            {
                return false;
            }
            numVertices  = slices + 2;
            numTriangles = slices * 2;
            numLines     = slices * 2;
        }
        break;

    case YE_GRAPH_CYLINDER:
        { 
            if (slices < 2)
            {
                return false;
            }
            numVertices  = slices * 2 + 2;
            numTriangles = slices * 4;
            numLines     = slices * 5;
        }
        break;

    case YE_GRAPH_CAPSULE:
        {
            unsigned int halfStacks = stacks / 2;
            if (slices < 2 || halfStacks < 2)
            {
                return false;
            }
            numVertices  = slices * (halfStacks - 1) * 2 + 2;
            numTriangles = slices * (halfStacks - 1) * 4;
            numLines     = slices * (halfStacks - 1) * 2 + slices * (2 * halfStacks - 1);
        }
        break;

    case YE_GRAPH_PYRAMID:
        {
            numVertices  = 5;
            numTriangles = 6;
            numLines     = 8;
        }
        break;

    case YE_GRAPH_ROUND:
        {
            if (slices < 3)
            {
                return false;
            }
            numVertices  = slices + 1;
            numTriangles = slices;
            numLines     = slices;
        }
        break;

    case YE_GRAPH_RING:
        {
            if (slices < 2 || stacks < 3)
            {
                return false;
            }
            numVertices  = slices * stacks;
            numTriangles = slices * stacks * 2;
            numLines     = slices * stacks * 2;
        }
        break;

    case YE_GRAPH_PIPE:
        {
            if (slices < 2)
            {
                return false;
            }
            numVertices  = slices * 4;
            numTriangles = slices * 8;
            numLines     = slices * 8;
        }
        break;

    case YE_GRAPH_WEDGE:
        {
            numVertices  = 6;
            numTriangles = 8;
            numLines     = 9;
        }
        break;

    case YE_GRAPH_FAN:
        {
            if (slices < 2)
            {
                return false;
            }
            numVertices  = (slices + 1) * 2;
            numTriangles = slices * 4;
            numLines     = slices + (slices-1) * 2 + 5;
        }
        break;

    case YE_GRAPH_ARC:
        {
            if (slices < 2 || stacks < 3)
            {
                return false;
            }
            numVertices  = slices * (stacks - 1) + 1;
            numTriangles = slices * (stacks - 2) * 2 + slices;
            numLines     = slices * (stacks - 1) * 2;
        }
        break;

    case YE_GRAPH_GEARWHEEL:
        {
            if (slices < 3)
            {
                return false;
            }
            numVertices  = (slices*2) * 2 + 2;
            numTriangles = slices * 8;
            numLines     = (slices*2) * 4;
        }
        break;

    case YE_GRAPH_STAR:
        {
            if (slices < 3)
            {
                return false;
            }
            numVertices  = (slices*2) + 2;
            numTriangles = slices * 4;
            numLines     = slices * 5;
        }
        break;

    case YE_GRAPH_SPIRE:
        {
            if (stacks < 1 || slices < 3)
            {
                return false;
            }
            numVertices  = 2 + stacks * 2;
            numTriangles = stacks * 2;
            numLines     = 1 + stacks * 3;
        }
        break;

    case YE_GRAPH_STAIRS:
        {
            if (stacks < 1)
            {
                return false;
            }
            numVertices  = 2 + stacks * 4;
            numTriangles = stacks * 8;
            numLines     = 1 + stacks * 8;
        }
        break;

    case YE_GRAPH_SPIRAL_STAIRS:
        {
            if (stacks < 1)
            {
                return false;
            }
            numVertices  = 2 + stacks * 4;
            numTriangles = stacks * 8;
            numLines     = 1 + stacks * 8;
        }
        break;

    default:
        {
            numVertices  = 0;
            numTriangles = 0;
            numLines     = 0;
            return false;
        }
        break;
    }

    return true;
}

bool                        YfBuildGeometry
(
    const YsGraphDesc&      graphDesc, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,    // 顶点数据指针
    void*                   pTriIndicesBuffer,  // 三角形索引数据指针
    void*                   pWireIndicesBuffer  // 线段索引数据指针
)
{
    switch (graphDesc.graphType)
    {
    case YE_GRAPH_PLANE:
        {
            return YfBuildPlaneGeometry(
                graphDesc.width, 
                graphDesc.length, 
                graphDesc.posY, 
                graphDesc.slices, 
                graphDesc.stacks, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer);
        }
        break;
    case YE_GRAPH_BOX:
        {
            return YfBuildBoxGeometry(
                graphDesc.extentX, 
                graphDesc.extentY, 
                graphDesc.extentZ, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer); 
        }
        break;
    case YE_GRAPH_SPHERE:
        {
            return YfBuildSphereGeometry(
                graphDesc.radius, 
                graphDesc.slices, 
                graphDesc.stacks, 
                graphDesc.originPose, 
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer);  
        }
        break;
    case YE_GRAPH_DRUM:
        {
            return YfBuildDrumGeometry(
                graphDesc.radius, 
                graphDesc.assistRadius,
                graphDesc.slices, 
                graphDesc.stacks, 
                graphDesc.originPose, 
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer);  
        }
        break;
    case YE_GRAPH_HEMISPHERE:
        {
            return YfBuildHemisphereGeometry(
                graphDesc.radius, 
                graphDesc.slices, 
                graphDesc.stacks, 
                graphDesc.originPose, 
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer);  
        }
        break;
    case YE_GRAPH_CONE:
        {
            return YfBuildConeGeometry(
                graphDesc.radius, 
                graphDesc.height, 
                graphDesc.slices, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer);  
        }
        break;
    case YE_GRAPH_CYLINDER:
        { 
            return YfBuildCylinderGeometry(
                graphDesc.radius, 
                graphDesc.assistRadius, 
                graphDesc.height, 
                graphDesc.slices, 
                graphDesc.originPose, 
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer);  
        }
        break;
    case YE_GRAPH_CAPSULE:
        {
            return YfBuildCapsuleGeometry(
                graphDesc.radius, 
                graphDesc.height, 
                graphDesc.slices, 
                graphDesc.stacks, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer);  
        }
        break;
    case YE_GRAPH_PYRAMID:
        {
            return YfBuildPyramidGeometry(
                graphDesc.width, 
                graphDesc.length, 
                graphDesc.height, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer); 
        }
        break;
    case YE_GRAPH_ROUND:
        {
            return YfBuildRoundGeometry(
                graphDesc.radius, 
                graphDesc.posY, 
                graphDesc.slices, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer); 

        }
        break;
    case YE_GRAPH_RING:
        {
            return YfBuildRingGeometry(
                graphDesc.radius,
                graphDesc.assistRadius, 
                graphDesc.height, 
                graphDesc.slices, 
                graphDesc.stacks, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer); 
        }
        break;
    case YE_GRAPH_PIPE:
        {
            return YfBuildPipeGeometry(
                graphDesc.radius, 
                graphDesc.assistRadius, 
                graphDesc.height, 
                graphDesc.slices, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer); 
        }
        break;
    case YE_GRAPH_WEDGE:
        {
            return YfBuildWedgeGeometry(
                graphDesc.width, 
                graphDesc.length, 
                graphDesc.height, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer); 
        }
        break;
    case YE_GRAPH_FAN:
        {
            return YfBuildFunGeometry(
                graphDesc.radius, 
                graphDesc.degree, 
                graphDesc.height, 
                graphDesc.slices, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer); 
        }
        break;
    case YE_GRAPH_ARC:
        {
            return YfBuildArcGeometry(
                graphDesc.radius, 
                graphDesc.degree, 
                graphDesc.height, 
                graphDesc.slices, 
                graphDesc.stacks, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer); 
        }
        break;
    case YE_GRAPH_GEARWHEEL:
        {
            return YfBuildGearWheelGeometry(
                graphDesc.radius, 
                graphDesc.assistRadius, 
                graphDesc.height, 
                graphDesc.slices, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer); 
        }
        break;
    case YE_GRAPH_STAR:
        {
            return YfBuildStarGeometry(
                graphDesc.radius, 
                graphDesc.assistRadius, 
                graphDesc.height, 
                graphDesc.slices, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer); 
        }
        break;
    case YE_GRAPH_SPIRE:
        {
            return YfBuildSpireGeometry(
                graphDesc.radius, 
                graphDesc.assistRadius, 
                graphDesc.height, 
                graphDesc.slices, 
                graphDesc.stacks, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer); 
        }
        break;
    case YE_GRAPH_STAIRS:
        {
            return YfBuildStairsGeometry(
                graphDesc.width, 
                graphDesc.length, 
                graphDesc.height, 
                graphDesc.stacks, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer);   
        }
        break;
    case YE_GRAPH_SPIRAL_STAIRS:
        {
            return YfBuildSpiralStairsGeometry(
                graphDesc.radius,
                graphDesc.assistRadius,
                graphDesc.height, 
                graphDesc.slices, 
                graphDesc.stacks, 
                graphDesc.originPose,
                dataDesc, 
                pVerticesBuffer,
                pTriIndicesBuffer,
                pWireIndicesBuffer);
        }
        break;
    default:
        {
            return false;
        }
        break;
    }

    return false;
}

// --------------------------------------------------------------------------------------

// ******************* //
bool                        YfBuildPlaneVertices
(
    float                   width, 
    float                   length, 
    float                   height, 
    unsigned int            slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose,     
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,    
    void*                   pVerticesBuffer
)
{
    if (slices < 2 || stacks < 2 || !pVerticesBuffer)
    {
        return false;
    }

    // 顶点赋值
    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr = 0;
    unsigned int nOffset = 0;
  
    float xStep = width  / slices;
    float zStep = length / stacks;
    YsVector3 vOrigin;
    if (originPose == YE_ORIGIN_POSE_CENTER)
    {
        vOrigin.x = -width * 0.5f;
        vOrigin.z = -length * 0.5f;
    }

    for (unsigned int j = 0; j < stacks; j++)             // Z方向
    {
        for (unsigned int i = 0; i < slices; i++)         // X方向
        {
            nOffset =  (j*slices + i) * vertexStriding;
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = vOrigin.x + i*xStep;
            curVertexPtr->y = height;
            curVertexPtr->z = vOrigin.z + j*zStep;
        }
    }

    return true;
}

bool                        YfBuildPlaneTriIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (slices < 2 || stacks < 2 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * stacks;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    //unsigned int numTriangles = (slices - 1) * (stacks - 1) * 2;

    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;

    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;

        for (unsigned int j = 0; j < stacks - 1; j++)             // Z方向
        {
            for (unsigned int i = 0; i < slices - 1; i++)         // X方向
            {
                nOffset = (2 * (j*(slices - 1) + i)) * indexStriding;
                triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                triIndexPtr->index0 = j*slices + i;
                triIndexPtr->index1 = (j + 1)*slices + i + 1;
                triIndexPtr->index2 = j*slices + i + 1;

                nOffset += indexStriding;                
                triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                triIndexPtr->index0 = j*slices + i;
                triIndexPtr->index1 = (j + 1)*slices + i;
                triIndexPtr->index2 = (j + 1)*slices + i + 1;
            }
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;

        for (unsigned int j = 0; j < stacks - 1; j++)             // Z方向
        {
            for (unsigned int i = 0; i < slices - 1; i++)         // X方向
            {
                nOffset = (2 * (j*(slices - 1) + i)) * indexStriding;
                triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                triIndexPtr->index0 = j*slices + i;
                triIndexPtr->index1 = (j + 1)*slices + i + 1;
                triIndexPtr->index2 = j*slices + i + 1;

                nOffset += indexStriding;                
                triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                triIndexPtr->index0 = j*slices + i;
                triIndexPtr->index1 = (j + 1)*slices + i;
                triIndexPtr->index2 = (j + 1)*slices + i + 1;
            }
        }
    }

    return true;
}

bool                        YfBuildPlaneWireIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || stacks < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * stacks;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    //unsigned int numLines = slices * (stacks - 1) +(slices - 1) * stacks;

    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;

    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;

        // 行
        for (unsigned int j = 0; j < stacks; j++)                 
        {
            for (unsigned int i = 0; i < slices - 1; i++)         
            {
                nOffset = ((j*(slices - 1) + i)) * indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                lineIndexPtr->index0 = j*slices + i;
                lineIndexPtr->index1 = j*slices + i + 1;
            }
        }

        // 列
        unsigned int half = (slices - 1) * stacks;
        for (unsigned int i = 0; i < slices; i++)                 
        {
            for (unsigned int j = 0; j < stacks - 1; j++)         
            {
                nOffset = (half + (i*(stacks - 1) + j)) * indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                lineIndexPtr->index0 = j*slices + i;
                lineIndexPtr->index1 = (j + 1)*slices + i;
            }
        }
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;

        // 行
        for (unsigned int j = 0; j < stacks; j++)                 
        {
            for (unsigned int i = 0; i < slices - 1; i++)         
            {
                nOffset = ((j*(slices - 1) + i)) * indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = j*slices + i;
                lineIndexPtr->index1 = j*slices + i + 1;
            }
        }

        // 列
        unsigned int half = (slices - 1) * stacks;
        for (unsigned int i = 0; i < slices; i++)                 
        {
            for (unsigned int j = 0; j < stacks - 1; j++)         
            {
                nOffset = (half + (i*(stacks - 1) + j)) * indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = j*slices + i;
                lineIndexPtr->index1 = (j + 1)*slices + i;
            }
        }
    }

    return true;
}

// ******************* //
bool                        YfBuildBoxVertices
(
    float                   extentX,
    float                   extentY,  
    float                   extentZ, 
    YeOriginPose            originPose,
    unsigned int            vertexStriding,
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (!pVerticesBuffer)
    {
        return false;
    }

    //unsigned int numVertices = 8;

    // 顶点赋值
    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;

    YsVector3* curVertexPtr   = 0;
    YsVector3* curNormalPtr   = 0;

    unsigned int nOffset = 0;

    YsVector3 vOriginOffset(-extentX / 2, -extentY / 2, -extentZ / 2);
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        vOriginOffset.y = -extentY;
    }
    else if (originPose == YE_ORIGIN_POSE_BOTTOM)
    {
        vOriginOffset.y = 0.0f;
    }

    for (unsigned int k = 0; k < 2; k++)
    {
        for (unsigned int j = 0; j < 2; j++)
        {
            for (unsigned int i = 0; i < 2; i++)
            {
                nOffset = (k*4 + j*2 + i) * vertexStriding;
                curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
                curVertexPtr->x = extentX*i + vOriginOffset.x;
                curVertexPtr->z = extentZ*j + vOriginOffset.z;
                curVertexPtr->y = extentY*k + vOriginOffset.y;
            }
        }
    }

    return true;
}  

bool                        YfBuildBoxTriIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (!pTriIndicesBuffer)
    {
        return false;
    }

    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr[12];
        for (unsigned int i = 0; i < 12; i++)
        {
            triIndexPtr[i] = (YsTriIndex16*)(indexPtr + i * indexStriding);
        }

        // bottom
        triIndexPtr[0]->index0 = 0; triIndexPtr[0]->index1 = 1; triIndexPtr[0]->index2 = 3;
        triIndexPtr[1]->index0 = 0; triIndexPtr[1]->index1 = 3; triIndexPtr[1]->index2 = 2;

        // top                                                   
        triIndexPtr[10]->index0= 4; triIndexPtr[10]->index1= 6; triIndexPtr[10]->index2= 7;
        triIndexPtr[11]->index0= 4; triIndexPtr[11]->index1= 7; triIndexPtr[11]->index2= 5;

        // front side
        triIndexPtr[2]->index0 = 0; triIndexPtr[2]->index1 = 4; triIndexPtr[2]->index2 = 5;
        triIndexPtr[3]->index0 = 0; triIndexPtr[3]->index1 = 5; triIndexPtr[3]->index2 = 1;

        // back side
        triIndexPtr[4]->index0 = 2; triIndexPtr[4]->index1 = 3; triIndexPtr[4]->index2 = 7;
        triIndexPtr[5]->index0 = 2; triIndexPtr[5]->index1 = 7; triIndexPtr[5]->index2 = 6;

        // right side
        triIndexPtr[6]->index0 = 1; triIndexPtr[6]->index1 = 5; triIndexPtr[6]->index2 = 7;
        triIndexPtr[7]->index0 = 1; triIndexPtr[7]->index1 = 7; triIndexPtr[7]->index2 = 3;

        // left side                                                       
        triIndexPtr[8]->index0 = 0; triIndexPtr[8]->index1 = 2; triIndexPtr[8]->index2 = 6;
        triIndexPtr[9]->index0 = 0; triIndexPtr[9]->index1 = 6; triIndexPtr[9]->index2 = 4;
    }
    else
    {
        YsTriIndex32* triIndexPtr[12];
        for (unsigned int i = 0; i < 12; i++)
        {
            triIndexPtr[i] = (YsTriIndex32*)(indexPtr + i * indexStriding);
        }

        // bottom
        triIndexPtr[0]->index0 = 0; triIndexPtr[0]->index1 = 1; triIndexPtr[0]->index2 = 3;
        triIndexPtr[1]->index0 = 0; triIndexPtr[1]->index1 = 3; triIndexPtr[1]->index2 = 2;

        // top                                                   
        triIndexPtr[10]->index0= 4; triIndexPtr[10]->index1= 6; triIndexPtr[10]->index2= 7;
        triIndexPtr[11]->index0= 4; triIndexPtr[11]->index1= 7; triIndexPtr[11]->index2= 5;

        // front side
        triIndexPtr[2]->index0 = 0; triIndexPtr[2]->index1 = 4; triIndexPtr[2]->index2 = 5;
        triIndexPtr[3]->index0 = 0; triIndexPtr[3]->index1 = 5; triIndexPtr[3]->index2 = 1;

        // back side
        triIndexPtr[4]->index0 = 2; triIndexPtr[4]->index1 = 3; triIndexPtr[4]->index2 = 7;
        triIndexPtr[5]->index0 = 2; triIndexPtr[5]->index1 = 7; triIndexPtr[5]->index2 = 6;

        // right side
        triIndexPtr[6]->index0 = 1; triIndexPtr[6]->index1 = 5; triIndexPtr[6]->index2 = 7;
        triIndexPtr[7]->index0 = 1; triIndexPtr[7]->index1 = 7; triIndexPtr[7]->index2 = 3;

        // left side                                                       
        triIndexPtr[8]->index0 = 0; triIndexPtr[8]->index1 = 2; triIndexPtr[8]->index2 = 6;
        triIndexPtr[9]->index0 = 0; triIndexPtr[9]->index1 = 6; triIndexPtr[9]->index2 = 4;
    }

    return true;
}  

bool                        YfBuildBoxWireIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (!pWireIndicesBuffer)
    {
        return false;
    }

    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr[12];
        for (unsigned int i = 0; i < 12; i++)
        {
            lineIndexPtr[i] = (YsLineIndex16*)(indexPtr + i * indexStriding);
        }

        // bottom
        lineIndexPtr[0]->index0 = 0; lineIndexPtr[0]->index1 = 1; 
        lineIndexPtr[1]->index0 = 0; lineIndexPtr[1]->index1 = 2; 
        lineIndexPtr[2]->index0 = 3; lineIndexPtr[2]->index1 = 1; 
        lineIndexPtr[3]->index0 = 3; lineIndexPtr[3]->index1 = 2; 

        // center
        lineIndexPtr[4]->index0 = 0; lineIndexPtr[4]->index1 = 4; 
        lineIndexPtr[5]->index0 = 1; lineIndexPtr[5]->index1 = 5; 
        lineIndexPtr[6]->index0 = 2; lineIndexPtr[6]->index1 = 6; 
        lineIndexPtr[7]->index0 = 3; lineIndexPtr[7]->index1 = 7; 
                                        
        // top                                               
        lineIndexPtr[8]->index0 = 4; lineIndexPtr[8]->index1 = 5; 
        lineIndexPtr[9]->index0 = 4; lineIndexPtr[9]->index1 = 6; 
        lineIndexPtr[10]->index0= 7; lineIndexPtr[10]->index1= 5; 
        lineIndexPtr[11]->index0= 7; lineIndexPtr[11]->index1= 6;    
    }
    else
    {
        YsLineIndex32* lineIndexPtr[12];
        for (unsigned int i = 0; i < 12; i++)
        {
            lineIndexPtr[i] = (YsLineIndex32*)(indexPtr + i * indexStriding);
        }

        // bottom
        lineIndexPtr[0]->index0 = 0; lineIndexPtr[0]->index1 = 1; 
        lineIndexPtr[1]->index0 = 0; lineIndexPtr[1]->index1 = 2; 
        lineIndexPtr[2]->index0 = 3; lineIndexPtr[2]->index1 = 1; 
        lineIndexPtr[3]->index0 = 3; lineIndexPtr[3]->index1 = 2; 

        // center
        lineIndexPtr[4]->index0 = 0; lineIndexPtr[4]->index1 = 4; 
        lineIndexPtr[5]->index0 = 1; lineIndexPtr[5]->index1 = 5; 
        lineIndexPtr[6]->index0 = 2; lineIndexPtr[6]->index1 = 6; 
        lineIndexPtr[7]->index0 = 3; lineIndexPtr[7]->index1 = 7; 
                                        
        // top                                               
        lineIndexPtr[8]->index0 = 4; lineIndexPtr[8]->index1 = 5; 
        lineIndexPtr[9]->index0 = 4; lineIndexPtr[9]->index1 = 6; 
        lineIndexPtr[10]->index0= 7; lineIndexPtr[10]->index1= 5; 
        lineIndexPtr[11]->index0= 7; lineIndexPtr[11]->index1= 6;  
    }

    return true;
}  

// ******************* //
bool                        YfBuildSphereVertices
(
    float                   radius, 
    unsigned int            slices,
    unsigned int            stacks, 
    YeOriginPose            originPose,  
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (slices < 2 || stacks < 3 || !pVerticesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * (stacks - 2) + 2;

    // 顶点赋值
    char* vertexPtr   = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -radius;
    }
    else if (originPose == YE_ORIGIN_POSE_BOTTOM)
    {
        originOffsetY = radius;
    }

    float* pSinList = (float*)malloc(sizeof(float)*slices);
    float* pCosList = (float*)malloc(sizeof(float)*slices);
    float angleXZ;
    for (unsigned int j = 0; j < slices; j++)
    {
        angleXZ = YD_REAL_TWAIN_PI * j / slices;
        pSinList[j] = sinf(angleXZ);
        pCosList[j] = cosf(angleXZ);
    }

    // 赋值
    {
        for (unsigned int i = 0; i < stacks; i++)
        {
            if (i == 0)                         // 第一个顶点
            {
                nOffset = 0;            
                curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
                curVertexPtr->x = 0.0f;
                curVertexPtr->y = radius + originOffsetY;
                curVertexPtr->z = 0.0f;
                continue;
            }
            else if (i == stacks - 1)           // 最后一个顶点
            {        
                nOffset = (numVertices - 1) * vertexStriding; 
                curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
                curVertexPtr->x = 0.0f;
                curVertexPtr->y = -radius + originOffsetY;
                curVertexPtr->z = 0.0f;
                continue;
            }

            float angleY = YD_REAL_PI * i / (stacks - 1);
            float posY = radius * cosf(angleY);
            float radiusXZ = radius * sinf(angleY);
            float posX, posZ;

            for (unsigned int j = 0; j < slices; j++)
            {
                posX = radiusXZ * pSinList[j];
                posZ = radiusXZ * pCosList[j];
                nOffset = ((i - 1) * slices + j + 1) * vertexStriding; 
                curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
                curVertexPtr->x = posX;
                curVertexPtr->y = posY + originOffsetY;
                curVertexPtr->z = posZ;
            }
        }
    }

    free(pSinList);
    free(pCosList);

    return true;
}     

bool                        YfBuildSphereTriIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (slices < 2 || stacks < 3 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * (stacks - 2) + 2;
    unsigned int numTriangles = slices * (stacks - 2) * 2;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;

        for (unsigned int i = 0; i < stacks - 1; i++)
        {
            if (i == 0)                                 // 第一层
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = j * indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 0;
                    triIndexPtr->index1 = 1 + j;
                    triIndexPtr->index2 = 1 + (j + 1)%slices;
                }
            }
            else if (i == stacks - 2)                   // 最后一层
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = (numTriangles - slices + j) * indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = numVertices - 1;
                    triIndexPtr->index1 = numVertices - 1 - slices + (j + 1)%slices;
                    triIndexPtr->index2 = numVertices - 1 - slices + j;
                }
            }
            else
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = ((i - 1)*slices * 2 + slices + j * 2) * indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * (i - 1) + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + (j + 1)%slices;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * i + (j + 1)%slices;
                }
            }
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;

        // 赋值
        for (unsigned int i = 0; i < stacks - 1; i++)
        {
            if (i == 0)                                 // 第一层
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = j * indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 0;
                    triIndexPtr->index1 = 1 + j;
                    triIndexPtr->index2 = 1 + (j + 1)%slices;
                }
            }
            else if (i == stacks - 2)                   // 最后一层
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = (numTriangles - slices + j) * indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = numVertices - 1;
                    triIndexPtr->index1 = numVertices - 1 - slices + (j + 1)%slices;
                    triIndexPtr->index2 = numVertices - 1 - slices + j;
                }
            }
            else
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = ((i - 1)*slices * 2 + slices + j * 2) * indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * (i - 1) + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + (j + 1)%slices;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * i + (j + 1)%slices;
                }
            }
        }
    }

    return true;
}  

bool                        YfBuildSphereWireIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || stacks < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices = slices * (stacks - 2) + 2;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }
    //unsigned int numLines = slices * (stacks - 2) + slices * (stacks - 1);

    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;

    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;

        // 行
        for (unsigned int j = 1; j < stacks - 1; j++)                 
        {
            for (unsigned int i = 0; i < slices; i++)         
            {
                nOffset = ((j - 1)*slices + i) * indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
                lineIndexPtr->index1 = 1 + (j - 1)*slices + (i + 1)%slices;
            }
        }

        // 列
        unsigned int half = slices * (stacks - 2);
        for (unsigned int i = 0; i < slices; i++)                 
        {
            for (unsigned int j = 0; j < stacks - 1; j++)         
            {
                nOffset = (half + (i*(stacks - 1) + j)) * indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                if (j == 0)
                {
                    lineIndexPtr->index0 = 0;
                }
                else
                {
                    lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
                }
                if (j == stacks - 2)
                {
                    lineIndexPtr->index1 = numVertices - 1;
                }
                else
                {
                    lineIndexPtr->index1 = 1 + j*slices + i;
                }
            }
        }
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;

        // 行
        for (unsigned int j = 1; j < stacks - 1; j++)                 
        {
            for (unsigned int i = 0; i < slices; i++)         
            {
                nOffset = ((j - 1)*slices + i) * indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + j*slices + i;
                lineIndexPtr->index1 = 1 + j*slices + (i + 1)%slices;
            }
        }

        // 列
        unsigned int half = slices * (stacks - 2);
        for (unsigned int i = 0; i < slices; i++)                 
        {
            for (unsigned int j = 0; j < stacks - 1; j++)         
            {
                nOffset = (half + (i*(stacks - 1) + j)) * indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                if (j == 0)
                {
                    lineIndexPtr->index0 = 0;
                }
                else
                {
                    lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
                }
                if (j == stacks - 2)
                {
                    lineIndexPtr->index1 = numVertices - 1;
                }
                else
                {
                    lineIndexPtr->index1 = 1 + j*slices + i;
                }
            }
        }
    }

    return true;
}

// ******************* //
bool   YfBuildDrumVertices
(
    float                   radius, 
    float                   assistRadius,
    unsigned int            slices,
    unsigned int            stacks, 
    YeOriginPose            originPose,  
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (slices < 2 || stacks < 3 || !pVerticesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * stacks + 2;

    // 顶点赋值
    char* vertexPtr   = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -radius;
    }
    else if (originPose == YE_ORIGIN_POSE_BOTTOM)
    {
        originOffsetY = radius;
    }

    float* pSinList = (float*)malloc(sizeof(float)*slices);
    float* pCosList = (float*)malloc(sizeof(float)*slices);
    float angleXZ;
    for (unsigned int j = 0; j < slices; j++)
    {
        angleXZ = YD_REAL_TWAIN_PI * j / slices;
        pSinList[j] = sinf(angleXZ);
        pCosList[j] = cosf(angleXZ);
    }

    // 赋值
    {
        // 第一个顶点
        {
            nOffset = 0;            
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = 0.0f;
            curVertexPtr->y = radius + originOffsetY;
            curVertexPtr->z = 0.0f;
        }

        // 最后一个顶点
        {        
            nOffset = (numVertices - 1) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = 0.0f;
            curVertexPtr->y = -radius + originOffsetY;
            curVertexPtr->z = 0.0f;
        }

        for (unsigned int i = 0; i < stacks; i++)
        {
            float angleY = YD_REAL_PI * i / (stacks - 1);
            float posY = radius * cosf(angleY);
            float radiusXZ = assistRadius + radius * sinf(angleY);
            float posX, posZ;

            for (unsigned int j = 0; j < slices; j++)
            {
                posX = radiusXZ * pSinList[j];
                posZ = radiusXZ * pCosList[j];
                nOffset = (i * slices + j + 1) * vertexStriding; 
                curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
                curVertexPtr->x = posX;
                curVertexPtr->y = posY + originOffsetY;
                curVertexPtr->z = posZ;
            }
        }
    }

    free(pSinList);
    free(pCosList);

    return true;
}

bool   YfBuildDrumTriIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    return YfBuildSphereTriIndices(
        slices,
        stacks + 2, 
        indexType,
        indexStriding,  
        indexPos,
        pTriIndicesBuffer);
}

bool   YfBuildDrumWireIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    return YfBuildSphereWireIndices(
        slices,
        stacks + 2, 
        indexType,
        indexStriding,  
        indexPos,
        pWireIndicesBuffer);
}

// ******************* //
bool                        YfBuildHemisphereVertices
(
    float                   radius, 
    unsigned int            slices,
    unsigned int            stacks, 
    YeOriginPose            originPose,  
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (slices < 2 || stacks < 3 || !pVerticesBuffer)
    {
        return false;
    }

    //unsigned int numVertices  = slices * (stacks - 1) + 1;

    // 顶点赋值
    char* vertexPtr   = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -radius;
    }
    else if (originPose == YE_ORIGIN_POSE_BOTTOM)
    {
        originOffsetY = radius;
    }

    float* pSinList = (float*)malloc(sizeof(float)*slices);
    float* pCosList = (float*)malloc(sizeof(float)*slices);
    float angleXZ;
    for (unsigned int j = 0; j < slices; j++)
    {
        angleXZ = YD_REAL_TWAIN_PI * j / slices;
        pSinList[j] = sinf(angleXZ);
        pCosList[j] = cosf(angleXZ);
    }

    // 赋值
    {
        for (unsigned int i = 0; i < stacks; i++)
        {
            if (i == 0)                         // 第一个顶点
            {
                nOffset = 0;            
                curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
                curVertexPtr->x = 0.0f;
                curVertexPtr->y = radius + originOffsetY;
                curVertexPtr->z = 0.0f;
                continue;
            }

            float angleY = YD_REAL_HALF_PI * i / (stacks - 1);
            float posY = radius * cosf(angleY);
            float radiusXZ = radius * sinf(angleY);
            float posX, posZ;

            for (unsigned int j = 0; j < slices; j++)
            {
                posX = radiusXZ * pSinList[j];
                posZ = radiusXZ * pCosList[j];
                nOffset = ((i - 1) * slices + j + 1) * vertexStriding; 
                curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
                curVertexPtr->x = posX;
                curVertexPtr->y = posY + originOffsetY;
                curVertexPtr->z = posZ;
            }
        }
    }

    free(pSinList);
    free(pCosList);

    return true;
}     

bool                        YfBuildHemisphereTriIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (slices < 2 || stacks < 3 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * (stacks - 1) + 1;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }
    //unsigned int numTriangles = slices * (stacks - 2) * 2 + slices;

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;

        for (unsigned int i = 0; i < stacks - 1; i++)
        {
            if (i == 0)                                 // 第一层
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = j * indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 0;
                    triIndexPtr->index1 = 1 + j;
                    triIndexPtr->index2 = 1 + (j + 1)%slices;
                }
            }
            else
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = (slices + (i - 1)*slices * 2 + j * 2) * indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * (i - 1) + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + (j + 1)%slices;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * i + (j + 1)%slices;
                }
            }
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;

        // 赋值
        for (unsigned int i = 0; i < stacks - 1; i++)
        {
            if (i == 0)                                 // 第一层
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = j * indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 0;
                    triIndexPtr->index1 = 1 + j;
                    triIndexPtr->index2 = 1 + (j + 1)%slices;
                }
            }
            else
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = (slices + (i - 1)*slices * 2 + j * 2) * indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * (i - 1) + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + (j + 1)%slices;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * i + (j + 1)%slices;
                }
            }
        }
    }

    return true;
}  

bool                        YfBuildHemisphereWireIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || stacks < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * (stacks - 1) + 1;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }
    //unsigned int numLines = slices * (stacks - 1) * 2;

    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;

    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;

        // 行
        for (unsigned int j = 1; j < stacks; j++)                 
        {
            for (unsigned int i = 0; i < slices; i++)         
            {
                nOffset = ((j - 1)*slices + i) * indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
                lineIndexPtr->index1 = 1 + (j - 1)*slices + (i + 1)%slices;
            }
        }

        unsigned int count = slices * (stacks - 1);
        
        // 列
        for (unsigned int i = 0; i < slices; i++)         
        {
            nOffset = (count + i) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i;
        }
        count += slices;

        for (unsigned int j = 1; j < stacks - 1; j++)                 
        {
            for (unsigned int i = 0; i < slices; i++)         
            {
                nOffset = (count + (j - 1)*slices + i) * indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
                lineIndexPtr->index1 = 1 + j*slices + i;
            }
        }
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;

        // 行
        for (unsigned int j = 1; j < stacks; j++)                 
        {
            for (unsigned int i = 0; i < slices; i++)         
            {
                nOffset = ((j - 1)*slices + i) * indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
                lineIndexPtr->index1 = 1 + (j - 1)*slices + (i + 1)%slices;
            }
        }

        unsigned int count = slices * (stacks - 1);

        // 列
        for (unsigned int i = 0; i < slices; i++)         
        {
            nOffset = (count + i) * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i;
        }
        count += slices;

        for (unsigned int j = 1; j < stacks - 1; j++)                 
        {
            for (unsigned int i = 0; i < slices; i++)         
            {
                nOffset = (count + (j - 1)*slices + i) * indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
                lineIndexPtr->index1 = 1 + j*slices + i;
            }
        }
    }

    return true;
}

// ******************* //
bool                        YfBuildConeVertices
(
    float                   radius, 
    float                   height, 
    unsigned int            slices, 
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (slices < 2 || !pVerticesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices + 2;

    // 顶点赋值
    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -height;
    }
    else if (originPose == YE_ORIGIN_POSE_CENTER)
    {
        originOffsetY = -height * 0.5f;
    }

    // 锥尖顶点赋值
    {
        nOffset = 0;   
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = height + originOffsetY;
        curVertexPtr->z = 0.0f;
    }

    // 锥底顶点赋值
    {
        nOffset = (numVertices - 1) * vertexStriding;   
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = originOffsetY;
        curVertexPtr->z = 0.0f;
    }

    float angleXZ;
    float posX, posZ;        
    for (unsigned int i = 0; i < slices; i++)
    {
        angleXZ = YD_REAL_TWAIN_PI * i / slices;
        posX = sinf(angleXZ);
        posZ = cosf(angleXZ);
        nOffset = (i + 1) * vertexStriding; 

        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = radius * posX;
        curVertexPtr->y = originOffsetY;
        curVertexPtr->z = radius * posZ;
    }

    return true;
}     

bool                        YfBuildConeTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)                           
{
    if (slices < 2 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices + 2;
    //unsigned int numTriangles = slices * 2;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = (i * 2) * indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + (i + 1)%slices;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = numVertices - 1;
            triIndexPtr->index1 = 1 + (i + 1)%slices;
            triIndexPtr->index2 = 1 + i;
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = (i * 2) * indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + (i + 1)%slices;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = numVertices - 1;
            triIndexPtr->index1 = 1 + (i + 1)%slices;
            triIndexPtr->index2 = 1 + i;
        }
    }

    return true;
}     

bool                        YfBuildConeWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices = slices + 2;
    //unsigned int numLines    = slices * 2;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = (i * 2) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + (i + 1)%slices;
        }
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = (i * 2) * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + (i + 1)%slices;
        }
    }

    return true;
}  

// ******************* //
bool                        YfBuildCylinderVertices
(
    float                   topRadius, 
    float                   bottomRadius, 
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (slices < 2 || !pVerticesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * 2 + 2;

    // 顶点赋值
    char* vertexPtr   = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -height;
    }
    else if (originPose == YE_ORIGIN_POSE_CENTER)
    {
        originOffsetY = -height * 0.5f;
    }

    // 柱顶顶点赋值
    {
        nOffset = 0;   
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = height + originOffsetY;
        curVertexPtr->z = 0.0f;
    }

    // 柱底顶点赋值
    {
        nOffset = (numVertices - 1) * vertexStriding;   
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = originOffsetY;
        curVertexPtr->z = 0.0f;
    }

    float angleXZ;
    float posX, posZ;        
    for (unsigned int i = 0; i < slices; i++)
    {
        angleXZ = YD_REAL_TWAIN_PI * i / slices;
        posX = sinf(angleXZ);
        posZ = cosf(angleXZ);

        // 上顶点
        {
            nOffset = (i + 1) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = topRadius * posX;
            curVertexPtr->y = height + originOffsetY;
            curVertexPtr->z = topRadius * posZ;
        }

        // 下顶点
        {
            nOffset = (i + 1 + slices) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = bottomRadius * posX;
            curVertexPtr->y = originOffsetY;
            curVertexPtr->z = bottomRadius * posZ;
        }
    }

    return true;
}     

bool                        YfBuildCylinderTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (slices < 2 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * 2 + 2;
    //unsigned int numTriangles = slices * 4;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = (i * 4) * indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + (i + 1)%slices;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = numVertices - 1;
            triIndexPtr->index1 = 1 + slices + (i + 1)%slices;
            triIndexPtr->index2 = 1 + slices + i;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + i;
            triIndexPtr->index1 = 1 + slices + i;
            triIndexPtr->index2 = 1 + (i + 1)%slices;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + (i + 1)%slices;
            triIndexPtr->index1 = 1 + slices + i;
            triIndexPtr->index2 = 1 + slices + (i + 1)%slices;
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = (i * 4) * indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + (i + 1)%slices;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = numVertices - 1;
            triIndexPtr->index1 = 1 + slices + (i + 1)%slices;
            triIndexPtr->index2 = 1 + slices + i;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + i;
            triIndexPtr->index1 = 1 + slices + i;
            triIndexPtr->index2 = 1 + (i + 1)%slices;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + (i + 1)%slices;
            triIndexPtr->index1 = 1 + slices + i;
            triIndexPtr->index2 = 1 + slices + (i + 1)%slices;
        }
    }

    return true;
}    

bool                        YfBuildCylinderWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices = slices * 2  + 2;
    //unsigned int numLines    = slices * 5;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = (i * 5) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + (i + 1)%slices;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + slices + i;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + slices + i;
            lineIndexPtr->index1 = 1 + slices + (i + 1)%slices;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = numVertices - 1;
            lineIndexPtr->index1 = 1 + slices + i;
        }
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = (i * 5) * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + (i + 1)%slices;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + slices + i;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + slices + i;
            lineIndexPtr->index1 = 1 + slices + (i + 1)%slices;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = numVertices - 1;
            lineIndexPtr->index1 = 1 + slices + i;
        }
    }

    return true;
}

// ******************* //
bool                        YfBuildCapsuleVertices
(
    float                   radius, 
    float                   height, 
    unsigned int            slices,
    unsigned int            stacks, 
    YeOriginPose            originPose, 
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    unsigned int halfStacks = stacks / 2;
    if (slices < 2 || halfStacks < 2 || !pVerticesBuffer)
    {
        return false;
    }

    unsigned int numVertices     = slices * (halfStacks - 1) * 2 + 2;
    unsigned int numHalfVertices = numVertices / 2;

    // 顶点赋值
    char* vertexPtr   = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;

    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -radius - height*0.5f;
    }
    else if (originPose == YE_ORIGIN_POSE_BOTTOM)
    {
        originOffsetY = radius + height*0.5f;
    }

    float halfHeight = height * 0.5f;
    float tallness = radius*2 + height;        // 胶囊体真正高度

    // 最高顶点
    {
        nOffset = 0 * vertexStriding;            
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = halfHeight + radius + originOffsetY;
        curVertexPtr->z = 0.0f;
    }
    // 最底顶点
    {
        nOffset = numHalfVertices * vertexStriding;            
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = -halfHeight - radius + originOffsetY;
        curVertexPtr->z = 0.0f;
    }

    float* pSinList = (float*)malloc(sizeof(float)*slices);
    float* pCosList = (float*)malloc(sizeof(float)*slices);
    float angleXZ;
    for (unsigned int j = 0; j < slices; j++)
    {
        angleXZ = YD_REAL_TWAIN_PI * j / slices;
        pSinList[j] = sinf(angleXZ);
        pCosList[j] = cosf(angleXZ);
    }

    for (unsigned int i = 1; i < halfStacks; i++)
    {
        float angleY = YD_REAL_HALF_PI * i / (halfStacks - 1);
        float posY = radius * cosf(angleY);
        float radiusXZ = radius * sinf(angleY);
        float posX, posZ;

        for (unsigned int j = 0; j < slices; j++)
        {
            posX = radiusXZ * pSinList[j % slices];
            posZ = radiusXZ * pCosList[j % slices];

            nOffset = (1 + (i - 1) * slices + j) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = posX;
            curVertexPtr->y = posY + halfHeight + originOffsetY;
            curVertexPtr->z = posZ;
  
            nOffset = (numHalfVertices + 1 + (i - 1) * slices + j) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = posX;
            curVertexPtr->y = -posY - halfHeight + originOffsetY;
            curVertexPtr->z = posZ;
        }
    }

    free(pSinList);
    free(pCosList);

    return true;
}     

bool                        YfBuildCapsuleTriIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos, 
    void*                   pTriIndicesBuffer
)
{
    unsigned int halfStacks = stacks / 2;
    if (slices < 2 || halfStacks < 2 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * (halfStacks - 1) * 2 + 2;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }
    unsigned int numHalfVertices = numVertices / 2;
    //unsigned int numTriangles    = slices * (halfStacks - 1) * 4;

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;

        for (unsigned int i = 0; i < halfStacks - 1; i++)
        {
            if (i == 0)                                 // 第一层
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = j * 2 * indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 0;
                    triIndexPtr->index1 = 1 + j;
                    triIndexPtr->index2 = 1 + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = numHalfVertices;
                    triIndexPtr->index1 = numHalfVertices + 1 + (j + 1)%slices;
                    triIndexPtr->index2 = numHalfVertices + 1 + j;
                }
            }
            else
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = ((i - 1)*slices*4 + slices*2 + j*4) * indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * (i - 1) + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + (j + 1)%slices;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * i + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = numHalfVertices + 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = numHalfVertices + 1 + slices * (i - 1) + (j + 1)%slices;
                    triIndexPtr->index2 = numHalfVertices + 1 + slices * i + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = numHalfVertices + 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = numHalfVertices + 1 + slices * i + (j + 1)%slices;
                    triIndexPtr->index2 = numHalfVertices + 1 + slices * i + j;
                }
            }
        }

        // 连接两个半球
        for (unsigned int j = 0; j < slices; j++)
        {
            nOffset = ((halfStacks - 2)*slices*4 + slices*2 + j*2) * indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + slices  * (halfStacks - 2) + j;
            triIndexPtr->index1 = numHalfVertices + 1 + slices * (halfStacks - 2) + j;
            triIndexPtr->index2 = 1 + slices * (halfStacks - 2) + (j + 1)%slices;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + slices * (halfStacks - 2) + (j + 1)%slices;
            triIndexPtr->index1 = numHalfVertices + 1 + slices * (halfStacks - 2) + j;
            triIndexPtr->index2 = numHalfVertices + 1 + slices * (halfStacks - 2) + (j + 1)%slices;
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;

        for (unsigned int i = 0; i < halfStacks - 1; i++)
        {
            if (i == 0)                                 // 第一层
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = j * 2 * indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 0;
                    triIndexPtr->index1 = 1 + j;
                    triIndexPtr->index2 = 1 + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = numHalfVertices;
                    triIndexPtr->index1 = numHalfVertices + 1 + (j + 1)%slices;
                    triIndexPtr->index2 = numHalfVertices + (j + 1)%slices;
                }
            }
            else
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = ((i - 1)*slices*4 + slices*2 + j*4) * indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * (i - 1) + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + (j + 1)%slices;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * i + j + 1;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = numHalfVertices + 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = numHalfVertices + 1 + slices * (i - 1) + (j + 1)%slices;
                    triIndexPtr->index2 = numHalfVertices + 1 + slices * i + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = numHalfVertices + 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = numHalfVertices + 1 + slices * i + (j + 1)%slices;
                    triIndexPtr->index2 = numHalfVertices + 1 + slices * i + j;
                }
            }
        }

        // 连接两个半球
        for (unsigned int j = 0; j < slices; j++)
        {
            nOffset = ((halfStacks - 2)*slices*4 + slices*2 + j*2) * indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + slices  * (halfStacks - 2) + j;
            triIndexPtr->index1 = numHalfVertices + 1 + slices * (halfStacks - 2) + j;
            triIndexPtr->index2 = 1 + slices * (halfStacks - 2) + (j + 1)%slices;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + slices * (halfStacks - 2) + (j + 1)%slices;
            triIndexPtr->index1 = numHalfVertices + 1 + slices * (halfStacks - 2) + j;
            triIndexPtr->index2 = numHalfVertices + 1 + slices * (halfStacks - 2) + (j + 1)%slices;
        }
    }

    return true;
}     

bool                        YfBuildCapsuleWireIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos, 
    void*                   pWireIndicesBuffer
)
{
    unsigned int halfStacks = stacks / 2;
    if (slices < 2 || halfStacks < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * (halfStacks - 1) * 2 + 2;
    //unsigned int numLines     = slices * (halfStacks - 1) * 2 + slices * (2 * halfStacks - 1);
    unsigned int numHalfVertices = numVertices / 2;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        for (unsigned int i = 1; i < halfStacks; i++)
        {
            for (unsigned int j = 0; j < slices; j++)
            {
                nOffset = ((i - 1)*slices + j) * 2 * indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + (i - 1)*slices + j;
                lineIndexPtr->index1 = 1 + (i - 1)*slices + (j + 1)%slices;

                nOffset += indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                lineIndexPtr->index0 = numHalfVertices + 1 + (i - 1)*slices + j;
                lineIndexPtr->index1 = numHalfVertices + 1 + (i - 1)*slices + (j + 1)%slices;
            }
        }

        unsigned int half = (halfStacks - 1) * slices *2;
        for (unsigned int j = 0; j < slices; j++)
        {
            for (unsigned int i = 0; i < halfStacks - 1; i++)
            {
                if (i == 0)
                {
                    nOffset = (half + (halfStacks*2 - 1) * j) * indexStriding;
                    lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                    lineIndexPtr->index0 = 0;
                    lineIndexPtr->index1 = 1 + j;

                    nOffset += indexStriding;
                    lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                    lineIndexPtr->index0 = numHalfVertices;
                    lineIndexPtr->index1 = numHalfVertices + 1 + j;
                }
                else
                {
                    nOffset = (half + (halfStacks*2 - 1) * j + i * 2) * indexStriding;
                    lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                    lineIndexPtr->index0 = 1 + slices*(i - 1) + j;
                    lineIndexPtr->index1 = 1 + slices*i + j;

                    nOffset += indexStriding;
                    lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                    lineIndexPtr->index0 = numHalfVertices + 1 + slices*(i - 1) + j;
                    lineIndexPtr->index1 = numHalfVertices + 1 + slices*i + j;                    
                }
            }

            nOffset = (half + (halfStacks*2 - 1) * (j + 1) - 1) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + slices  * (halfStacks - 2) + j;
            lineIndexPtr->index1 = numHalfVertices + 1 + slices * (halfStacks - 2) + j;
        }
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            YsLineIndex32* lineIndexPtr = 0;
            for (unsigned int i = 1; i < halfStacks; i++)
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = ((i - 1)*slices + j) * 2 * indexStriding;
                    lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                    lineIndexPtr->index0 = 1 + (i - 1)*slices + j;
                    lineIndexPtr->index1 = 1 + (i - 1)*slices + (j + 1)%slices;

                    nOffset += indexStriding;
                    lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                    lineIndexPtr->index0 = numHalfVertices + 1 + (i - 1)*slices + j;
                    lineIndexPtr->index1 = numHalfVertices + 1 + (i - 1)*slices + (j + 1)%slices;
                }
            }

            unsigned int half = (halfStacks - 1) * slices *2;
            for (unsigned int j = 0; j < slices; j++)
            {
                for (unsigned int i = 0; i < halfStacks - 1; i++)
                {
                    if (i == 0)
                    {
                        nOffset = (half + (halfStacks*2 - 1) * j) * indexStriding;
                        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                        lineIndexPtr->index0 = 0;
                        lineIndexPtr->index1 = 1 + j;

                        nOffset += indexStriding;
                        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                        lineIndexPtr->index0 = numHalfVertices;
                        lineIndexPtr->index1 = numHalfVertices + 1 + j;
                    }
                    else
                    {
                        nOffset = (half + (halfStacks*2 - 1) * j + i * 2) * indexStriding;
                        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                        lineIndexPtr->index0 = 1 + slices*(i - 1) + j;
                        lineIndexPtr->index1 = 1 + slices*i + j;

                        nOffset += indexStriding;
                        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                        lineIndexPtr->index0 = numHalfVertices + 1 + slices*(i - 1) + j;
                        lineIndexPtr->index1 = numHalfVertices + 1 + slices*i + j;                    
                    }
                }

                nOffset = (half + (halfStacks*2 - 1) * (j + 1) - 1) * indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + slices  * (halfStacks - 2) + j;
                lineIndexPtr->index1 = numHalfVertices + 1 + slices * (halfStacks - 2) + j;
            }
        }
    }

    return true;
}

// ******************* //
bool                        YfBuildPyramidVertices
(
    float                   width, 
    float                   length, 
    float                   height, 
    YeOriginPose            originPose, 
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (!pVerticesBuffer)
    {
        return false;
    }

    //unsigned int numVertices  = 5;

    char* vertexPtr   = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    YsVector3 vOriginOffset(-width / 2, -height / 2, -length / 2);
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        vOriginOffset.y = -height;
    }
    else if (originPose == YE_ORIGIN_POSE_BOTTOM)
    {
        vOriginOffset.y = 0.0f;
    }

    // 底四个顶点
    for (unsigned int j = 0; j < 2; j++)
    {
        for (unsigned int i = 0; i < 2; i++)
        {
            nOffset = (j*2 + i) * vertexStriding;
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = width*i + vOriginOffset.x;
            curVertexPtr->y = vOriginOffset.y;
            curVertexPtr->z = length*j + vOriginOffset.z;
        }
    }

    // 顶尖顶点
    nOffset = 4 * vertexStriding;
    curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
    curVertexPtr->x = 0.0f;
    curVertexPtr->y = height + vOriginOffset.y;
    curVertexPtr->z = 0.0f;
 
    return true;
}     

bool                        YfBuildPyramidTriIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (!pTriIndicesBuffer)
    {
        return false;
    }

    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;

        // bottom
        triIndexPtr = (YsTriIndex16*)(indexPtr + 0*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 1; triIndexPtr->index2 = 3;
        triIndexPtr = (YsTriIndex16*)(indexPtr + 1*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 3; triIndexPtr->index2 = 2;

        // 4 face
        triIndexPtr = (YsTriIndex16*)(indexPtr + 2*indexStriding);
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 1; triIndexPtr->index2 = 0;
        triIndexPtr = (YsTriIndex16*)(indexPtr + 3*indexStriding);
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 3; triIndexPtr->index2 = 1;
        triIndexPtr = (YsTriIndex16*)(indexPtr + 4*indexStriding);
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 2; triIndexPtr->index2 = 3;
        triIndexPtr = (YsTriIndex16*)(indexPtr + 5*indexStriding);                                                
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 0; triIndexPtr->index2 = 2;
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;

        // bottom
        triIndexPtr = (YsTriIndex32*)(indexPtr + 0*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 1; triIndexPtr->index2 = 3;
        triIndexPtr = (YsTriIndex32*)(indexPtr + 1*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 3; triIndexPtr->index2 = 2;

        // 4 face
        triIndexPtr = (YsTriIndex32*)(indexPtr + 2*indexStriding);
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 1; triIndexPtr->index2 = 0;
        triIndexPtr = (YsTriIndex32*)(indexPtr + 3*indexStriding);
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 3; triIndexPtr->index2 = 1;
        triIndexPtr = (YsTriIndex32*)(indexPtr + 4*indexStriding);
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 2; triIndexPtr->index2 = 3;
        triIndexPtr = (YsTriIndex32*)(indexPtr + 5*indexStriding);                                                
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 0; triIndexPtr->index2 = 2;
    }

    return true;
}     

bool                        YfBuildPyramidWireIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding, 
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (!pWireIndicesBuffer)
    {
        return false;
    }

    //unsigned int numVertices = 5;
    //unsigned int numLines    = 8;

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        
        // bottom
        lineIndexPtr = (YsLineIndex16*)(indexPtr + 0*indexStriding);
        lineIndexPtr->index0 = 0; lineIndexPtr->index1 = 1; 
        lineIndexPtr = (YsLineIndex16*)(indexPtr + 1*indexStriding);
        lineIndexPtr->index0 = 1; lineIndexPtr->index1 = 3; 
        lineIndexPtr = (YsLineIndex16*)(indexPtr + 2*indexStriding);
        lineIndexPtr->index0 = 3; lineIndexPtr->index1 = 2; 
        lineIndexPtr = (YsLineIndex16*)(indexPtr + 3*indexStriding);
        lineIndexPtr->index0 = 2; lineIndexPtr->index1 = 0; 

        // 4 lines
        lineIndexPtr = (YsLineIndex16*)(indexPtr + 4*indexStriding);
        lineIndexPtr->index0 = 4; lineIndexPtr->index1 = 0; 
        lineIndexPtr = (YsLineIndex16*)(indexPtr + 5*indexStriding);
        lineIndexPtr->index0 = 4; lineIndexPtr->index1 = 1; 
        lineIndexPtr = (YsLineIndex16*)(indexPtr + 6*indexStriding);
        lineIndexPtr->index0 = 4; lineIndexPtr->index1 = 2; 
        lineIndexPtr = (YsLineIndex16*)(indexPtr + 7*indexStriding);
        lineIndexPtr->index0 = 4; lineIndexPtr->index1 = 3; 
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;

        // bottom
        lineIndexPtr = (YsLineIndex32*)(indexPtr + 0*indexStriding);
        lineIndexPtr->index0 = 0; lineIndexPtr->index1 = 1; 
        lineIndexPtr = (YsLineIndex32*)(indexPtr + 1*indexStriding);
        lineIndexPtr->index0 = 1; lineIndexPtr->index1 = 3; 
        lineIndexPtr = (YsLineIndex32*)(indexPtr + 2*indexStriding);
        lineIndexPtr->index0 = 3; lineIndexPtr->index1 = 2; 
        lineIndexPtr = (YsLineIndex32*)(indexPtr + 3*indexStriding);
        lineIndexPtr->index0 = 2; lineIndexPtr->index1 = 0; 

        // 4 lines
        lineIndexPtr = (YsLineIndex32*)(indexPtr + 4*indexStriding);
        lineIndexPtr->index0 = 4; lineIndexPtr->index1 = 0; 
        lineIndexPtr = (YsLineIndex32*)(indexPtr + 5*indexStriding);
        lineIndexPtr->index0 = 4; lineIndexPtr->index1 = 1; 
        lineIndexPtr = (YsLineIndex32*)(indexPtr + 6*indexStriding);
        lineIndexPtr->index0 = 4; lineIndexPtr->index1 = 2; 
        lineIndexPtr = (YsLineIndex32*)(indexPtr + 7*indexStriding);
        lineIndexPtr->index0 = 4; lineIndexPtr->index1 = 3; 
    }

    return true;
}

// ******************* //
bool                        YfBuildRoundVertices
(
    float                   radius, 
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose, 
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (slices < 3 || !pVerticesBuffer)
    {
        return false;
    }

    //unsigned int numVertices  = slices + 1;

    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    // 圆心顶点赋值
    {
        nOffset = 0;   
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = height;
        curVertexPtr->z = 0.0f;
    }

    float angleXZ;
    float posX, posZ;        
    for (unsigned int i = 0; i < slices; i++)
    {
        angleXZ = YD_REAL_TWAIN_PI * i / slices;
        posX = sinf(angleXZ);
        posZ = cosf(angleXZ);

        nOffset = (i + 1) * vertexStriding; 
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = radius * posX;
        curVertexPtr->y = height;
        curVertexPtr->z = radius * posZ;
    }

    return true;
}     

bool                        YfBuildRoundTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (slices < 3 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices + 1;
    //unsigned int numTriangles = slices;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = i * indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + (i + 1)%slices;
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = i * indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + (i + 1)%slices;
        }
    }

    return true;
}     

bool                        YfBuildRoundWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices = slices + 1;
    //unsigned int numLines    = slices;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = i * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + (i + 1)%slices;
        }
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            nOffset = i * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + (i + 1)%slices;
        }
    }

    return true;
}

// ******************* //
bool                        YfBuildRingVertices
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices,
    unsigned int            stacks, 
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (slices < 2 || stacks < 3 || !pVerticesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * stacks;
    //unsigned int numTriangles = slices * stacks * 2;

    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr = 0;
    unsigned int nOffset = 0;

    float halfHeight = height * 0.5f;
    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -halfHeight;
    }
    else if (originPose == YE_ORIGIN_POSE_BOTTOM)
    {
        originOffsetY = halfHeight;
    }

    float angle, s, c;
    YsVector3* initVerticesPtr = (YsVector3*)malloc(sizeof(YsVector3) * (slices + 1));
    for (unsigned int j = 0; j < slices; j++)
    {
        angle = YD_REAL_TWAIN_PI * j / slices;
        s = sinf(angle);
        c = cosf(angle);
        initVerticesPtr[j].x = radius + assistRadius*s;
        initVerticesPtr[j].y = halfHeight*c + originOffsetY;
        initVerticesPtr[j].z = 0.0f;
    }

    for (unsigned int i = 0; i < stacks; i++)
    {
        angle = YD_REAL_TWAIN_PI * i / stacks;
        s = sinf(angle);
        c = cosf(angle);

        for (unsigned int j = 0; j < slices; j++)
        {
            nOffset = (i * slices + j) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = initVerticesPtr[j].x * s;
            curVertexPtr->y = initVerticesPtr[j].y;
            curVertexPtr->z = initVerticesPtr[j].x * c;
        }
    }

    free(initVerticesPtr);

    return true;
}     

bool                        YfBuildRingTriIndices
(
    unsigned int            slices,
    unsigned int            stacks,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (slices < 2 || stacks < 3 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * stacks;
    //unsigned int numTriangles = slices * stacks * 2;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            for (unsigned int j = 0; j < slices; j++)
            {
                nOffset = (i*slices + j)*2 * indexStriding;
                triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                triIndexPtr->index0 = slices * i + j;
                triIndexPtr->index1 = slices * i + (j + 1)%slices;
                triIndexPtr->index2 = slices * ((i + 1) % stacks) + j;

                nOffset += indexStriding;
                triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                triIndexPtr->index0 = slices * i + (j + 1)%slices;
                triIndexPtr->index1 = slices * ((i + 1) % stacks) + (j + 1)%slices;
                triIndexPtr->index2 = slices * ((i + 1) % stacks) + j;
            }
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            for (unsigned int j = 0; j < slices; j++)
            {
                nOffset = (i*slices + j)*2 * indexStriding;
                triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                triIndexPtr->index0 = slices * i + j;
                triIndexPtr->index1 = slices * i + (j + 1)%slices;
                triIndexPtr->index2 = slices * ((i + 1) % stacks) + j;

                nOffset += indexStriding;
                triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                triIndexPtr->index0 = slices * i + (j + 1)%slices;
                triIndexPtr->index1 = slices * ((i + 1) % stacks) + (j + 1)%slices;
                triIndexPtr->index2 = slices * ((i + 1) % stacks) + j;
            }
        }
    }

    return true;
}   

bool                        YfBuildRingWireIndices
(
    unsigned int            slices,
    unsigned int            stacks,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices = slices * stacks;
    //unsigned int numLines    = slices * stacks * 2;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            for (unsigned int j = 0; j < slices; j++)
            {
                nOffset = (i*slices + j) * 2 * indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                lineIndexPtr->index0 = slices * i + j;
                lineIndexPtr->index1 = slices * i + (j + 1)%slices;

                nOffset += indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                lineIndexPtr->index0 = slices * i + j;
                lineIndexPtr->index1 = slices * ((i + 1) % stacks) + j;
            }
        }
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            for (unsigned int j = 0; j < slices; j++)
            {
                nOffset = (i*slices + j) * 2 * indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = slices * i + j;
                lineIndexPtr->index1 = slices * i + (j + 1)%slices;

                nOffset += indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = slices * i + j;
                lineIndexPtr->index1 = slices * ((i + 1) % stacks) + j;
            }
        }
    }

    return true;
}

// ******************* //
bool                        YfBuildPipeVertices
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose, 
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (slices < 2 || !pVerticesBuffer)
    {
        return false;
    }

    //unsigned int numVertices  = slices * 4;

    // 顶点赋值
    char* vertexPtr   = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -height;
    }
    else if (originPose == YE_ORIGIN_POSE_CENTER)
    {
        originOffsetY = -height * 0.5f;
    }

    float angleXZ;
    float posX, posZ;        
    for (unsigned int i = 0; i < slices; i++)
    {
        angleXZ = YD_REAL_TWAIN_PI * i / slices;
        posX = sinf(angleXZ);
        posZ = cosf(angleXZ);

        // 上顶点
        {
            // 外
            nOffset = (i * 2) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = radius * posX;
            curVertexPtr->y = height + originOffsetY;
            curVertexPtr->z = radius * posZ;

            // 内
            nOffset += vertexStriding;
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = assistRadius * posX;
            curVertexPtr->y = height + originOffsetY;
            curVertexPtr->z = assistRadius * posZ;
        }

        // 下顶点
        {
            // 外
            nOffset = (slices * 2 + i * 2) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = radius * posX;
            curVertexPtr->y = originOffsetY;
            curVertexPtr->z = radius * posZ;

            // 内
            nOffset += vertexStriding;
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = assistRadius * posX;
            curVertexPtr->y = originOffsetY;
            curVertexPtr->z = assistRadius * posZ;
        }
    }

    return true;
}     

bool                        YfBuildPipeTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (slices < 3 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * 4;
    //unsigned int numTriangles = slices * 8;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    unsigned int halfVtxNum = numVertices / 2;

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;
        for (unsigned int i = 0; i < slices; i++)
        {
            // 外面
            nOffset = (i * 8) * indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i * 2;
            triIndexPtr->index1 = halfVtxNum + i * 2;
            triIndexPtr->index2 = (i + 1) % slices * 2;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = (i + 1) % slices * 2;
            triIndexPtr->index1 = halfVtxNum + i * 2;
            triIndexPtr->index2 = halfVtxNum + (i + 1) % slices * 2;

            // 里面
            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i * 2 + 1;
            triIndexPtr->index1 = (i + 1) % slices * 2 + 1;
            triIndexPtr->index2 = halfVtxNum + i * 2 + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = (i + 1) % slices * 2 + 1;
            triIndexPtr->index1 = halfVtxNum + (i + 1) % slices * 2 + 1;
            triIndexPtr->index2 = halfVtxNum + i * 2 + 1;

            // 上面
            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i * 2;
            triIndexPtr->index1 = (i + 1) % slices * 2 + 1;
            triIndexPtr->index2 = i * 2 + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i * 2;
            triIndexPtr->index1 = (i + 1) % slices * 2;
            triIndexPtr->index2 = (i + 1) % slices * 2 + 1;

            // 下面
            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = halfVtxNum + i * 2;
            triIndexPtr->index1 = halfVtxNum + i * 2 + 1;
            triIndexPtr->index2 = halfVtxNum + (i + 1) % slices * 2 + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = halfVtxNum + i * 2;
            triIndexPtr->index1 = halfVtxNum + (i + 1) % slices * 2 + 1;
            triIndexPtr->index2 = halfVtxNum + (i + 1) % slices * 2;
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;
        for (unsigned int i = 0; i < slices; i++)
        {
            // 外面
            nOffset = (i * 8) * indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i * 2;
            triIndexPtr->index1 = halfVtxNum + i * 2;
            triIndexPtr->index2 = (i + 1) % slices * 2;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = (i + 1) % slices * 2;
            triIndexPtr->index1 = halfVtxNum + i * 2;
            triIndexPtr->index2 = halfVtxNum + (i + 1) % slices * 2;

            // 里面
            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i * 2 + 1;
            triIndexPtr->index1 = (i + 1) % slices * 2 + 1;
            triIndexPtr->index2 = halfVtxNum + i * 2 + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = (i + 1) % slices * 2 + 1;
            triIndexPtr->index1 = halfVtxNum + (i + 1) % slices * 2 + 1;
            triIndexPtr->index2 = halfVtxNum + i * 2 + 1;

            // 上面
            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i * 2;
            triIndexPtr->index1 = (i + 1) % slices * 2 + 1;
            triIndexPtr->index2 = i * 2 + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i * 2;
            triIndexPtr->index1 = (i + 1) % slices * 2;
            triIndexPtr->index2 = (i + 1) % slices * 2 + 1;

            // 下面
            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = halfVtxNum + i * 2;
            triIndexPtr->index1 = halfVtxNum + i * 2 + 1;
            triIndexPtr->index2 = halfVtxNum + (i + 1) % slices * 2 + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = halfVtxNum + i * 2;
            triIndexPtr->index1 = halfVtxNum + (i + 1) % slices * 2 + 1;
            triIndexPtr->index2 = halfVtxNum + (i + 1) % slices * 2;
        }
    }

    return true;
}   

bool                        YfBuildPipeWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices = slices * 4;
    //unsigned int numLines    = slices * 8;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    unsigned int halfVtxNum = numVertices / 2;

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            // 外面
            nOffset = (i * 8) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2;
            lineIndexPtr->index1 = halfVtxNum + i * 2;

            // 里面
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2 + 1;
            lineIndexPtr->index1 = halfVtxNum + i * 2 + 1;

            // 上面
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2;
            lineIndexPtr->index1 = i * 2 + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2;
            lineIndexPtr->index1 = (i + 1) % slices * 2;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2 + 1;
            lineIndexPtr->index1 = (i + 1) % slices * 2 + 1;

            // 下面
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = halfVtxNum + i * 2;
            lineIndexPtr->index1 = halfVtxNum + i * 2 + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = halfVtxNum + i * 2;
            lineIndexPtr->index1 = halfVtxNum + (i + 1) % slices * 2;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = halfVtxNum + i * 2 + 1;
            lineIndexPtr->index1 = halfVtxNum + (i + 1) % slices * 2 + 1;
        }
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            // 外面
            nOffset = (i * 8) * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2;
            lineIndexPtr->index1 = halfVtxNum + i * 2;

            // 里面
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2 + 1;
            lineIndexPtr->index1 = halfVtxNum + i * 2 + 1;

            // 上面
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2;
            lineIndexPtr->index1 = i * 2 + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2;
            lineIndexPtr->index1 = (i + 1) % slices * 2;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2 + 1;
            lineIndexPtr->index1 = (i + 1) % slices * 2 + 1;

            // 下面
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = halfVtxNum + i * 2;
            lineIndexPtr->index1 = halfVtxNum + i * 2 + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = halfVtxNum + i * 2;
            lineIndexPtr->index1 = halfVtxNum + (i + 1) % slices * 2;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = halfVtxNum + i * 2 + 1;
            lineIndexPtr->index1 = halfVtxNum + (i + 1) % slices * 2 + 1;
        }
    }

    return true;
}

// ******************* //
bool                        YfBuildWedgeVertices
(
    float                   width, 
    float                   length, 
    float                   height, 
    YeOriginPose            originPose,  
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (!pVerticesBuffer)
    {
        return false;
    }

    //unsigned int numVertices  = 6;

    // 顶点赋值
    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    YsVector3 vOriginOffset(-width / 2, -height / 2, -length / 2);
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        vOriginOffset.y = -height;
    }
    else if (originPose == YE_ORIGIN_POSE_BOTTOM)
    {
        vOriginOffset.y = 0.0f;
    }

    // 底四个顶点
    for (unsigned int j = 0; j < 2; j++)
    {
        for (unsigned int i = 0; i < 2; i++)
        {
            nOffset = (j*2 + i) * vertexStriding;
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = width*i + vOriginOffset.x;
            curVertexPtr->y = vOriginOffset.y;
            curVertexPtr->z = length*j + vOriginOffset.z;
        }
    }

    // 上顶点
    nOffset = 4 * vertexStriding;
    if (vertexPtr)
    {
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = vOriginOffset.x + width / 2;
        curVertexPtr->y = height + vOriginOffset.y;
        curVertexPtr->z = vOriginOffset.z;

        curVertexPtr = (YsVector3*)(vertexPtr + nOffset + vertexStriding);
        curVertexPtr->x = vOriginOffset.x + width / 2;
        curVertexPtr->y = height + vOriginOffset.y;
        curVertexPtr->z = length + vOriginOffset.z;
    }

    return true;
}     

bool                        YfBuildWedgeTriIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (!pTriIndicesBuffer)
    {
        return false;
    }

    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;

        // bottom
        triIndexPtr = (YsTriIndex16*)(indexPtr + 0*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 1; triIndexPtr->index2 = 3;
        triIndexPtr = (YsTriIndex16*)(indexPtr + 1*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 3; triIndexPtr->index2 = 2;

        // 4 face
        triIndexPtr = (YsTriIndex16*)(indexPtr + 2*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 2; triIndexPtr->index2 = 5;
        triIndexPtr = (YsTriIndex16*)(indexPtr + 3*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 5; triIndexPtr->index2 = 4;
        triIndexPtr = (YsTriIndex16*)(indexPtr + 4*indexStriding);
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 5; triIndexPtr->index2 = 3;
        triIndexPtr = (YsTriIndex16*)(indexPtr + 5*indexStriding);                                                
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 3; triIndexPtr->index2 = 1;

        // 2 face
        triIndexPtr = (YsTriIndex16*)(indexPtr + 6*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 4; triIndexPtr->index2 = 1;
        triIndexPtr = (YsTriIndex16*)(indexPtr + 7*indexStriding);
        triIndexPtr->index0 = 2; triIndexPtr->index1 = 3; triIndexPtr->index2 = 5;
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;

        // bottom
        triIndexPtr = (YsTriIndex32*)(indexPtr + 0*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 1; triIndexPtr->index2 = 3;
        triIndexPtr = (YsTriIndex32*)(indexPtr + 1*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 3; triIndexPtr->index2 = 2;

        // 4 face
        triIndexPtr = (YsTriIndex32*)(indexPtr + 2*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 2; triIndexPtr->index2 = 5;
        triIndexPtr = (YsTriIndex32*)(indexPtr + 3*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 5; triIndexPtr->index2 = 4;
        triIndexPtr = (YsTriIndex32*)(indexPtr + 4*indexStriding);
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 5; triIndexPtr->index2 = 3;
        triIndexPtr = (YsTriIndex32*)(indexPtr + 5*indexStriding);                                                
        triIndexPtr->index0 = 4; triIndexPtr->index1 = 3; triIndexPtr->index2 = 1;

        // 2 face
        triIndexPtr = (YsTriIndex32*)(indexPtr + 6*indexStriding);
        triIndexPtr->index0 = 0; triIndexPtr->index1 = 4; triIndexPtr->index2 = 1;
        triIndexPtr = (YsTriIndex32*)(indexPtr + 7*indexStriding);
        triIndexPtr->index0 = 2; triIndexPtr->index1 = 3; triIndexPtr->index2 = 5;
    }

    return true;
}   

bool                        YfBuildWedgeWireIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (!pWireIndicesBuffer)
    {
        return false;
    }

    //unsigned int numVertices = 6;
    //unsigned int numLines    = 9;

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;

        nOffset = 0 * indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 0;
        lineIndexPtr->index1 = 1;

        nOffset = 1 * indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 0;
        lineIndexPtr->index1 = 4;

        nOffset = 2 * indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 4;
        lineIndexPtr->index1 = 1;

        nOffset = 3 * indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 2;
        lineIndexPtr->index1 = 3;

        nOffset = 4 * indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 2;
        lineIndexPtr->index1 = 5;

        nOffset = 5 * indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 5;
        lineIndexPtr->index1 = 3;

        nOffset = 6 * indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 0;
        lineIndexPtr->index1 = 2;

        nOffset = 7 * indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 4;
        lineIndexPtr->index1 = 5;

        nOffset = 8 * indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 1;
        lineIndexPtr->index1 = 3;
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;

        nOffset = 0 * indexStriding;
        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 0;
        lineIndexPtr->index1 = 1;

        nOffset = 1 * indexStriding;
        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 0;
        lineIndexPtr->index1 = 4;

        nOffset = 2 * indexStriding;
        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 4;
        lineIndexPtr->index1 = 1;

        nOffset = 3 * indexStriding;
        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 2;
        lineIndexPtr->index1 = 3;

        nOffset = 4 * indexStriding;
        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 2;
        lineIndexPtr->index1 = 5;

        nOffset = 5 * indexStriding;
        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 5;
        lineIndexPtr->index1 = 3;

        nOffset = 6 * indexStriding;
        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 0;
        lineIndexPtr->index1 = 2;

        nOffset = 7 * indexStriding;
        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 4;
        lineIndexPtr->index1 = 5;

        nOffset = 8 * indexStriding;
        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 1;
        lineIndexPtr->index1 = 3;
    }

    return true;
}

// ******************* //
bool                        YfBuildFunVertices
(
    float                   radius, 
    float                   degree, 
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose,
    unsigned int            vertexStriding,
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (degree < 0 || degree > 360 || !pVerticesBuffer)
    {
        return false;
    }
    if (slices < 2 || !pVerticesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = (slices + 1) * 2;

    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -height;
    }
    else if (originPose == YE_ORIGIN_POSE_CENTER)
    {
        originOffsetY = -height * 0.5f;
    }

    // 柱顶顶点赋值
    {
        nOffset = 0;   
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = height + originOffsetY;
        curVertexPtr->z = 0.0f;
    }

    // 柱底顶点赋值
    {
        nOffset = (numVertices - 1) * vertexStriding;   
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = originOffsetY;
        curVertexPtr->z = 0.0f;
    }

    float radian = YD_DEGREE_TO_RADIAN(degree);
    float angleXZ;
    float posX, posZ;        
    for (unsigned int i = 0; i < slices; i++)
    {
        angleXZ = radian * i / (slices - 1);
        posX = sinf(angleXZ);
        posZ = cosf(angleXZ);

        // 上顶点
        {
            nOffset = (1 + i) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = radius * posX;
            curVertexPtr->y = height + originOffsetY;
            curVertexPtr->z = radius * posZ;
        }

        // 下顶点
        {
            nOffset = (1 + slices + i) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = radius * posX;
            curVertexPtr->y = originOffsetY;
            curVertexPtr->z = radius * posZ;
        }
    }

    return true;
}     

bool                        YfBuildFunTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (slices < 2 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = (slices + 1) * 2;
    //unsigned int numTriangles = slices * 4;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;
        for (unsigned int i = 0; i < slices - 1; i++)
        {
            nOffset = (i * 4) * indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + i + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = numVertices - 1;
            triIndexPtr->index1 = 1 + slices + i + 1;
            triIndexPtr->index2 = 1 + slices + i;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + i;
            triIndexPtr->index1 = 1 + slices + i;
            triIndexPtr->index2 = 1 + i + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + i + 1;
            triIndexPtr->index1 = 1 + slices + i;
            triIndexPtr->index2 = 1 + slices + i + 1;
        }

        nOffset = ((slices - 1) * 4) * indexStriding;
        triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
        triIndexPtr->index0 = 0;
        triIndexPtr->index1 = numVertices - 1;
        triIndexPtr->index2 = 1;

        nOffset += indexStriding;
        triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
        triIndexPtr->index0 = 1;
        triIndexPtr->index1 = numVertices - 1;
        triIndexPtr->index2 = 1 + slices;

        nOffset += indexStriding;
        triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
        triIndexPtr->index0 = 0;
        triIndexPtr->index1 = slices;
        triIndexPtr->index2 = numVertices - 1;

        nOffset += indexStriding;
        triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
        triIndexPtr->index0 = slices;
        triIndexPtr->index1 = numVertices - 2;
        triIndexPtr->index2 = numVertices - 1;
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;
        for (unsigned int i = 0; i < slices - 1; i++)
        {
            nOffset = (i * 4) * indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + i + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = numVertices - 1;
            triIndexPtr->index1 = 1 + slices + i + 1;
            triIndexPtr->index2 = 1 + slices + i;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + i;
            triIndexPtr->index1 = 1 + slices + i;
            triIndexPtr->index2 = 1 + i + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + i + 1;
            triIndexPtr->index1 = 1 + slices + i;
            triIndexPtr->index2 = 1 + slices + i + 1;
        }

        nOffset = ((slices - 1) * 4) * indexStriding;
        triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
        triIndexPtr->index0 = 0;
        triIndexPtr->index1 = numVertices - 1;
        triIndexPtr->index2 = 1;

        nOffset += indexStriding;
        triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
        triIndexPtr->index0 = 1;
        triIndexPtr->index1 = numVertices - 1;
        triIndexPtr->index2 = 1 + slices;

        nOffset += indexStriding;
        triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
        triIndexPtr->index0 = 0;
        triIndexPtr->index1 = slices;
        triIndexPtr->index2 = numVertices - 1;

        nOffset += indexStriding;
        triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
        triIndexPtr->index0 = slices;
        triIndexPtr->index1 = numVertices - 2;
        triIndexPtr->index2 = numVertices - 1;
    }

    return true;
}   

bool                        YfBuildFunWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices = (slices + 1) * 2;
    //unsigned int numLines    = slices + (slices-1) * 2 + 5;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        for (unsigned int i = 0; i < slices - 1; i++)
        {
            nOffset = (i * 3) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + slices + i;
       
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + i + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + slices + i;
            lineIndexPtr->index1 = 1 + slices + i + 1;
        }

        nOffset += indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = slices;
        lineIndexPtr->index1 = 2*slices;

        nOffset += indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 0;
        lineIndexPtr->index1 = 2*slices + 1;

        nOffset += indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 0;
        lineIndexPtr->index1 = 1;

        nOffset += indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 0;
        lineIndexPtr->index1 = slices;

        nOffset += indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 2*slices + 1;
        lineIndexPtr->index1 = slices + 1;

        nOffset += indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = 2*slices + 1;
        lineIndexPtr->index1 = 2*slices;
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;
        for (unsigned int i= 0; i < slices; i++)
        {
            YsLineIndex32* lineIndexPtr = 0;
            for (unsigned int i = 0; i < slices - 1; i++)
            {
                nOffset = (i * 3) * indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + i;
                lineIndexPtr->index1 = 1 + slices + i;

                nOffset += indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + i;
                lineIndexPtr->index1 = 1 + i + 1;

                nOffset += indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + slices + i;
                lineIndexPtr->index1 = 1 + slices + i + 1;
            }

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = slices;
            lineIndexPtr->index1 = 2*slices;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 2*slices + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = slices;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 2*slices + 1;
            lineIndexPtr->index1 = slices + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 2*slices + 1;
            lineIndexPtr->index1 = 2*slices;
        }
    }

    return true;
}

// ******************* //
bool                        YfBuildArcVertices
(
    float                   radius, 
    float                   degree, 
    float                   height, 
    unsigned int            slices,
    unsigned int            stacks, 
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (degree < 0 || degree > 180 || !pVerticesBuffer)
    {
        return false;
    }
    if (slices < 2 || stacks < 3 || !pVerticesBuffer)
    {
        return false;
    }

    //unsigned int numVertices  = slices * (stacks - 1) + 1;

    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -radius;
    }
    else if (originPose == YE_ORIGIN_POSE_BOTTOM)
    {
        originOffsetY = radius;
    }

    float* pSinList = (float*)malloc(sizeof(float) * slices);
    float* pCosList = (float*)malloc(sizeof(float) * slices);
    float angleXZ;
    for (unsigned int j = 0; j < slices; j++)
    {
        angleXZ = YD_REAL_TWAIN_PI * j / slices;
        pSinList[j] = sinf(angleXZ);
        pCosList[j] = cosf(angleXZ);
    }

    // 赋值
    {
        float radian = YD_DEGREE_TO_RADIAN(degree);
        for (unsigned int i = 0; i < stacks; i++)
        {
            if (i == 0)                         // 第一个顶点
            {
                nOffset = 0;            
                curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
                curVertexPtr->x = 0.0f;
                curVertexPtr->y = radius + originOffsetY;
                curVertexPtr->z = 0.0f;
                continue;
            }

            float angleY = radian * i / (stacks - 1);
            float posY = radius * cosf(angleY);
            float radiusXZ = radius * sinf(angleY);
            float posX, posZ;

            for (unsigned int j = 0; j < slices; j++)
            {
                posX = radiusXZ * pSinList[j % slices];
                posZ = radiusXZ * pCosList[j % slices];

                nOffset = (1 + (i - 1) * slices + j) * vertexStriding; 
                curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
                curVertexPtr->x = posX;
                curVertexPtr->y = posY + originOffsetY;
                curVertexPtr->z = posZ;
            }
        }
    }

    free(pSinList);
    free(pCosList);

    return true;
}     

bool                        YfBuildArcTriIndices
(
    unsigned int            slices,
    unsigned int            stacks,
    YeIndexType             indexType,
    unsigned int            indexStriding, 
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (slices < 2 || stacks < 3 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices * (stacks - 1) + 1;
    //unsigned int numTriangles = slices * (stacks - 2) * 2 + slices;

    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;

        // 赋值
        for (unsigned int i = 0; i < stacks - 1; i++)
        {
            if (i == 0)                                 // 第一层
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = j * indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 0;
                    triIndexPtr->index1 = 1 + j;
                    triIndexPtr->index2 = 1 + (j + 1)%slices;
                }
            }
            else
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = ((i - 1)*slices * 2 + slices + j * 2) * indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * (i - 1) + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + (j + 1)%slices;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * i + (j + 1)%slices;
                }
            }
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;

        // 赋值
        for (unsigned int i = 0; i < stacks - 1; i++)
        {
            if (i == 0)                                 // 第一层
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = j * indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 0;
                    triIndexPtr->index1 = 1 + j;
                    triIndexPtr->index2 = 1 + (j + 1)%slices;
                }
            }
            else
            {
                for (unsigned int j = 0; j < slices; j++)
                {
                    nOffset = ((i - 1)*slices * 2 + slices + j * 2) * indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + j;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * (i - 1) + (j + 1)%slices;

                    nOffset += indexStriding;
                    triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
                    triIndexPtr->index0 = 1 + slices * (i - 1) + (j + 1)%slices;
                    triIndexPtr->index1 = 1 + slices * i + j;
                    triIndexPtr->index2 = 1 + slices * i + (j + 1)%slices;
                }
            }
        }
    }

    return true;
}   

bool                        YfBuildArcWireIndices
(
    unsigned int            slices,
    unsigned int            stacks,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices = slices * (stacks - 1) + 1;
    //unsigned int numLines    = slices * (stacks - 1) + slices * (stacks - 1);
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;

        // 行
        for (unsigned int j = 1; j < stacks; j++)
        {
            for (unsigned int i = 0; i < slices; i++)
            {
                nOffset = ((j - 1)*slices + i) * indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
                lineIndexPtr->index1 = 1 + (j - 1)*slices + (i + 1)%slices;
            }
        }

        // 列
        unsigned int half = slices * (stacks - 1);
        for (unsigned int i = 0; i < slices; i++)
        {
            nOffset = (half + i) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i;
        }
        half += slices;

        for (unsigned int j = 1; j < stacks - 1; j++)
        {
            for (unsigned int i = 0; i < slices; i++)
            {
                nOffset = (half + (j - 1)*slices + i) * indexStriding;
                lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
                lineIndexPtr->index1 = 1 + j*slices + i;
            }
        }

        //// 列
        //unsigned int half = slices * (stacks - 1);
        //for (unsigned int i = 0; i < slices; i++)                 
        //{
        //    for (unsigned int j = 0; j < stacks - 2; j++)         
        //    {
        //        nOffset = (half + (i*(stacks - 1) + j)) * indexStriding;
        //        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        //        if (j == 0)
        //        {
        //            lineIndexPtr->index0 = 0;
        //        }
        //        else
        //        {
        //            lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
        //        }

        //        lineIndexPtr->index1 = 1 + j*slices + i;
        //    }
        //}
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;

        // 行
        for (unsigned int j = 1; j < stacks; j++)                 
        {
            for (unsigned int i = 0; i < slices; i++)         
            {
                nOffset = ((j - 1)*slices + i) * indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
                lineIndexPtr->index1 = 1 + (j - 1)*slices + (i + 1)%slices;
            }
        }

        // 列
        unsigned int half = slices * (stacks - 1);
        for (unsigned int i = 0; i < slices; i++)
        {
            nOffset = (half + i) * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i;
        }
        half += slices;

        for (unsigned int j = 1; j < stacks - 1; j++)
        {
            for (unsigned int i = 0; i < slices; i++)
            {
                nOffset = (half + (j - 1)*slices + i) * indexStriding;
                lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
                lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
                lineIndexPtr->index1 = 1 + j*slices + i;
            }
        }

        // 列
        //unsigned int half = slices * (stacks - 1);
        //for (unsigned int i = 0; i < slices; i++)                 
        //{
        //    for (unsigned int j = 0; j < stacks - 2; j++)         
        //    {
        //        nOffset = (half + (i*(stacks - 1) + j)) * indexStriding;
        //        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        //        if (j == 0)
        //        {
        //            lineIndexPtr->index0 = 0;
        //        }
        //        else
        //        {
        //            lineIndexPtr->index0 = 1 + (j - 1)*slices + i;
        //        }

        //        lineIndexPtr->index1 = 1 + j*slices + i;
        //    }
        //}
    }

    return true;
}

// ******************* //
bool                        YfBuildGearwheelVertices
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (slices < 2 || !pVerticesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices*4 + 2;

    // 顶点赋值
    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -height;
    }
    else if (originPose == YE_ORIGIN_POSE_CENTER)
    {
        originOffsetY = -height * 0.5f;
    }

    // 柱顶顶点赋值
    {
        nOffset = 0;   
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = height + originOffsetY;
        curVertexPtr->z = 0.0f;
    }

    // 柱底顶点赋值
    {
        nOffset = (numVertices - 1) * vertexStriding;   
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = originOffsetY;
        curVertexPtr->z = 0.0f;
    }

    float angleXZ;
    float posX, posZ;        
    float fRadius;
    for (unsigned int i = 0; i < 2*slices; i++)
    {
        angleXZ = YD_REAL_TWAIN_PI * i / (slices*2);
        posX = sinf(angleXZ);
        posZ = cosf(angleXZ);
        fRadius = (i%2 == 0) ? radius : assistRadius;

        // 上顶点
        {
            nOffset = (1 + i) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = fRadius * posX;
            curVertexPtr->y = height + originOffsetY;
            curVertexPtr->z = fRadius * posZ;
        }

        // 下顶点
        {
            nOffset = (1 + 2*slices + i) * vertexStriding; 
            curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
            curVertexPtr->x = fRadius * posX;
            curVertexPtr->y = originOffsetY;
            curVertexPtr->z = fRadius * posZ;
        }
    }

    return true;
}     

bool                        YfBuildGearwheelTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (slices < 2 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices*4 + 2;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }
    //unsigned int numTriangles = slices * 8;

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;
        for (unsigned int i = 0; i < 2 * slices; i++)
        {
            nOffset = (i * 4) * indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + (i + 1) % (2*slices);

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = numVertices - 1;
            triIndexPtr->index1 = 1 + slices*2 + (i + 1) % (2*slices);
            triIndexPtr->index2 = 1 + slices*2 + i;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + i;
            triIndexPtr->index1 = 1 + slices*2 + i;
            triIndexPtr->index2 = 1 + (i + 1) % (2*slices);

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + (i + 1) % (2*slices);
            triIndexPtr->index1 = 1 + slices*2 + i;
            triIndexPtr->index2 = 1 + slices*2 + (i + 1) % (2*slices);
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;
        for (unsigned int i = 0; i < 2 * slices; i++)
        {
            nOffset = (i * 4) * indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + (i + 1) % (2*slices);

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = numVertices - 1;
            triIndexPtr->index1 = 1 + slices*2 + (i + 1) % (2*slices);
            triIndexPtr->index2 = 1 + slices*2 + i;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + i;
            triIndexPtr->index1 = 1 + slices*2 + i;
            triIndexPtr->index2 = 1 + (i + 1) % (2*slices);

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 1 + (i + 1) % (2*slices);
            triIndexPtr->index1 = 1 + slices*2 + i;
            triIndexPtr->index2 = 1 + slices*2 + (i + 1) % (2*slices);
        }
    }

    return true;
}   

bool                        YfBuildGearwheelWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding, 
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices = (slices*2) * 2 + 2;
    //unsigned int numLines    = (slices*2) * 4;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        for (unsigned int i = 0; i < 2 * slices; i++)
        {
            nOffset = (i * 3) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + (i + 1)%(slices*2);

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + slices*2 + i;
            lineIndexPtr->index1 = 1 + slices*2 + (i + 1)%(slices*2);

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + slices*2 + i;
        }
        
        unsigned int half = 6 * slices * indexStriding;
        for (unsigned int i = 0; i < slices; i++)
        {
            nOffset = half + (i * 2) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i*2 + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = numVertices - 1;
            lineIndexPtr->index1 = 1 + slices*2 + i*2 + 1;
        }
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;
        for (unsigned int i = 0; i < 2 * slices; i++)
        {
            nOffset = (i * 3) * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + (i + 1)%(slices*2);

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + slices*2 + i;
            lineIndexPtr->index1 = 1 + slices*2 + (i + 1)%(slices*2);

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + slices*2 + i;
        }

        unsigned int half = 6 * slices * indexStriding;
        for (unsigned int i = 0; i < slices; i++)
        {
            nOffset = half + (i * 2) * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i*2 + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = numVertices - 1;
            lineIndexPtr->index1 = 1 + slices*2 + i*2 + 1;
        }
    }

    return true;
}

// ******************* //
bool                        YfBuildStarVertices
(
    float                   radius, 
    float                   assistRadius,
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (slices < 2 || !pVerticesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = (slices*2) + 2;

    // 顶点赋值
    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr   = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -height;
    }
    else if (originPose == YE_ORIGIN_POSE_CENTER)
    {
        originOffsetY = -height * 0.5f;
    }

    // 顶赋值
    {
        nOffset = 0;   
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = height + originOffsetY;
        curVertexPtr->z = 0.0f;
    }

    // 底赋值
    {
        nOffset = (numVertices - 1) * vertexStriding;   
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = 0.0f;
        curVertexPtr->y = originOffsetY;
        curVertexPtr->z = 0.0f;
    }

    float angleXZ;
    float posX, posZ;        
    float fRadius;
    for (unsigned int i = 0; i < 2*slices; i++)
    {
        angleXZ = YD_REAL_TWAIN_PI * i / (slices*2);
        posX = sinf(angleXZ);
        posZ = cosf(angleXZ);
        fRadius = (i%2 == 0) ? radius : assistRadius;

        nOffset = (1 + i) * vertexStriding; 
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = fRadius * posX;
        curVertexPtr->y = originOffsetY;
        curVertexPtr->z = fRadius * posZ;
    }

    return true;
}     

bool                        YfBuildStarTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (slices < 2 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = slices*2 + 2;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }
    //unsigned int numTriangles = slices * 4;

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;
        for (unsigned int i = 0; i < 2 * slices; i++)
        {
            nOffset = (i * 2) * indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + (i + 1) % (2*slices);

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = numVertices - 1;
            triIndexPtr->index1 = 1 + (i + 1) % (2*slices);
            triIndexPtr->index2 = 1 + i;
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;
        for (unsigned int i = 0; i < 2 * slices; i++)
        {
            nOffset = (i * 2) * indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = 0;
            triIndexPtr->index1 = 1 + i;
            triIndexPtr->index2 = 1 + (i + 1) % (2*slices);

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = numVertices - 1;
            triIndexPtr->index1 = 1 + (i + 1) % (2*slices);
            triIndexPtr->index2 = 1 + i;
        }
    }

    return true;
}   

bool                        YfBuildStarWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (slices < 2 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices = slices*2 + 2;
    //unsigned int numLines    = slices * 5;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        for (unsigned int i = 0; i < 2 * slices; i++)
        {
            nOffset = (i * 2) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + (i + 1)%(slices*2);
        }

        unsigned int half = 4 * slices * indexStriding;
        for (unsigned int i = 0; i < slices; i++)
        {
            nOffset = half + i * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = numVertices - 1;
            lineIndexPtr->index1 = 1 + i*2 + 1;
        }
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;
        for (unsigned int i = 0; i < 2 * slices; i++)
        {
            nOffset = (i * 2) * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 0;
            lineIndexPtr->index1 = 1 + i;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = 1 + i;
            lineIndexPtr->index1 = 1 + (i + 1)%(slices*2);
        }

        unsigned int half = 4 * slices * indexStriding;
        for (unsigned int i = 0; i < slices; i++)
        {
            nOffset = half + i * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = numVertices - 1;
            lineIndexPtr->index1 = 1 + i*2 + 1;
        }
    }

    return true;
}

// ******************* //
bool                        YfBuildSpireVertices
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices,
    unsigned int            stacks, 
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
)
{
    if (stacks < 1 || slices < 3 || !pVerticesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = 2 + stacks * 2;
    //unsigned int numTriangles = stacks * 2;

    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -height;
    }
    else if (originPose == YE_ORIGIN_POSE_CENTER)
    {
        originOffsetY = -height * 0.5f;
    }

    float fAngle = YD_REAL_TWAIN_PI / slices;

    float angleXZ;
    float posX, posZ;        
    for (unsigned int i = 0; i <= stacks; i++)
    {
        angleXZ = i * fAngle;
        posX = sinf(angleXZ);
        posZ = cosf(angleXZ);
        nOffset = i * 2 * vertexStriding; 

        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = assistRadius * posX;
        curVertexPtr->y = height * i * 1.0f / stacks + originOffsetY;
        curVertexPtr->z = assistRadius * posZ;
    
        nOffset += vertexStriding;  

        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = radius * posX;
        curVertexPtr->y = height * i * 1.0f / stacks + originOffsetY;
        curVertexPtr->z = radius * posZ;
    }

    return true;
}

bool                        YfBuildSpireTriIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos, 
    void*                   pTriIndicesBuffer
)
{
    if (stacks < 3 || !pTriIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = 2 + stacks * 2;
    //unsigned int numTriangles = stacks * 2;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            nOffset = 2 * i * indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*2;
            triIndexPtr->index1 = i*2 + 1;
            triIndexPtr->index2 = i*2 + 3;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*2;
            triIndexPtr->index1 = i*2 + 3;
            triIndexPtr->index2 = i*2 + 2;
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            nOffset = 2 * i * indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*2;
            triIndexPtr->index1 = i*2 + 1;
            triIndexPtr->index2 = i*2 + 3;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*2;
            triIndexPtr->index1 = i*2 + 3;
            triIndexPtr->index2 = i*2 + 2;
        }
    }

    return true;
} 

bool                        YfBuildSpireWireIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding, 
    unsigned int            indexPos, 
    void*                   pWireIndicesBuffer
)
{
    if (stacks < 3 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices = 2 + stacks * 2;
    //unsigned int numLines    = 1 + stacks * 3;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            nOffset = (i * 3) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2;
            lineIndexPtr->index1 = i * 2 + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2 + 2;
            lineIndexPtr->index1 = i * 2;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2 + 3;
            lineIndexPtr->index1 = i * 2 + 1;
        }

        nOffset = (stacks * 3) * indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = stacks * 2;
        lineIndexPtr->index1 = stacks * 2 + 1;
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            nOffset = (i * 3) * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2;
            lineIndexPtr->index1 = i * 2 + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2 + 2;
            lineIndexPtr->index1 = i * 2;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 2 + 3;
            lineIndexPtr->index1 = i * 2 + 1;
        }

        nOffset = (stacks * 3) * indexStriding;
        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        lineIndexPtr->index0 = stacks * 2;
        lineIndexPtr->index1 = stacks * 2 + 1;
    }

    return true;
}  

// ******************* //
bool                        YfBuildStairsVertices
(
    float                   width, 
    float                   length, 
    float                   height, 
    unsigned int            stacks, 
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos, 
    void*                   pVerticesBuffer
)
{
    if (stacks < 1 || !pVerticesBuffer)
    {
        return false;
    }
    //unsigned int numVertices  = 2 + stacks * 4;
    //unsigned int numTriangles = stacks * 8;

    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr = 0;
    unsigned int nOffset = 0;

    YsVector3 vOriginOffset(-width / 2, -height / 2, -length / 2);
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        vOriginOffset.y = -height;
    }
    else if (originPose == YE_ORIGIN_POSE_BOTTOM)
    {
        vOriginOffset.y = 0.0f;
    }

    float fStepLength = length / stacks;
    float fStepHeight = height / stacks;

    for (unsigned int i = 0; i <= stacks; i++)
    {
        nOffset = i * 4 * vertexStriding; 
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = vOriginOffset.x;
        curVertexPtr->y = i * fStepHeight + vOriginOffset.y;
        curVertexPtr->z = i * fStepLength + vOriginOffset.z;

        nOffset += vertexStriding;  
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = width + vOriginOffset.x;
        curVertexPtr->y = i * fStepHeight + vOriginOffset.y;
        curVertexPtr->z = i * fStepLength + vOriginOffset.z;

        if (i == stacks)
        {
            continue;
        }

        nOffset += vertexStriding;  
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = vOriginOffset.x;
        curVertexPtr->y = (i+1) * fStepHeight + vOriginOffset.y;
        curVertexPtr->z = i * fStepLength + vOriginOffset.z;

        nOffset += vertexStriding;  
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = width + vOriginOffset.x;
        curVertexPtr->y = (i+1) * fStepHeight + vOriginOffset.y;
        curVertexPtr->z = i * fStepLength + vOriginOffset.z;
    }

    return true;
}  

bool                        YfBuildStairsTriIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    if (stacks < 1 || !pTriIndicesBuffer)
    {
        return false;
    }
    unsigned int numVertices  = 2 + stacks * 4;
    //unsigned int numTriangles = stacks * 8;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pTriIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            nOffset = 8 * i * indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 0;
            triIndexPtr->index1 = i*4 + 2;
            triIndexPtr->index2 = i*4 + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 1;
            triIndexPtr->index1 = i*4 + 2;
            triIndexPtr->index2 = i*4 + 3;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 2;
            triIndexPtr->index1 = i*4 + 4;
            triIndexPtr->index2 = i*4 + 3;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 3;
            triIndexPtr->index1 = i*4 + 4;
            triIndexPtr->index2 = i*4 + 5;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 0;
            triIndexPtr->index1 = i*4 + 1;
            triIndexPtr->index2 = i*4 + 5;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 0;
            triIndexPtr->index1 = i*4 + 5;
            triIndexPtr->index2 = i*4 + 4;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 0;
            triIndexPtr->index1 = i*4 + 4;
            triIndexPtr->index2 = i*4 + 2;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex16*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 1;
            triIndexPtr->index1 = i*4 + 3;
            triIndexPtr->index2 = i*4 + 5;
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            nOffset = 8 * i * indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 0;
            triIndexPtr->index1 = i*4 + 2;
            triIndexPtr->index2 = i*4 + 1;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 1;
            triIndexPtr->index1 = i*4 + 2;
            triIndexPtr->index2 = i*4 + 3;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 2;
            triIndexPtr->index1 = i*4 + 4;
            triIndexPtr->index2 = i*4 + 3;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 3;
            triIndexPtr->index1 = i*4 + 4;
            triIndexPtr->index2 = i*4 + 5;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 0;
            triIndexPtr->index1 = i*4 + 1;
            triIndexPtr->index2 = i*4 + 5;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 0;
            triIndexPtr->index1 = i*4 + 5;
            triIndexPtr->index2 = i*4 + 4;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 0;
            triIndexPtr->index1 = i*4 + 4;
            triIndexPtr->index2 = i*4 + 2;

            nOffset += indexStriding;
            triIndexPtr = (YsTriIndex32*)(indexPtr + nOffset);
            triIndexPtr->index0 = i*4 + 1;
            triIndexPtr->index1 = i*4 + 3;
            triIndexPtr->index2 = i*4 + 5;
        }
    }

    return true;
}  

bool                        YfBuildStairsWireIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    if (stacks < 3 || !pWireIndicesBuffer)
    {
        return false;
    }

    unsigned int numVertices  = 2 + stacks * 4;
    //unsigned int numLines     = 1 + stacks * 8;
    if (indexType == YE_INDEX_16_BIT && 
        numVertices > YD_MAX_UNSIGNED_INT16)
    {
        return false;
    }

    // 索引赋值
    char* indexPtr = (char*)pWireIndicesBuffer + indexPos;
    unsigned int nOffset = 0;
    if (indexType == YE_INDEX_16_BIT)
    {
        YsLineIndex16* lineIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            nOffset = (i * 8) * indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 0;
            lineIndexPtr->index1 = i * 4 + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 2;
            lineIndexPtr->index1 = i * 4 + 3;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 0;
            lineIndexPtr->index1 = i * 4 + 2;
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 4;
            lineIndexPtr->index1 = i * 4 + 2;
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 0;
            lineIndexPtr->index1 = i * 4 + 4;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 1;
            lineIndexPtr->index1 = i * 4 + 3;
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 5;
            lineIndexPtr->index1 = i * 4 + 3;
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 1;
            lineIndexPtr->index1 = i * 4 + 5;
        }

        nOffset = (stacks * 8) * indexStriding;
        lineIndexPtr = (YsLineIndex16*)(indexPtr + nOffset);
        lineIndexPtr->index0 = stacks * 4;
        lineIndexPtr->index1 = stacks * 4 + 1;
    }
    else
    {
        YsLineIndex32* lineIndexPtr = 0;
        for (unsigned int i = 0; i < stacks; i++)
        {
            nOffset = (i * 8) * indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 0;
            lineIndexPtr->index1 = i * 4 + 1;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 2;
            lineIndexPtr->index1 = i * 4 + 3;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 0;
            lineIndexPtr->index1 = i * 4 + 2;
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 4;
            lineIndexPtr->index1 = i * 4 + 2;
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 0;
            lineIndexPtr->index1 = i * 4 + 4;

            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 1;
            lineIndexPtr->index1 = i * 4 + 3;
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 5;
            lineIndexPtr->index1 = i * 4 + 3;
            nOffset += indexStriding;
            lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
            lineIndexPtr->index0 = i * 4 + 1;
            lineIndexPtr->index1 = i * 4 + 5;
        }

        nOffset = (stacks * 8) * indexStriding;
        lineIndexPtr = (YsLineIndex32*)(indexPtr + nOffset);
        lineIndexPtr->index0 = stacks * 4;
        lineIndexPtr->index1 = stacks * 4 + 1;
    }

    return true;
} 

// ******************* //
bool                        YfBuildSpiralStairsVertices
(
    float                   radius, 
    float                   assistRadius, 
    float                   height,
    unsigned int            slices,
    unsigned int            stacks, 
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos, 
    void*                   pVerticesBuffer
)
{
    if (stacks < 1 || slices < 3 || !pVerticesBuffer)
    {
        return false;
    }
    //unsigned int numVertices  = 2 + stacks * 4;
    //unsigned int numTriangles = stacks * 8;

    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    YsVector3* curVertexPtr = 0;
    unsigned int nOffset = 0;

    float originOffsetY = 0.0f;
    if (originPose == YE_ORIGIN_POSE_TOP)
    {
        originOffsetY = -height;
    }
    else if (originPose == YE_ORIGIN_POSE_CENTER)
    {
        originOffsetY = -height * 0.5f;
    }

    float fStepTexcoord = 1.0f / (stacks - 1);
    float fStepHeight = height / stacks;
    float fStepAngle = YD_REAL_TWAIN_PI / slices;

    float angleXZ;
    float posX, posZ;        
    for (unsigned int i = 0; i <= stacks; i++)
    {
        angleXZ = i * fStepAngle;
        posX = sinf(angleXZ);
        posZ = cosf(angleXZ);

        nOffset = i * 4 * vertexStriding; 
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = radius * posX;
        curVertexPtr->y = i * fStepHeight + originOffsetY;
        curVertexPtr->z = radius * posZ;
    
        nOffset += vertexStriding;  
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = assistRadius * posX;
        curVertexPtr->y = i * fStepHeight + originOffsetY;
        curVertexPtr->z = assistRadius * posZ;
 
        if (i == stacks)
        {
            continue;
        }

        nOffset += vertexStriding;  
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = radius * posX;
        curVertexPtr->y = (i+1) * fStepHeight + originOffsetY;
        curVertexPtr->z = radius * posZ;
   
        nOffset += vertexStriding;  
        curVertexPtr = (YsVector3*)(vertexPtr + nOffset);
        curVertexPtr->x = assistRadius * posX;
        curVertexPtr->y = (i+1) * fStepHeight + originOffsetY;
        curVertexPtr->z = assistRadius * posZ;
    }

    return true;
}    

bool                        YfBuildSpiralStairsTriIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
)
{
    return YfBuildStairsTriIndices(
                stacks, 
                indexType, 
                indexStriding, 
                indexPos,
                pTriIndicesBuffer
                );
}  

bool                        YfBuildSpiralStairsWireIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
)
{
    return YfBuildStairsWireIndices(
                stacks, 
                indexType,  
                indexStriding,  
                indexPos,
                pWireIndicesBuffer
                );
}  

// --------------------------------------------------------------------------------------

inline void					YfVector3Cross
(
    YsVector3&          vDest, 
    const YsVector3&    vLeft, 
    const YsVector3&    vRight
)
{
    vDest.x = (vLeft.y * vRight.z) - (vLeft.z * vRight.y);
    vDest.y = (vLeft.z * vRight.x) - (vLeft.x * vRight.z);
    vDest.z = (vLeft.x * vRight.y) - (vLeft.y * vRight.x);
}

// 生成顶点法线
bool                        YfCalculateVertexNormal
(
    void*                   pNormalsBuffer, 
    unsigned int            normalStriding, 
    unsigned int            normalPos,  
    const void*             pVerticesBuffer,
    unsigned int            vertexNum,
    unsigned int            vertexStriding,
    unsigned int            vertexPos,    
    void*                   pIndicesBuffer,
    unsigned int            trianglesNum,
    YeIndexType             indexType,
    unsigned int            indexStriding,
    unsigned int            indexPos 
)
{
    if (!pNormalsBuffer || !pVerticesBuffer || !pIndicesBuffer ||
        vertexNum < 3 || trianglesNum < 1)
    {
        return false;
    }

    char* vertexPtr = (char*)pVerticesBuffer + vertexPos;
    char* normalPtr = (char*)pNormalsBuffer + normalPos;
    char* indexPtr  = (char*)pIndicesBuffer + indexPos;

    const YsVector3* curVertexPtr[3];
    YsVector3* curNormalPtr[3];
    unsigned int nOffset = 0;
    YsVector3 vSub_1;
    YsVector3 vSub_2;
    YsVector3 vCross;

    for (unsigned int i = 0; i < vertexNum; i++)
    {
        curNormalPtr[0] = (YsVector3*)(normalPtr + i * normalStriding);
		curNormalPtr[0]->x = 0.0f;
		curNormalPtr[0]->y = 0.0f;
		curNormalPtr[0]->z = 0.0f;
    }
  
    if (indexType == YE_INDEX_16_BIT)
    {
        YsTriIndex16* triIndexPtr = NULL;
        for (unsigned int i = 0; i < trianglesNum; i++)
        {
            triIndexPtr = (YsTriIndex16*)(indexPtr + i * indexStriding);
            curVertexPtr[0] = (YsVector3*)(vertexPtr + triIndexPtr->index0 * vertexStriding);
            curVertexPtr[1] = (YsVector3*)(vertexPtr + triIndexPtr->index1 * vertexStriding);
            curVertexPtr[2] = (YsVector3*)(vertexPtr + triIndexPtr->index2 * vertexStriding);
            curNormalPtr[0] = (YsVector3*)(normalPtr + triIndexPtr->index0 * normalStriding);
            curNormalPtr[1] = (YsVector3*)(normalPtr + triIndexPtr->index1 * normalStriding);
            curNormalPtr[2] = (YsVector3*)(normalPtr + triIndexPtr->index2 * normalStriding);

			vSub_1.x = curVertexPtr[1]->x - curVertexPtr[0]->x;
			vSub_1.y = curVertexPtr[1]->y - curVertexPtr[0]->y;
			vSub_1.z = curVertexPtr[1]->z - curVertexPtr[0]->z;

			vSub_2.x = curVertexPtr[2]->x - curVertexPtr[0]->x;
			vSub_2.y = curVertexPtr[2]->y - curVertexPtr[0]->y;
			vSub_2.z = curVertexPtr[2]->z - curVertexPtr[0]->z;

            YfVector3Cross(vCross, vSub_1, vSub_2);
            
            for (unsigned int j = 0; j < 3; j++)
            {
                curNormalPtr[j]->x += vCross.x;
                curNormalPtr[j]->y += vCross.y;
                curNormalPtr[j]->z += vCross.z;
            }
        }
    }
    else
    {
        YsTriIndex32* triIndexPtr = NULL;
        for (unsigned int i = 0; i < trianglesNum; i++)
        {
            triIndexPtr = (YsTriIndex32*)(indexPtr + i * indexStriding);
            curVertexPtr[0] = (YsVector3*)(vertexPtr + triIndexPtr->index0 * vertexStriding);
            curVertexPtr[1] = (YsVector3*)(vertexPtr + triIndexPtr->index1 * vertexStriding);
            curVertexPtr[2] = (YsVector3*)(vertexPtr + triIndexPtr->index2 * vertexStriding);
            curNormalPtr[0] = (YsVector3*)(normalPtr + triIndexPtr->index0 * normalStriding);
            curNormalPtr[1] = (YsVector3*)(normalPtr + triIndexPtr->index1 * normalStriding);
            curNormalPtr[2] = (YsVector3*)(normalPtr + triIndexPtr->index2 * normalStriding);

			vSub_1.x = curVertexPtr[1]->x - curVertexPtr[0]->x;
			vSub_1.y = curVertexPtr[1]->y - curVertexPtr[0]->y;
			vSub_1.z = curVertexPtr[1]->z - curVertexPtr[0]->z;

			vSub_2.x = curVertexPtr[2]->x - curVertexPtr[0]->x;
			vSub_2.y = curVertexPtr[2]->y - curVertexPtr[0]->y;
			vSub_2.z = curVertexPtr[2]->z - curVertexPtr[0]->z;

            YfVector3Cross(vCross, vSub_1, vSub_2);

            for (unsigned int j = 0; j < 3; j++)
            {
                curNormalPtr[j]->x += vCross.x;
                curNormalPtr[j]->y += vCross.y;
                curNormalPtr[j]->z += vCross.z;
            }
        }
    }

	float sq;
    for (unsigned int i = 0; i < vertexNum; i++)
    {
        curNormalPtr[0] = (YsVector3*)(normalPtr + i * normalStriding);
		sq = (curNormalPtr[0]->x*curNormalPtr[0]->x + curNormalPtr[0]->y*curNormalPtr[0]->y + curNormalPtr[0]->z*curNormalPtr[0]->z);
        if (sq > 0.0f)
        {
			sq = sqrtf(sq);
			curNormalPtr[0]->x /= sq;
			curNormalPtr[0]->y /= sq;
			curNormalPtr[0]->z /= sq;
        }
    }

    return true;
}

// 生成法线
bool						YfUpdateGeometryNormal
(
    const YsGeometryDesc&   geomDesc, 
    unsigned int            normalPos,
    void*                   pVerticesBuffer,  
    unsigned int            vertexNum, 
    void*                   pIndicesBuffer,  
    unsigned int            trianglesNum
)
{
    if (!pVerticesBuffer || vertexNum == 0 ||
        !pIndicesBuffer || trianglesNum == 0)
    {
        return false;
    }

    if (geomDesc.vertexStriding < normalPos + 12)
    {
        return false;
    }

    return YfCalculateVertexNormal(
         pVerticesBuffer, 
         geomDesc.vertexStriding, 
         normalPos,  
         pVerticesBuffer,
         vertexNum, 
         geomDesc.vertexStriding,
         geomDesc.vertexPos,    
         pIndicesBuffer,
         trianglesNum,
         geomDesc.triIndexType,
         geomDesc.triIndexStriding,
         geomDesc.triIndexPos
         );
}