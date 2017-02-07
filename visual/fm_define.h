//--------------------------------------------------------------------
// �ļ���:		fm_define.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��9��11��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _FM_DEFINE_H_
#define _FM_DEFINE_H_
#include "../public/macros.h"
#include "../public/persistid.h"

#include "../math/fm_math.h"
/*-------------------------------------------------------------------------
 * Data type definitions
 *-----------------------------------------------------------------------*/

typedef void             fm_void;
typedef unsigned int     fm_enum;
typedef unsigned char    fm_boolean;
typedef unsigned int     fm_bitfield;
typedef signed   char    fm_byte;
typedef short            fm_short;
typedef int              fm_int;
typedef int              fm_sizei;
typedef unsigned char    fm_ubyte;
typedef unsigned short   fm_ushort;
typedef unsigned int     fm_uint;
typedef float            fm_float;
typedef float            fm_clampf;
typedef int              fm_fixed;



union TypePointer
{
	unsigned int*  dw;
    int*           n;
	float*         f;
	double*        d;
	char*          c;
	unsigned char* uc;
	void*          p;

	TypePointer(void* in):p(in)
	{
	}
};



enum {
	PRIMITIVE_LINELIST,
	PRIMITIVE_TRIANGLE
};

enum 
{
	RT_TYPE_FIXED,	 // ��ȾĿ��Ĵ�С�̶�
	RT_TYPE_DEVICE, // ��ȾĿ���뵱ǰ�豸�ĳߴ����
};


/*
��ʽ���
Term Definition 
SNORM   Signed normalized integer, meaning that for an n-bit 2's complement number, the maximum value means 1.0f (e.g. the 5-bit value 01111 maps to 1.0f), 
        and the minimum value means -1.0f (e.g. the 5-bit value 10000 maps to -1.0f). In addition, the second-minimum number maps to -1.0f (e.g. the 5-bit value 10001 maps to -1.0f). 
		There are thus two integer representations for -1.0f. There is a single representation for 0.0f, and a single representation for 1.0f.
		This results in a set of integer representations for evenly spaced floating point values in the range (-1.0f...0.0f), 
		and also a complementary set of representations for numbers in the range (0.0f...1.0f) 

UNORM   Unsigned normalized integer, meaning that for an n-bit number, all 0's means 0.0f, and all 1's means 1.0f. A sequence of evenly spaced floating point values from 0.0f to 1.0f are represented. 
        e.g. a 2-bit UNORM represents 0.0f, 1/3, 2/3, and 1.0f. 

SINT   Signed integer. 2's complement integer. e.g. an 3-bit SINT represents the integral values -4, -3, -2, -1, 0, 1, 2, 3. 

UINT   Unsigned integer. e.g. a 3-bit UINT represents the integral values 0, 1, 2, 3, 4, 5, 6, 7. 

FLOAT  A floating-point value in any of the representations defined by Direct3D 10. 

SRGB   Similar to UNORM, in that for an n-bit number, all 0's means 0.0f and all 1's means 1.0f. 
       However unlike UNORM, with SRGB the sequence of unsigned integer encodings between all 0's to all 1's represent a nonlinear progression in the floating point interpretation of the numbers, 
	     between 0.0f to 1.0f. Roughly, if this nonlinear progression, SRGB, is displayed as a sequence of colors, it would appear as a linear ramp of luminosity levels to an "average" observer, 
		 under "average" viewing conditions, on an "average" display. For complete detail, refer to the SRGB color standard, IEC 61996-2-1, at IEC (International Electrotechnical Commission). 
*/

// ֧�ֵĶ�̬������ͼ�ĸ�ʽ
enum TEX_FORMAT
{
	TEX_FORMAT_NONE,
	TEX_FORMAT_RGB_DEFAULT,
	TEX_FORMAT_RGBA_DEFAULT,
	TEX_FORMAT_RGB565_UNORM,
	TEX_FORMAT_RGBA4444_UNORM,
	TEX_FORMAT_RGB888_UNORM,
	TEX_FORMAT_RGBA8888_UNORM,
	TEX_FORMAT_RGB161616_FLOAT,//16λ�����ʽ
	TEX_FORMAT_RGBA16161616_SNORM,
	TEX_FORMAT_RGBA16161616F_FLOAT,
	TEX_FORMAT_RG1616_FLOAT,
	TEX_FORMAT_R16_FLOAT,//�Ѿ�֪����iphone5֧��
	TEX_FORMAT_R32_FLOAT,//Ŀǰ��֪��ʲô�豸֧��
	//������
	TEX_FORMAT_D_DEFAULT,//δָ����ʽ����ȸ�ʽ
	TEX_FORMAT_D16_UINT,
	TEX_FORMAT_D32_UINT,
	TEX_FORMAT_D24_UINT,
	TEX_FORMAT_D24S8_UINT,//�����ʽҪ���⴦��
    //ģ�����
	TEX_FORMAT_S_DEFAULT,//δָ����ʽ��ģ���ʽ
	TEX_FORMAT_S1_UINT,//1λģ��
	TEX_FORMAT_S4_UINT,//4λģ��
	TEX_FORMAT_S8_UINT,//8λģ��
	
