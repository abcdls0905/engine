//--------------------------------------------------------------------
// 文件名:		helper.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年7月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "helper.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"


bool Helper_MakeViewPlanes(const FmMat4& view, 
	const FmMat4& proj, FmPlane* planes, size_t plane_num)
{
	Assert(planes != NULL);
	Assert(plane_num == 4);
	
	FmVec3 vWorld[8];
	FmVec3 vScreen[8];

	vScreen[0].x = -1.0F; vScreen[0].y = +1.0F; vScreen[0].z = 0.0F;
	vScreen[1].x = -1.0F; vScreen[1].y = +1.0F; vScreen[1].z = 1.0F;
	vScreen[2].x = -1.0F; vScreen[2].y = -1.0F; vScreen[2].z = 0.0F;
	vScreen[3].x = -1.0F; vScreen[3].y = -1.0F; vScreen[3].z = 1.0F;
	vScreen[4].x = +1.0F; vScreen[4].y = -1.0F; vScreen[4].z = 0.0F;
	vScreen[5].x = +1.0F; vScreen[5].y = -1.0F; vScreen[5].z = 1.0F;
	vScreen[6].x = +1.0F; vScreen[6].y = +1.0F; vScreen[6].z = 0.0F;
	vScreen[7].x = +1.0F; vScreen[7].y = +1.0F; vScreen[7].z = 1.0F;

	for(int i = 0; i < 8; i++ )
	{
       FmVec3Unproject(&vWorld[i],&vScreen[i],&proj,&view,NULL);
	}
	//FmVec3UnprojectArray(vWorld, sizeof(FmVec3),
	//	vScreen, sizeof(FmVec3), NULL, &proj, &view, NULL, 8);
	FmPlaneFromPoints(&planes[0], &vWorld[0], &vWorld[1], &vWorld[6]);
	FmPlaneFromPoints(&planes[1], &vWorld[6], &vWorld[7], &vWorld[4]);
	FmPlaneFromPoints(&planes[2], &vWorld[4], &vWorld[5], &vWorld[2]);
	FmPlaneFromPoints(&planes[3], &vWorld[0], &vWorld[2], &vWorld[3]);
	FmPlaneNormalize(&planes[0], &planes[0]);
	FmPlaneNormalize(&planes[1], &planes[1]);
	FmPlaneNormalize(&planes[2], &planes[2]);
	FmPlaneNormalize(&planes[3], &planes[3]);

	return true;
}

bool Helper_ScreenShot(const char* name)
{
//	Assert(name != NULL);
//
//	const char* dot = strrchr(name, '.');
//
//	if (NULL == dot)
//	{
//		return false;
//	}
//
//	D3DX11_IMAGE_FILE_FORMAT format;
//
//	if (stricmp(dot, ".bmp") == 0)
//	{
//		format = D3DX11_IFF_BMP;
//	}
//	else if (stricmp(dot, ".jpg") == 0)
//	{
//		format = D3DX11_IFF_JPG;
//	}
//	else if (stricmp(dot, ".png") == 0)
//	{
//		format = D3DX11_IFF_PNG;
//	}
//	else if (stricmp(dot, ".dds") == 0)
//	{
//		format = D3DX11_IFF_DDS;
//	}
//	else
//	{
//		return false;
//	}
//
//	ID3D11DeviceContext* pDeviceContext = NULL;
//	ID3D11RenderTargetView* pRenderTarget = NULL;
//	ID3D11Resource* pResource = NULL;
//
//	pDevice->GetImmediateContext(&pDeviceContext);
//	pDeviceContext->OMGetRenderTargets(1, &pRenderTarget, NULL);
//	pRenderTarget->GetResource(&pResource);
//
//	HRESULT hr = D3DX11SaveTextureToFile(pDeviceContext, pResource, format, 
//		name);
//
//	pResource->Release();
//	pRenderTarget->Release();
//	pDeviceContext->Release();
//
//	return SUCCEEDED(hr);
	return false;
}

