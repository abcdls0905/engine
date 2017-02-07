#pragma once
#include "../visual/i_res_loader.h"
#include "../public/fast_str.h"

class TimeAxis;
struct sObjectData;
class KeyFrameController;

class CObjectLoader :
	public IResLoader
{
public:
	static CObjectLoader* NewInstance();

public:
	CObjectLoader();
	virtual ~CObjectLoader();

	// 释放
	virtual void Release();
	// 获得名字
	virtual const char* GetName();
	// 加载
	virtual bool Load(bool async);
	// 创建
	virtual bool Create(bool async);
	// 结束异步创建 
	virtual bool EndCreate(int category, bool succeed);

	// 资源主体
	void SetOwner(TimeAxis* pOwner);
	// 设置对象及模型文件名
	void SetFileName(sObjectData* pObject, const char* file_name);
private:
	CObjectLoader(const CObjectLoader&);
	CObjectLoader& operator=(const CObjectLoader&);

private:
	sObjectData* m_pObject;
	result_string m_strFileName;

	TimeAxis* m_pOwner;
};