	TEX_FORMAT_A8_UNORM,
	TEX_FORMAT_CUBE_UINT,
	TXF_FORMAT_DDS,
	TEX_FORMAT_UNKNOWN,
};


// ��ѡ�ص�����
typedef bool (*CULL_FUNC)(void* context, const FmMat4& mat, 
	const FmVec3* v, size_t nv);

// ��ײ�����Ϣ
struct trace_info_t
{
	// ��ײ����
	struct hit_t
	{
		float fDistance;		// ��ײ����
		float fTraceU;			// ��ײλ��
		float fTraceV;
		PERSISTID TraceID;		// ��ײ����
		int nMaterialIndex;		// ��������
		const char* strMaterialName; //������
		FmVec3 Vertex[3];	// �����ζ���
		FmVec3 VertexMark[3]; //Vertex�о�����ȷ��������Ȳ�����
		const char* strTexName;	// ��ͼ��
		FmVec2 TexUV[3];	// �����ζ������ͼUV
	};
	
	int nHitMax;			// �����ײ����
	int nHitCount;			// ������ײ����
	float fDistance;		// �����ײ����(0��1)
	hit_t Hits[1];

	trace_info_t()
	{
		nHitMax = 1;
		nHitCount = 0;
		fDistance = 1e8;
	}
};

// ���������
struct camera_t
{
	FmVec3 vPosition;
	FmVec3 vAngle;
	float fFov;
	float fRatio;
	float fNearZ;
	float fFarZ;
	FmMat4 mtxView;
	FmMat4 mtxProj;
	FmMat4 mtxViewProj;
	FmMat4 mtxViewInverse;
	FmMat4 mtxProjInverse;
	FmMat4 mtxViewRelative;
	FmMat4 mtxViewProjRelative;

	//For Dx
	FmMat4 mtxDxProj;
	FmMat4 mtxDxViewProj;
	//viewport_t Viewport;
	FmVec3 vFrustumCenter;
	float fFrustumRadius;
	float fFrustumRadius2D;
	unsigned int nPhaseMask;
};

// ����ͳ��CPU��Ϣ
struct performance_cpu_info_t
{
	double dRenderTime; 					
	double dPresentTime;
	double dUpdateTime;
	double dRealizeTime;

	double dGuiRealizeTime;
	double dShadowMapRealizeTime;
	double dReflectionRealizeTime;
	double dPostProcessRealizeTime;
	double dSoundRealizeTime;

	double dGroundRealizeTime;
	double dVisualRealizeTime;
	double dGrassRealizeTime;
	double dWaterRealizeTime;

	double dModelRealizeTime;
	double dActorRealizeTime;
	double dParticleRealizeTime;
	double dShadowRealizeTime;

	double dTerrainRefreshTime;
	double dParticleRefreshTime;
	double dShadowRefreshTime;
	double dSkeletonBlendTime;

	double dRayTraceTime;
	double dSphereTraceTime;
	double dCullTime;
};

// ����ͳ��GPU�������
struct performance_gpu_info_t
{
	double dDrawGuiTime;
	double dDrawEarlyZBatchsTime;
    double dDrawSolidBatchsTime;
	double dDrawGroundBatchsTime;
	double dDrawSolidAlphaTestBatchsTime;
	double dDrawGrassBatchsTime;
	double dDrawAfterSolidBatchsTime;
	double dDrawSkyBatchsTime;
	double dDrawWaterBatchsTime;
	double dDrawPriorBlendBatchsTime;
	double dDrawBlendBatchsTime;
	double dDrawTopmostBatchsTime;
	double dDrawPostBatchsTime;
	double dBuildShaderTime;//����shader�Ļ���ʱ��

