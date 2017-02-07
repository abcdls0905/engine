//--------------------------------------------------------------------
// 文件名:		render_shader.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "render.h"
#include "vertex_shader.h"
#include "pixel_shader.h"
#include "../utils/load_archive.h"
#include "../utils/store_archive.h"
#include "../public/auto_mem.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/core_file.h"

// 着色器资源管理

#define SHADER_OBJECT_MARKER 'FXSO'

// 时间编码
static void encode_time(unsigned int& time_high, unsigned int& time_low,
	unsigned int year, unsigned int month, unsigned int day, 
	unsigned int hour, unsigned int minute, unsigned int second)
{
	time_high = ((year & 0xFFFF) << 16) + ((month & 0xFF) << 8) + (day & 0xFF);
	time_low = ((hour & 0xFF) << 16) + ((minute & 0xFF) << 8) + (second & 0xFF);
}

// 获得文件时间
static bool get_file_time(const char* file_name, unsigned int& time_high, 
	unsigned int& time_low)
{
	Assert(file_name != NULL);

	IFileSys::file_time_t ft;

	if (!g_pCore->GetFileSys()->FileTime(file_name, &ft))
	{
		return false;
	}

	encode_time(time_high, time_low, ft.nYear, ft.nMonth, ft.nDay, ft.nHour,
		ft.nMinute, ft.nSecond);

	return true;
}

IVertexShader* Render::LoadVertexShader(const char* file_name, 
	const char* func_name, const char* define)
{
	Assert(file_name != NULL);
	Assert(func_name != NULL);
	Assert(define != NULL);

	int shader_index = LoadShader(file_name, func_name, define);

	if (shader_index < 0)
	{
		return NULL;
	}

	CVertexShader* pShader = 
		(CVertexShader*)m_ShaderList[shader_index].pShader;

	if (NULL == pShader)
	{
		const void* pCode = m_ShaderList[shader_index].pCode;
		unsigned int code_size = m_ShaderList[shader_index].nCodeSize;

		pShader = CreateVertexShader(pCode, code_size);
		//编译失败输出文件名字
		if(pShader)
		{
			pShader->SetFileName(file_name);
			pShader->SetFuncName(func_name);
			pShader->SetDefine(define);

		}
		else
		{
			CORE_TRACE(file_name);
            CORE_TRACE(define);
		}
		m_ShaderList[shader_index].pShader = pShader;
	}
	else if (pShader->GetResourceType() != IResource::TYPE_VERTEX_SHADER)
	{
		return NULL;
	}

	return pShader;
}

IPixelShader* Render::LoadPixelShader(const char* file_name, 
	const char* func_name, const char* define)
{
	Assert(file_name != NULL);
	Assert(func_name != NULL);
	Assert(define != NULL);

	int shader_index = LoadShader(file_name, func_name, define);

	if (shader_index < 0)
	{
		return NULL;
	}

	CPixelShader* pShader = 
		(CPixelShader*)m_ShaderList[shader_index].pShader;

	if (NULL == pShader)
	{
		const void* pCode = m_ShaderList[shader_index].pCode;
		unsigned int code_size = m_ShaderList[shader_index].nCodeSize;

		pShader = CreatePixelShader(pCode, code_size);
		//编译成功
		if(pShader)
		{
			pShader->SetFileName(file_name);
			pShader->SetFuncName(func_name);
			pShader->SetDefine(define);
		}
		else//编译失败
		{
			CORE_TRACE(file_name);
            CORE_TRACE(define);
		}
		m_ShaderList[shader_index].pShader = pShader;
	}
	else if (pShader->GetResourceType() != IResource::TYPE_PIXEL_SHADER)
	{
		return NULL;
	}

	return pShader;
}

