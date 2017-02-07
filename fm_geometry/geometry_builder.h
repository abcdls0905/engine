
/****************************************************************

  Author      :  叶峰
  Version     :  2.0
  Create Date :  2010/01/08  
  Description :  几何图形数据的创建

*****************************************************************/

#ifndef __YfGeometryBuilder_H__
#define __YfGeometryBuilder_H__

// INCLUDES -----------------------------------------------------------------------------

#include "../visual/i_geometry.h"

// --------------------------------------------------------------------------------------

// 索引类型(16位索引或32位索引)
enum YeIndexType
{
	YE_INDEX_INVALID		= 0,        
	YE_INDEX_16_BIT			= 16,
	YE_INDEX_32_BIT			= 32,
};

// --------------------------------------------------------------------------------------

// Geom的数据结构描述
struct YsGeometryDesc
{
	unsigned int        vertexStriding;     // 每个顶点结构所占的字节数
	unsigned int        vertexPos;          // 顶点数据在结构体中的偏移位置

	YeIndexType         triIndexType;
	unsigned int        triIndexStriding; 
	unsigned int        triIndexPos;

	YeIndexType         wireIndexType;
	unsigned int        wireIndexStriding; 
	unsigned int        wireIndexPos;

	YsGeometryDesc()
	{
		vertexStriding      = 12;
		vertexPos           = 0;  
		triIndexType        = YE_INDEX_16_BIT;
		triIndexStriding    = 6;
		triIndexPos         = 0;
		wireIndexType       = YE_INDEX_16_BIT;
		wireIndexStriding   = 4;
		wireIndexPos        = 0;
	}
};

// --------------------------------------------------------------------------------------

// 获取创建图形需要的数据大小
bool   YfCalculateGeometryBufferSize
(
    YeGraphType            graphType,      
    unsigned int           slices, 
    unsigned int           stacks,           
    unsigned int&		   numVertices,        // 顶点数目
	unsigned int&          numTriangles,       // 三角形数目
	unsigned int&          numLines            // 线段数目
);

// 获取创建图形需要的数据大小
bool   YfCalculateGeometryBufferSize
(
    const YsGraphDesc&     graphDesc, 
	unsigned int&          numVertices,        // 顶点数目
	unsigned int&          numTriangles,       // 三角形数目
	unsigned int&          numLines            // 线段数目
);

// 创建图形
bool   YfBuildGeometry
(
    const YsGraphDesc&      graphDesc, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,    // 顶点数据指针
    void*                   pTriIndicesBuffer,  // 三角形索引数据指针
    void*                   pWireIndicesBuffer  // 线段索引数据指针
);

bool   YfBuildPlaneGeometry
(
    float                   width,              // 平面X方向长度
    float                   length,             // 平面Z方向长度
    float                   height,             // 平面Y方向位置
    unsigned int            slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);   

bool   YfBuildBoxGeometry
(
    float                   extentX, 
    float                   extentY,  
    float                   extentZ, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

bool   YfBuildSphereGeometry
(
    float                   radius, 
    unsigned int			slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

bool   YfBuildHemisphereGeometry
(
    float                   radius, 
    unsigned int            slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);   

bool   YfBuildDrumGeometry
(
    float                   radius, 
    float                   assistRadius,
    unsigned int            slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);   

bool   YfBuildConeGeometry
(
    float                   radius, 
    float                   height, 
    unsigned int            slices, 
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     
    
bool   YfBuildCylinderGeometry
(
    float                   topRadius, 
    float                   bottomRadius, 
    float                   height, 
    unsigned int            slices, 
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

bool   YfBuildCapsuleGeometry
(
    float                   radius, 
    float                   height, 
    unsigned int            slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

bool   YfBuildPyramidGeometry
(
    float                   width, 
    float                   length, 
    float                   height, 
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);  

// 创建一个圆面
bool   YfBuildRoundGeometry
(
    float                   radius, 
    float                   height, 
    unsigned int            slices, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

// 创建一个圆环
bool   YfBuildRingGeometry
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

bool   YfBuildPipeGeometry
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

bool   YfBuildWedgeGeometry
(
    float                   width, 
    float                   length, 
    float                   height, 
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

bool   YfBuildFunGeometry
(   
    float                   radius, 
    float                   degree, 
    float                   height, 
    unsigned int            slices, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

bool   YfBuildArcGeometry
(
    float                   radius, 
    float                   degree, 
    float                   height, 
    unsigned int            slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

bool   YfBuildGearWheelGeometry
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

bool   YfBuildStarGeometry
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);     

bool   YfBuildSpireGeometry
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);    

bool   YfBuildStairsGeometry
(
    float                   width, 
    float                   length, 
    float                   height, 
    unsigned int            stacks, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);    

bool   YfBuildSpiralStairsGeometry
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
);    

// --------------------------------------------------------------------------------------

// ******************* //
bool   YfBuildPlaneVertices
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
);

bool   YfBuildPlaneTriIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType, 
    unsigned int            indexStriding, 
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);

bool   YfBuildPlaneWireIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);

// ******************* //
bool   YfBuildBoxVertices
(
    float                   extentX,
    float                   extentY,  
    float                   extentZ, 
    YeOriginPose            originPose,
    unsigned int            vertexStriding,
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);   

bool   YfBuildBoxTriIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);  

bool   YfBuildBoxWireIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);  

// ******************* //
bool   YfBuildSphereVertices
(
    float                   radius, 
    unsigned int            slices,
    unsigned int            stacks, 
    YeOriginPose            originPose,  
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildSphereTriIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);  

bool   YfBuildSphereWireIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);  

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
);     