	size_t nDrawEarlyZBatchsNum;
	size_t nDrawSolidBatchsNum;
	size_t nDrawGroundBatchsNum;
	size_t nDrawSolidAlphaTestBatchsNum;
	size_t nDrawGrassBatchsNum;
	size_t nDrawAfterSolidBatchsNum;
	size_t nDrawSkyBatchsNum;
	size_t nDrawWaterBatchsNum;
	size_t nDrawPriorBlendBatchsNum;
	size_t nDrawBlendBatchsNum;
	size_t nDrawTopmostBatchsNum;
	size_t nDrawPostBatchsNum;

	size_t nGpuUniformDataUsedNum;
	size_t nGpuUniformDataSumNum;
	double dCopyTempDepthRTTime;
	double dCopyTempColorRTTime;
	size_t nRenderStateUsedNum;
	size_t nRenderStateSumNum;

	size_t nRenderUsedVBNum;
	size_t nRenderSumVBNum;
	size_t nRenderUsedIBNum;
	size_t nRenderSumIBNum;

	size_t nRenderShaderUsedNum;
	size_t nRenderShaderSumNum;
	size_t nRenderBlendUsedNum;
	size_t nRenderBlendSumNum;
	size_t nRenderDrawArrayNum;
	size_t nRenderDrawIndexNum;
	size_t nRenderFrameRTUsedNum;
	size_t nRenderFrameRTSumNum;
 
	size_t nDrawPointNum;
	size_t nDrawLineStripNum;
	size_t nDrawLineLoopNum;
	size_t nDrawLineNum;
	size_t nDrawTriangleNum;
	size_t nDrawTriangleStripNum;
	size_t nDrawTriangleFanNum;

	double dTexUploadGPUTime;//�����ϴ�GPUʱ��
	double dTexBuildGPUTime;
	double dVBUploadGPUTime;//���������ϴ�GPUʱ��
	double dIBUploadGPUTime;//IB�����ϴ�GPUʱ��
	double dConstUploadGPUTime;//�����ϴ�GPUʱ��

	size_t nEffectModelDrawCallNum;// ÿ����������Ч��ģ�͵���������
	size_t nEffectParticleDrawCallNum;// ÿ����������Ч������ģ�͵���������

	size_t nGuiDrawCallNum;//ÿ����ƽ�����Ч����������
};

// ����ͳ���ڴ�����(1���ӵ��ۼ�ֵ)
struct performance_mem_info_t
{
	double dShaderConstData;//��ǰ�Ĵ������ݳ��ȵ�λ
	double dUsedTextureData;//��ǰʹ����ͼ���ݴ�С
	double dLoadFileData;//��ǰ��ȡIO���ݴ�С
	double dLoadTextureData;//��ȡ������ͼ
	double dLoadVBData;//��������
	double dLoadIBData;//������������
};


// ��������ͳ��
struct performance_physics_info_t
{
    size_t nCollisionShapeNum;
    size_t nRigidbodyNum;
    size_t nConstraintNum;
    size_t nRagdollNum;
    double nPhysicsTime;
};


// �����Զ�����չͳ������
struct performance_custom_info_t
{
	size_t nCustomInfo0;
	size_t nCustomInfo1;
	double dCustomInfo0;
	double dCustomInfo1;
};


struct performance_info_t :public performance_cpu_info_t,public performance_gpu_info_t,public performance_mem_info_t,public performance_physics_info_t,public performance_custom_info_t
{
	double dPerDrawCallNum;//һ����ƽ��ÿ֡����DrawCall����
	double dPerDrawTriangleNum;//һ����ƽ��ÿ֡���Ƶ���������Ŀ
	double dFPSTime;//FPSֵ

	double dAvgEffectModelDrawCallNumPerFrame;// ÿ֡���������Ч��ģ�͵�ƽ����������
	double dAvgEffectParticleDrawCallNumPerFrame;// ÿ֡���������Ч������ģ�͵�ƽ����������

	double dAvgGuiDrawCallNumPerFrame;// ÿ֡���ƽ����ƽ����������
};

struct performance_mem_t
{
	unsigned int nUsedTextureData;
	unsigned int nUsedDynamicTextureData;
	unsigned int nUsedTextureDataGui;
	unsigned int nUsedVBData;
	unsigned int nUsedIBData;

};

//#define _ES_3_0_

#endif