int Render::LoadShader(const char* file_name, const char* func_name,
	const char* define)
{
	Assert(file_name != NULL);
	Assert(func_name != NULL);
	Assert(define != NULL);

	shader_file_t* pShaderFile;
	size_t file_index;

	if (FindShaderFileIndex(file_name, file_index))
	{
		pShaderFile = m_ShaderFiles[file_index];
	}
	else
	{
		pShaderFile = CORE_NEW(shader_file_t);
		pShaderFile->strFile = file_name;
		pShaderFile->nHash = GetHashValue(file_name);

		char practice_name[256];

		SafeSprintf(practice_name, sizeof(practice_name), "%s%s",
			m_strShaderPath.c_str(), file_name);

		if (!get_file_time(practice_name, pShaderFile->nTimeHigh, 
			pShaderFile->nTimeLow))
		{
			pShaderFile->nTimeHigh = 0;
			pShaderFile->nTimeLow = 0;
		}

		m_ShaderFiles.push_back(pShaderFile);
		//LoadShaderObject(pShaderFile);
	}

	// 计算相关的文件名字
	char shader_name[1024];

	SafeSprintf(shader_name, sizeof(shader_name), "%s,%s", func_name, define);

	size_t shader_index;

	if (pShaderFile->Shaders.GetData(shader_name, shader_index))
	{
		return (int)shader_index;
	}

	char practice_name[256];

	SafeSprintf(practice_name, sizeof(practice_name), "%s%s",
		m_strShaderPath.c_str(), file_name);

	/*
	FILE* fp = core_file::fopen(practice_name, "rb");

	if (NULL == fp)
	{
		CORE_TRACE("(Render::LoadShader)can't open file");
		CORE_TRACE(file_name);
		return -1;
	}
	*/
	if(!this->LoadCache(practice_name,false))
	{
		CORE_TRACE("(Render::LoadShader)can't open file");
		CORE_TRACE(file_name);
		return -1;
	}

	// 获取原始文件中的内容
	/*
	core_file::fseek(fp, 0, SEEK_END);
	unsigned int file_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);
	*/
	size_t file_size;
	const void* pShaderCode = this->GetCacheData(practice_name,file_size);

	unsigned int define_len = (unsigned int)strlen(define);
	unsigned int code_size = define_len + 2 + file_size;
	char* shader_code = (char*)CORE_ALLOC(code_size);

	memcpy(shader_code, define, define_len);
	shader_code[define_len] = '\n';

	//core_file::fread(shader_code + define_len + 1, sizeof(char), file_size, fp);
    memcpy(shader_code + define_len + 1, pShaderCode , file_size);
	//core_file::fclose(fp);
	shader_code[code_size-1] = '\0';

	shader_index = m_ShaderList.size();

	shader_data_t data;

	data.pCode = (unsigned char*)shader_code;
	data.nCodeSize = code_size;
	data.pShader = NULL;

	m_ShaderList.push_back(data);
	pShaderFile->Shaders.Add(shader_name, shader_index);

	return (int)shader_index;
}