bool Helper_GetTextureAlpha(fm_uint nTex, unsigned int x, unsigned int y, 
	unsigned int& alpha)
{
//	D3D11_TEXTURE2D_DESC td;
//	
//	pTex->GetDesc(&td);
//	
//	if ((x >= td.Width) || (y >= td.Height))
//	{
//		return false;
//	}
//
//	ID3D11Device* pDevice;
//	ID3D11DeviceContext* pDeviceContext;
//	
//	pTex->GetDevice(&pDevice);
//	pDevice->GetImmediateContext(&pDeviceContext);
//	
//	D3D11_MAPPED_SUBRESOURCE mapped_data;
//	HRESULT hr = pDeviceContext->Map(pTex, 0, D3D11_MAP_READ, 0, &mapped_data);
//	
//	if (FAILED(hr))
//	{
//		pDeviceContext->Release();
//		pDevice->Release();
//		return false;
//	}
//	
//	bool succeed = false;
//	
//	if (DXGI_FORMAT_BC3_UNORM == td.Format)
//	{
//		// DXT5
//		size_t block_x = x / 4;
//		size_t block_y = y / 4;
//		unsigned char* pdata = (unsigned char*)mapped_data.pData;
//
//		pdata += mapped_data.RowPitch * block_y + block_x * 16;
//
//		unsigned int alpha0 = pdata[0];
//		unsigned int alpha1 = pdata[1];
//		uint64_t info = *(uint64_t*)pdata;
//		size_t index = (y % 4) * 4 + (x % 4);
//		unsigned int v = unsigned int((info >> (index * 3 + 16)) & 0x7);
//
//		if (alpha0 > alpha1)
//		{
//			switch (v)
//			{
//			case 0:
//				alpha = alpha0;
//				break;
//			case 1:
//				alpha = alpha1;
//				break;
//			case 2:
//				alpha = (6 * alpha0 + 1 * alpha1 + 3) / 7;
//				break;
//			case 3:
//				alpha = (5 * alpha0 + 2 * alpha1 + 3) / 7;
//				break;
//			case 4:
//				alpha = (4 * alpha0 + 3 * alpha1 + 3) / 7;
//				break;
//			case 5: 
//				alpha = (3 * alpha0 + 4 * alpha1 + 3) / 7;
//				break;
//			case 6:
//				alpha = (2 * alpha0 + 5 * alpha1 + 3) / 7;
//				break;
//			case 7:
//				alpha = (1 * alpha0 + 6 * alpha1 + 3) / 7;
//				break;
//			default:
//				Assert(0);
//				break;
//			}
//		}
//		else
//		{
//			switch (v)
//			{
//			case 0:
//				alpha = alpha0;
//				break;
//			case 1:
//				alpha = alpha1;
//				break;
//			case 2:
//				alpha = (4 * alpha0 + 1 * alpha1 + 2) / 5;
//				break;
//			case 3:
//				alpha = (3 * alpha0 + 2 * alpha1 + 2) / 5;
//				break;
//			case 4:
//				alpha = (2 * alpha0 + 3 * alpha1 + 2) / 5;
//				break;
//			case 5: 
//				alpha = (1 * alpha0 + 4 * alpha1 + 2) / 5;
//				break;
//			case 6:
//				alpha = 0;
//				break;
//			case 7:
//				alpha = 255;
//				break;
//			default:
//				Assert(0);
//				break;
//			}
//		}
//
//		succeed = true;
//	}
//	else if ((DXGI_FORMAT_B8G8R8A8_UNORM == td.Format)
//		|| (DXGI_FORMAT_R8G8B8A8_UNORM == td.Format))
//	{
//		unsigned char* pdata = (unsigned char*)mapped_data.pData;
//		
//		pdata += mapped_data.RowPitch * y + x * sizeof(unsigned int);
//
//		unsigned int color = *(unsigned int*)pdata;
//
//		alpha = (color >> 24) & 0xFF;
//		succeed = true;
//	}
//	else if (DXGI_FORMAT_BC1_UNORM == td.Format)
//	{
//		// DXT1
//		size_t block_x = x / 4;
//		size_t block_y = y / 4;
//		unsigned char* pdata = (unsigned char*)mapped_data.pData;
//		
//		pdata += mapped_data.RowPitch * block_y + block_x * 8;
//		
//		unsigned short* color = (unsigned short*)pdata;
//
//		if (color[0] <= color[1])
//		{
//			// 包含ALPHA信息
//			unsigned int info = *(unsigned int*)(pdata + 4);
//			size_t index = (y % 4) * 4 + (x % 4);
//			unsigned int color_index = (info >> (index * 2)) & 0x3;
//
//			if (color_index == 3)
//			{
//				// 颜色索引为3表示透明
//				alpha = 0;
//			}
//			else
//			{
//				alpha = 255;
//			}
//			
//			succeed = true;
//		}
//	}
//	else if (DXGI_FORMAT_BC2_UNORM == td.Format)
//	{
//		// DXT3
//		size_t block_x = x / 4;
//		size_t block_y = y / 4;
//		unsigned char* pdata = (unsigned char*)mapped_data.pData;
//		
//		pdata += mapped_data.RowPitch * block_y + block_x * 16;
//		
//		uint64_t info = *(uint64_t*)pdata;
//		size_t index = (y % 4) * 4 + (x % 4);
//		unsigned int v = unsigned int((info >> (index * 4)) & 0xF);
//		
//		alpha = v * 16;
//		succeed = true;
//	}
//	else
//	{
//		alpha = 255;
//		succeed = true;
//	}
//	
//	pDeviceContext->Unmap(pTex, 0);
//	pDeviceContext->Release();
//	pDevice->Release();
//	
//	return succeed;
	return false;
}