bool   YfBuildDrumTriIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);  

bool   YfBuildDrumWireIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);  

// ******************* //
bool   YfBuildHemisphereVertices
(
    float                   radius, 
    unsigned int            slices,
    unsigned int            stacks, 
    YeOriginPose            originPose,  
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildHemisphereTriIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);  

bool   YfBuildHemisphereWireIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);  

// ******************* //
bool   YfBuildConeVertices
(
    float                   radius, 
    float                   height, 
    unsigned int            slices, 
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildConeTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);     

bool   YfBuildConeWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);    

// ******************* //
bool   YfBuildCylinderVertices
(
    float                   topRadius, 
    float                   bottomRadius, 
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildCylinderTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);     

bool   YfBuildCylinderWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);     

// ******************* //
bool   YfBuildCapsuleVertices
(
    float                   radius, 
    float                   height, 
    unsigned int            slices,
    unsigned int            stacks, 
    YeOriginPose            originPose, 
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildCapsuleTriIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos, 
    void*                   pTriIndicesBuffer
);     

bool   YfBuildCapsuleWireIndices
(
    unsigned int            slices,
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos, 
    void*                   pWireIndicesBuffer
);  

// ******************* //
bool   YfBuildPyramidVertices
(
    float                   width, 
    float                   length, 
    float                   height, 
    YeOriginPose            originPose, 
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildPyramidTriIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);     

bool   YfBuildPyramidWireIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding, 
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
); 

// ******************* //
bool   YfBuildRoundVertices
(
    float                   radius, 
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose, 
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildRoundTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);     

bool   YfBuildRoundWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);  

// ******************* //
bool   YfBuildRingVertices
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
);     

bool   YfBuildRingTriIndices
(
    unsigned int            slices,
    unsigned int            stacks,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);   

bool   YfBuildRingWireIndices
(
    unsigned int            slices,
    unsigned int            stacks,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);   

// ******************* //
bool   YfBuildPipeVertices
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose, 
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildPipeTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);   

bool   YfBuildPipeWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);

// ******************* //
bool   YfBuildWedgeVertices
(
    float                   width, 
    float                   length, 
    float                   height, 
    YeOriginPose            originPose,  
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildWedgeTriIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);                         

bool   YfBuildWedgeWireIndices
(
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);                         

// ******************* //
bool   YfBuildFunVertices
(
    float                   radius, 
    float                   degree, 
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose,
    unsigned int            vertexStriding,
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildFunTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);   

bool   YfBuildFunWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);

// ******************* //
bool   YfBuildArcVertices
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
);                          

bool   YfBuildArcTriIndices
(
    unsigned int            slices,
    unsigned int            stacks,
    YeIndexType             indexType,
    unsigned int            indexStriding, 
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);   

bool   YfBuildArcWireIndices
(
    unsigned int            slices,
    unsigned int            stacks,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);

// ******************* //
bool   YfBuildGearwheelVertices
(
    float                   radius, 
    float                   assistRadius, 
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildGearwheelTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);   

bool   YfBuildGearwheelWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding, 
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);

// ******************* //
bool   YfBuildStarVertices
(
    float                   radius, 
    float                   assistRadius,
    float                   height, 
    unsigned int            slices,
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos,
    void*                   pVerticesBuffer
);     

bool   YfBuildStarTriIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);   

bool   YfBuildStarWireIndices
(
    unsigned int            slices,
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);

// ******************* //
bool   YfBuildSpireVertices
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
);                          

bool   YfBuildSpireTriIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos, 
    void*                   pTriIndicesBuffer
);  

bool   YfBuildSpireWireIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding, 
    unsigned int            indexPos, 
    void*                   pWireIndicesBuffer
);  

// ******************* //
bool   YfBuildStairsVertices
(
    float                   width, 
    float                   length, 
    float                   height, 
    unsigned int            stacks, 
    YeOriginPose            originPose,
    unsigned int            vertexStriding, 
    unsigned int            vertexPos, 
    void*                   pVerticesBuffer
);  

bool   YfBuildStairsTriIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);  

bool   YfBuildStairsWireIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
); 

// ******************* //
bool   YfBuildSpiralStairsVertices
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
);                          

bool   YfBuildSpiralStairsTriIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pTriIndicesBuffer
);  

bool   YfBuildSpiralStairsWireIndices
(
    unsigned int            stacks, 
    YeIndexType             indexType,
    unsigned int            indexStriding,  
    unsigned int            indexPos,
    void*                   pWireIndicesBuffer
);  

// --------------------------------------------------------------------------------------

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
);

// 生成法线
bool						YfUpdateGeometryNormal
(
    const YsGeometryDesc&   geomDesc, 
    unsigned int            normalPos,
    void*                   pVerticesBuffer,  
    unsigned int            vertexNum, 
    void*                   pIndicesBuffer,  
    unsigned int            trianglesNum
);

// --------------------------------------------------------------------------------------

#include "geometry_builder.inl"

// --------------------------------------------------------------------------------------

#endif