bool Render::FindShaderFileIndex(const char* file_name, size_t& index) const
{
	Assert(file_name != NULL);

	unsigned int hash = GetHashValue(file_name);
	size_t file_num = m_ShaderFiles.size();

	for (size_t i = 0; i < file_num; ++i)
	{
		if ((m_ShaderFiles[i]->nHash == hash)
			&& (stricmp(m_ShaderFiles[i]->strFile.c_str(), file_name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}




/*
bool Render::LoadShaderObject(shader_file_t* pShaderFile)
{
	Assert(pShaderFile != NULL);
	Assert(pShaderFile->Shaders.GetCount() == 0);

	char practice_obj_name[256];

	if (m_bUseShaderModel40)
	{
		SafeSprintf(practice_obj_name, sizeof(practice_obj_name), 
			"%sobj\\%s.sm4", m_strShaderPath.c_str(), 
			pShaderFile->strFile.c_str());
	}
	else
	{
		SafeSprintf(practice_obj_name, sizeof(practice_obj_name), 
			"%sobj\\%s.sm5", m_strShaderPath.c_str(), 
			pShaderFile->strFile.c_str());
	}

	char* file_data = NULL;
	size_t file_size = 0;

	// 先尝试直接从硬盘文件中读取
	FILE* fp = fopen(practice_obj_name, "rb");

	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		file_data = (char*)CORE_ALLOC(file_size);

		if (fread(file_data, sizeof(char), file_size, fp) != file_size)
		{
			CORE_FREE(file_data, file_size);
			return false;
		}

		fclose(fp);
	}
	else
	{
		// 再尝试通过文件系统读取
		fp = core_file::fopen(practice_obj_name, "rb");

		if (NULL == fp)
		{
			return false;
		}

		core_file::fseek(fp, 0, SEEK_END);
		file_size = core_file::ftell(fp);
		core_file::fseek(fp, 0, SEEK_SET);
		file_data = (char*)CORE_ALLOC(file_size);

		if (core_file::fread(file_data, sizeof(char), file_size, fp) 
			!= file_size)
		{
			CORE_FREE(file_data, file_size);
			return false;
		}

		core_file::fclose(fp);
	}

	CLoadArchive arLoad(file_data, file_size);

	try
	{
		// 文件标记
		unsigned int file_mark = arLoad.ReadInt();

		if (file_mark != SHADER_OBJECT_MARKER)
		{
			throw CExceptMsg("shader object version error");
		}

		// 源文件时间
		unsigned int time_high = arLoad.ReadInt();
		unsigned int time_low = arLoad.ReadInt();

		if ((time_high != pShaderFile->nTimeHigh)
			|| (time_low != pShaderFile->nTimeLow))
		{
			// 时间不匹配
			CORE_FREE(file_data, file_size);
			return false;
		}

		size_t shader_num = arLoad.ReadInt();

		for (size_t i = 0; i < shader_num; ++i)
		{
			const char* shader_name = arLoad.ReadString();
			size_t shader_size = 0;
			void* shader_data = arLoad.ReadBinary(shader_size);

			if (0 == shader_size)
			{
				// 文件异常
				CORE_FREE(file_data, file_size);
				return false;
			}

			unsigned char* pCode = (unsigned char*)CORE_ALLOC(shader_size);

			memcpy(pCode, shader_data, shader_size);
			
			size_t shader_index = m_ShaderList.size();
			shader_data_t data;

			data.pCode = pCode;
			data.nCodeSize = (unsigned int)shader_size;
			data.pShader = NULL;
			
			m_ShaderList.push_back(data);
			pShaderFile->Shaders.Add(shader_name, shader_index);
		}
	}
	catch (CExceptMsg& e)
	{
		CORE_TRACE("(DxRender::LoadShaderObject)load failed");
		CORE_TRACE(e.GetMsg());
		CORE_TRACE(pShaderFile->strFile.c_str());
		CORE_FREE(file_data, file_size);
		return false;
	}

	CORE_FREE(file_data, file_size);

	return true;
}

bool Render::SaveShaderObject(shader_file_t* pShaderFile)
{
	Assert(pShaderFile != NULL);

	char practice_obj_name[256];

	if (m_bUseShaderModel40)
	{
		SafeSprintf(practice_obj_name, sizeof(practice_obj_name), 
			"%sobj\\%s.sm4", m_strShaderPath.c_str(), 
			pShaderFile->strFile.c_str());
	}
	else
	{
		SafeSprintf(practice_obj_name, sizeof(practice_obj_name), 
			"%sobj\\%s.sm5", m_strShaderPath.c_str(), 
			pShaderFile->strFile.c_str());
	}

	char buffer[256];
	CStoreArchive arStore(buffer, sizeof(buffer));
	size_t shader_num = pShaderFile->Shaders.GetCount();

	arStore.WriteInt(SHADER_OBJECT_MARKER);
	arStore.WriteInt(pShaderFile->nTimeHigh);
	arStore.WriteInt(pShaderFile->nTimeLow);
	arStore.WriteInt((unsigned int)shader_num);

	shader_container_t::iterator it = pShaderFile->Shaders.Begin();
	shader_container_t::iterator end = pShaderFile->Shaders.End();

	for (; it != end; ++it)
	{
		size_t shader_index = it.GetData();
		unsigned int shader_size = m_ShaderList[shader_index].nCodeSize;

		if (0 == shader_size)
		{
			return false;
		}

		const void* shader_data = m_ShaderList[shader_index].pCode;
		const char* shader_name = it.GetKey();

		arStore.WriteString(shader_name);
		arStore.WriteBinary(shader_data, shader_size);
	}

	FILE* fp = core_file::fopen(practice_obj_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	core_file::fwrite(arStore.GetData(), sizeof(char), arStore.GetLength(), 
		fp);
	core_file::fclose(fp);

	return true;
}

bool Render::SaveAllShaderObjects()
{
	for (size_t i = 0; i < m_ShaderFiles.size(); ++i)
	{
		SaveShaderObject(m_ShaderFiles[i]);
	}
	
	return true;
}
*/

bool Render::ReloadAllShader()
{
	/*
	char practice_name[256];

	for (size_t i = 0; i < m_ShaderFiles.size(); ++i)
	{
		shader_file_t* pShaderFile = m_ShaderFiles[i];

		SafeSprintf(practice_name, sizeof(practice_name), "%s%s",
			m_strShaderPath.c_str(), pShaderFile->strFile.c_str());

		unsigned int time_high;
		unsigned int time_low;

		if (!get_file_time(practice_name, time_high, time_low))
		{
			// 源文件不存在了
			continue;
		}

		if ((time_high == pShaderFile->nTimeHigh)
			&& (time_low == pShaderFile->nTimeLow))
		{
			// 源文件时间没有变化
			continue;
		}

		shader_container_t::iterator it = pShaderFile->Shaders.Begin();
		shader_container_t::iterator end = pShaderFile->Shaders.End();

		for (; it != end; ++it)
		{
			size_t shader_index = it.GetData();
			unsigned char* pOldCode = m_ShaderList[shader_index].pCode;

			if (NULL == pOldCode)
			{
				continue;
			}

			const char* shader_name = it.GetKey();
			char buffer[1024];

			CopyString(buffer, sizeof(buffer), shader_name);

			char* comma = strchr(buffer, ',');

			if (NULL == comma)
			{
				continue;
			}

			*comma = 0;

			const char* func_name = buffer;
			const char* real_define = comma + 1;
			IResource* pShader = m_ShaderList[shader_index].pShader;

			if (NULL == pShader)
			{
				continue;
			}

			const char* profile = "";

			if (profile[0] == 0)
			{
				continue;
			}

			// 编译文件
			ID3D10Blob* shader_code;

			if (!CompileShaderFromFile(practice_name, func_name, real_define, 
				profile, &shader_code))
			{
				// 编译失败
				continue;
			}

			unsigned int code_size = (unsigned int)shader_code->GetBufferSize();
			unsigned char* pNewCode = (unsigned char*)CORE_ALLOC(code_size);

			memcpy(pNewCode, shader_code->GetBufferPointer(), code_size);

			if (pOldCode)
			{
				CORE_FREE(pOldCode, m_ShaderList[shader_index].nCodeSize);
			}

			m_ShaderList[shader_index].pCode = pNewCode;
			m_ShaderList[shader_index].nCodeSize = code_size;

			switch (pShader->GetResourceType())
			{
			case IResource::TYPE_VERTEX_SHADER:
				((CVertexShader*)pShader)->Create(
					shader_code->GetBufferPointer(), 
					(unsigned int)shader_code->GetBufferSize());
				break;
			case IResource::TYPE_PIXEL_SHADER:
				((CPixelShader*)pShader)->Create(
					shader_code->GetBufferPointer(), 
					(unsigned int)shader_code->GetBufferSize());
				break;
			default:
				Assert(0);
				break;
			}

			SAFE_RELEASE(shader_code);
		}

		//SaveShaderObject(pShaderFile);
	}
	*/

	return true;
}

/*
bool Render::GenMacroFromString(const char* define, 
	D3D10_SHADER_MACRO* macros, char* macro_buf, size_t macro_buf_size)
{
	Assert(define != NULL);
	Assert(macros != NULL);
	Assert(macro_buf != NULL);

	size_t len_define = strlen(define);

	if (len_define > macro_buf_size / 2)
	{
		CORE_TRACE("(DxRender::GenMacroFromString)define too long");
		CORE_TRACE(define);
		return false;
	}

	// 语法检查
	{
		const char* p = define;
		int step_state = 0;

		while (*p != 0)
		{
			switch (*p)
			{
			case '=':
				if (step_state != 1)
				{
					return false;
				}
				step_state = 2;
				break;
			case ';':
				if (step_state != 3)
				{
					return false;
				}
				step_state = 4;
				break;
			default:
				if (step_state == 0)
				{
					step_state = 1;
				}
				else if (step_state == 2)
				{
					step_state = 3;
				}
				else if (step_state == 4)
				{
					step_state = 1;
				}
				break;
			}

			p++;
		}

		if (step_state != 4)
		{
			CORE_TRACE("(DxRender::GenMacroFromString)define error");
			CORE_TRACE(define);
			return false;
		}
	}

	// 语句拆分
	{
		const char* ps = define;
		char* pd = macro_buf;
		int step_state = 0;
		int cur_macro = 0;

		while (*ps != 0)
		{
			switch(*ps)
			{
			case '=':
				step_state = 2;
				*pd++ = 0;
				break;
			case ';':
				step_state = 4;
				*pd++ = 0;
				break;
			default:
				if (step_state == 0)
				{
					macros[cur_macro].Name = pd;
					step_state = 1;
				}
				else if (step_state == 2)
				{
					macros[cur_macro++].Definition = pd;
					step_state = 3;
				}
				else if (step_state == 4)
				{
					macros[cur_macro].Name = pd;
					step_state = 1;
				}
				break;
			}

			*pd++ = *ps++;
		}

		if (step_state != 4)
		{
			return false;
		}
	}

	return true;
}

bool Render::CompileShaderFromFile(const char* file_name, 
	const char* func_name, const char* define, const char* profile, 
	ID3D10Blob** shader_code)
{
	Assert(file_name != NULL);
	Assert(func_name != NULL);
	Assert(define != NULL);
	Assert(profile != NULL);
	Assert(shader_code != NULL);

	FILE* fp = core_file::fopen(file_name, "rb");

	if (NULL == fp)
	{
		CORE_TRACE("(DxRender::CompileShaderFromFile)can't open file");
		CORE_TRACE(file_name);
		return false;
	}

	// 获取原始文件中的内容
	core_file::fseek(fp, 0, SEEK_END);
	unsigned int file_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);

	TAutoMem<char, 256, TCoreAlloc> auto_buf(file_size);
	char* file_data = auto_buf.GetBuffer();

	core_file::fread(file_data, sizeof(char), file_size, fp);
	core_file::fclose(fp);

	D3D10_SHADER_MACRO macros[100];
	char macro_buffer[1024];

	memset(macros, 0, sizeof(macros));
	memset(macro_buffer, 0, sizeof(macro_buffer));

	if (define[0] != 0)
	{
		GenMacroFromString(define, macros, macro_buffer, sizeof(macro_buffer));
	}

	ID3D10Blob* pCode = NULL;
	ID3D10Blob* pError = NULL;

	// 编译原始文件中的内容
	HRESULT hr = D3DX11CompileFromMemory(file_data, file_size, file_name,
		macros, NULL, func_name, profile, D3D10_SHADER_OPTIMIZATION_LEVEL3, 
		0, NULL, &pCode, &pError, NULL);

	if (pError != NULL)
	{
		CORE_TRACE("(DxRender::CompileShaderFromFile)compile error");
		CORE_TRACE(file_name);
		CORE_TRACE((char*)pError->GetBufferPointer());
		pError->Release();
		pError = NULL;
	}

	if (FAILED(hr))
	{
		CORE_TRACE("(DxRender::CompileShaderFromFile)compile failed");
		CORE_TRACE(file_name);
		return false;
	}

	*shader_code = pCode;

	return true;
}
*/
