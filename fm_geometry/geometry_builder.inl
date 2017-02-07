
/****************************************************************

  Author      :  叶峰
  Version     :  2.0
  Create Date :  2010/01/08   
  Description :  几何图形数据的创建

*****************************************************************/

// --------------------------------------------------------------------------------------

inline    bool              YfCalculateGeometryBufferSize
(
    const YsGraphDesc&      graphDesc, 
    unsigned int&			numVertices,        // 顶点数目
    unsigned int&			numTriangles,       // 三角形数目
    unsigned int&			numLines            // 线段数目
)
{
    return YfCalculateGeometryBufferSize(
                graphDesc.graphType, 
                graphDesc.slices, 
                graphDesc.stacks, 
                numVertices, 
                numTriangles, 
                numLines
                );
}

inline    bool              YfBuildPlaneGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildPlaneVertices(
            width, 
            length, 
            height, 
            slices, 
            stacks, 
            originPose, 
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildPlaneTriIndices(
            slices, 
            stacks, 
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildPlaneWireIndices(
            slices, 
            stacks, 
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}   

inline    bool              YfBuildBoxGeometry
(
    float                   extentX, 
    float                   extentY,  
    float                   extentZ, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
)
{
    if (pVerticesBuffer &&
        !YfBuildBoxVertices(
            extentX, 
            extentY, 
            extentZ, 
            originPose, 
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildBoxTriIndices(
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildBoxWireIndices(
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}

inline    bool              YfBuildSphereGeometry
(
    float                   radius,  
    unsigned int            slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
)
{
    if (pVerticesBuffer &&
        !YfBuildSphereVertices(
            radius, 
            slices, 
            stacks, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildSphereTriIndices(
            slices, 
            stacks, 
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildSphereWireIndices(
            slices, 
            stacks, 
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}   
     
inline    bool              YfBuildDrumGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildDrumVertices(
            radius, 
            assistRadius, 
            slices, 
            stacks, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildDrumTriIndices(
            slices, 
            stacks, 
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildDrumWireIndices(
            slices, 
            stacks, 
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}

inline    bool              YfBuildHemisphereGeometry
(
    float                   radius, 
    unsigned int            slices, 
    unsigned int            stacks, 
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
)
{
    if (pVerticesBuffer &&
        !YfBuildHemisphereVertices(
            radius, 
            slices, 
            stacks, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildHemisphereTriIndices(
            slices, 
            stacks, 
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildHemisphereWireIndices(
            slices, 
            stacks, 
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}

inline    bool              YfBuildConeGeometry
(
    float                   radius,  
    float                   height,  
    unsigned int            slices, 
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
)
{
    if (pVerticesBuffer &&
        !YfBuildConeVertices(
            radius, 
            height, 
            slices, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildConeTriIndices(
            slices, 
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding,
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildConeWireIndices(
            slices, 
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}   
     
inline    bool              YfBuildCylinderGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildCylinderVertices(
            topRadius, 
            bottomRadius, 
            height, 
            slices, 
            originPose,
            dataDesc.vertexStriding,
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildCylinderTriIndices(
            slices,
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding,
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildCylinderWireIndices(
            slices,
            dataDesc.wireIndexType,
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}   
     
inline    bool              YfBuildCapsuleGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildCapsuleVertices(
            radius, 
            height, 
            slices, 
            stacks, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildCapsuleTriIndices(
            slices, 
            stacks, 
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildCapsuleWireIndices(
            slices, 
            stacks, 
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}   
     
inline    bool              YfBuildPyramidGeometry
(
    float                   width, 
    float                   length, 
    float                   height,  
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
)
{
    if (pVerticesBuffer &&
        !YfBuildPyramidVertices(
            width, 
            length,
            height, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildPyramidTriIndices(
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildPyramidWireIndices(
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}     

inline    bool              YfBuildRoundGeometry
(
    float                   radius,  
    float                   height,  
    unsigned int            slices, 
    YeOriginPose            originPose,
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
)
{
    if (pVerticesBuffer &&
        !YfBuildRoundVertices(
            radius, 
            height, 
            slices, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildRoundTriIndices(
            slices,
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildRoundWireIndices(
            slices,
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}   
     
inline    bool              YfBuildRingGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildRingVertices(
            radius, 
            assistRadius, 
            height, 
            slices, 
            stacks, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildRingTriIndices(
            slices, 
            stacks, 
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildRingWireIndices(
            slices, 
            stacks, 
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}        

inline    bool              YfBuildPipeGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildPipeVertices(
            radius, 
            assistRadius,
            height, 
            slices, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildPipeTriIndices(
            slices,
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildPipeWireIndices(
            slices,
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding,
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}        

inline    bool              YfBuildWedgeGeometry
(
    float                   width, 
    float                   length, 
    float                   height,  
    YeOriginPose            originPose, 
    const YsGeometryDesc&   dataDesc, 
    void*                   pVerticesBuffer,
    void*                   pTriIndicesBuffer,
    void*                   pWireIndicesBuffer
)
{
    if (pVerticesBuffer &&
        !YfBuildWedgeVertices(
            width, 
            length, 
            height, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildWedgeTriIndices(
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildWedgeWireIndices(
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}

inline    bool              YfBuildFunGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildFunVertices(
            radius, 
            degree, 
            height, 
            slices, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildFunTriIndices(
            slices,
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildFunWireIndices(
            slices,
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}

inline    bool              YfBuildArcGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildArcVertices(
            radius, 
            degree, 
            height, 
            slices, 
            stacks, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildArcTriIndices(
            slices, stacks,
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildArcWireIndices(
            slices, stacks,
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}

inline    bool              YfBuildGearWheelGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildGearwheelVertices(
            radius, 
            assistRadius, 
            height, 
            slices, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }
    if (pTriIndicesBuffer && 
        !YfBuildGearwheelTriIndices(
            slices,
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildGearwheelWireIndices(
            slices,
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding,
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}

inline    bool              YfBuildStarGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildStarVertices(
            radius, 
            assistRadius, 
            height, 
            slices, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildStarTriIndices(
            slices,
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildStarWireIndices(
            slices,
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}

inline    bool              YfBuildSpireGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildSpireVertices(
            radius, 
            assistRadius, 
            height, 
            slices, 
            stacks, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildSpireTriIndices(
            stacks,
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildSpireWireIndices(
            stacks,
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}

inline    bool              YfBuildStairsGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildStairsVertices(
            width, 
            length, 
            height, 
            stacks, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildStairsTriIndices(
            stacks,
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildStairsWireIndices(
            stacks,
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}

inline    bool              YfBuildSpiralStairsGeometry
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
)
{
    if (pVerticesBuffer &&
        !YfBuildSpiralStairsVertices(
            radius, 
            assistRadius, 
            height, 
            slices, 
            stacks, 
            originPose,
            dataDesc.vertexStriding, 
            dataDesc.vertexPos,
            pVerticesBuffer
            )
        )
    {
        return false;
    }

    if (pTriIndicesBuffer && 
        !YfBuildSpiralStairsTriIndices(
            stacks,
            dataDesc.triIndexType, 
            dataDesc.triIndexStriding, 
            dataDesc.triIndexPos,
            pTriIndicesBuffer
            )
        )
    {
        return false;
    }

    if (pWireIndicesBuffer && 
        !YfBuildSpiralStairsWireIndices(
            stacks,
            dataDesc.wireIndexType, 
            dataDesc.wireIndexStriding, 
            dataDesc.wireIndexPos,
            pWireIndicesBuffer
            )
        )
    {
        return false;
    }

    return true;
}

// --------------------------------------------------------------------------------------
