//--------------------------------------------------------------------
// 文件名:		var_list.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年1月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_VAR_LIST_H
#define _PUBLIC_VAR_LIST_H

#include "../public/var_type.h"
#include "../public/i_var_list.h"
#include "../public/i_var.h"
#include "../public/portable.h"

// 参数集模板

class TVarListAlloc
{
public:
    TVarListAlloc() {}
    ~TVarListAlloc() {}

    // 申请内存
    void* Alloc(size_t size) { return NEW char[size]; }
    // 释放内存
    void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
    // 交换
    void Swap(TVarListAlloc& src) {}
};

template<size_t DATA_SIZE, size_t BUFFER_SIZE, typename ALLOC = TVarListAlloc>
class TVarList: public IVarList
{
private:
    struct var_data_t
    {
        int nType;
        union
        {
            bool boolValue;
            int intValue;
            int64_t int64Value;
            float floatValue;
            double doubleValue;
            size_t stringValue;
            size_t widestrValue;
            void* pointerValue;
            size_t userdataValue;
            struct
            {
                unsigned int objectIdent;
                unsigned int objectSerial;
            };
        };
    };

public:
    TVarList()
    {
        Assert(DATA_SIZE > 0);
        Assert(BUFFER_SIZE > 0);

        m_pData = m_DataStack;
        m_nDataSize = DATA_SIZE;
        m_nDataUsed = 0;
        m_pBuffer = m_BufferStack;
        m_nBufferSize = BUFFER_SIZE;
        m_nBufferUsed = 0;
    }

    TVarList(const TVarList& src)
    {
        Assert(DATA_SIZE > 0);
        Assert(BUFFER_SIZE > 0);

        m_pData = m_DataStack;
        m_nDataSize = DATA_SIZE;
        m_nDataUsed = 0;
        m_pBuffer = m_BufferStack;
        m_nBufferSize = BUFFER_SIZE;
        m_nBufferUsed = 0;
        InnerAppend(src, 0, src.GetCount());
    }

    virtual ~TVarList()
    {
        if (m_nDataSize > DATA_SIZE)
        {
            m_Alloc.Free(m_pData, m_nDataSize * sizeof(var_data_t));
        }

        if (m_nBufferSize > BUFFER_SIZE)
        {
            m_Alloc.Free(m_pBuffer, m_nBufferSize);
        }
    }

    TVarList& operator=(const TVarList& src)
    {
        if (m_nDataSize > DATA_SIZE)
        {
            m_Alloc.Free(m_pData, m_nDataSize * sizeof(var_data_t));
        }

        if (m_nBufferSize > BUFFER_SIZE)
        {
            m_Alloc.Free(m_pBuffer, m_nBufferSize);
        }

        m_pData = m_DataStack;
        m_nDataSize = DATA_SIZE;
        m_nDataUsed = 0;
        m_pBuffer = m_BufferStack;
        m_nBufferSize = BUFFER_SIZE;
        m_nBufferUsed = 0;
        InnerAppend(src, 0, src.GetCount());

        return *this;
    }

    // 合并
    virtual bool Concat(const IVarList& src)
    {
        InnerAppend(src, 0, src.GetCount());

        return true;
    }

    // 添加
    virtual bool Append(const IVarList& src, size_t start, size_t count)
    {
        if (start >= src.GetCount())
        {
            return false;
        }

        size_t end = start + count;

        if (end > src.GetCount())
        {
            return false;
        }

        InnerAppend(src, start, end);

        return true;
    }

    // 清空
    virtual void Clear()
    {
        m_nDataUsed = 0;
        m_nBufferUsed = 0;
    }

    // 是否为空
    virtual bool IsEmpty() const
    {
        return (0 == m_nDataUsed);
    }

    // 数据数量
    virtual size_t GetCount() const 
    {
        return m_nDataUsed;
    }

    // 数据类型
    virtual int GetType(size_t index) const
    {
        if (index >= m_nDataUsed)
        {
            return 0;
        }

        return m_pData[index].nType;
    }

    // 添加数据
    virtual bool AddBool(bool value)
    {
        var_data_t* p = AddVarData();

        p->nType = VTYPE_BOOL;
        p->boolValue = value;

        return true;
    }

    virtual bool AddInt(int value)
    {
        var_data_t* p = AddVarData();

        p->nType = VTYPE_INT;
        p->intValue = value;

        return true;
    }

    virtual bool AddInt64(int64_t value)
    {
        var_data_t* p = AddVarData();

        p->nType = VTYPE_INT64;
        p->int64Value = value;

        return true;
    }

    virtual bool AddFloat(float value)
    {
        var_data_t* p = AddVarData();

        p->nType = VTYPE_FLOAT;
        p->floatValue = value;

        return true;
    }

    virtual bool AddDouble(double value)
    {
        var_data_t* p = AddVarData();

        p->nType = VTYPE_DOUBLE;
        p->doubleValue = value;

        return true;
    }

    virtual bool AddString(const char* value)
    {
        Assert(value != NULL);

        var_data_t* p = AddVarData();

        p->nType = VTYPE_STRING;
        p->stringValue = m_nBufferUsed;

        const size_t value_size = strlen(value) + 1;
        char* v = AddBuffer(value_size);

        memcpy(v, value, value_size);

        return true;
    }

    virtual bool AddWideStr(const wchar_t* value)
    {
        Assert(value != NULL);

        var_data_t* p = AddVarData();

        p->nType = VTYPE_WIDESTR;
        p->widestrValue = m_nBufferUsed;

        const size_t value_size = (wcslen(value) + 1) * sizeof(wchar_t);
        char* v = AddBuffer(value_size);

        memcpy(v, value, value_size);

        return true;
    }

    virtual bool AddObject(const PERSISTID& value)
    {
        var_data_t* p = AddVarData();

        p->nType = VTYPE_OBJECT;
        p->objectIdent = value.nIdent;
        p->objectSerial = value.nSerial;

        return true;
    }

    virtual bool AddPointer(void* value)
    {
        var_data_t* p = AddVarData();

        p->nType = VTYPE_POINTER;
        p->pointerValue = value;

        return true;
    }

    virtual bool AddUserData(const void* pdata, size_t size)
    {
        var_data_t* p = AddVarData();

        p->nType = VTYPE_USERDATA;
        p->userdataValue = m_nBufferUsed;

        const size_t value_size = IVar::GetRawUserDataSize(size);
        char* v = AddBuffer(value_size);

        IVar::InitRawUserData(v, pdata, size);

        return true;
    }

    virtual bool AddRawUserData(void* value)
    {
        return AddUserData(IVar::GetUserDataPointer(value), 
            IVar::GetUserDataSize(value));
    }

    // 获得数据
    virtual bool BoolVal(size_t index) const
    {
        if (index >= m_nDataUsed)
        {
            return false;
        }

        switch (m_pData[index].nType)
        {
        case VTYPE_BOOL:
            return m_pData[index].boolValue;
        case VTYPE_INT:
            return (m_pData[index].intValue != 0);
        case VTYPE_INT64:
            return (m_pData[index].int64Value != 0);
        case VTYPE_STRING:
            return (*(m_pBuffer + m_pData[index].stringValue) != 0);
        default:
            break;
        }

        return false;
    }

    virtual int IntVal(size_t index) const
    {
        if (index >= m_nDataUsed)
        {
            return 0;
        }

        switch (m_pData[index].nType)
        {
        case VTYPE_BOOL:
            return int(m_pData[index].boolValue);
        case VTYPE_INT:
            return m_pData[index].intValue;
        case VTYPE_INT64:
            return int(m_pData[index].int64Value);
        case VTYPE_FLOAT:
            return int(m_pData[index].floatValue);
        case VTYPE_DOUBLE:
            return int(m_pData[index].doubleValue);
        case VTYPE_STRING:
            return atoi(m_pBuffer + m_pData[index].stringValue);
        default:
            break;
        }

        return 0;
    }

    virtual int64_t Int64Val(size_t index) const
    {
        if (index >= m_nDataUsed)
        {
            return 0;
        }

        switch (m_pData[index].nType)
        {
        case VTYPE_BOOL:
            return int64_t(m_pData[index].boolValue);
        case VTYPE_INT:
            return int64_t(m_pData[index].intValue);
        case VTYPE_INT64:
            return m_pData[index].int64Value;
        case VTYPE_FLOAT:
            return int64_t(m_pData[index].floatValue);
        case VTYPE_DOUBLE:
            return int64_t(m_pData[index].doubleValue);
        case VTYPE_STRING:
            return Port_StringToInt64(m_pBuffer + m_pData[index].stringValue);
        default:
            break;
        }

        return 0;
    }

    virtual float FloatVal(size_t index) const
    {
        if (index >= m_nDataUsed)
        {
            return 0.0f;
        }

        switch (m_pData[index].nType)
        {
        case VTYPE_BOOL:
            return float(m_pData[index].boolValue);
        case VTYPE_INT:
            return float(m_pData[index].intValue);
        case VTYPE_INT64:
            return float(m_pData[index].int64Value);
        case VTYPE_FLOAT:
            return m_pData[index].floatValue;
        case VTYPE_DOUBLE:
            return float(m_pData[index].doubleValue);
        case VTYPE_STRING:
            return float(atof(m_pBuffer + m_pData[index].stringValue));
        default:
            break;
        }

        return 0.0f;
    }

    virtual double DoubleVal(size_t index) const
    {
        if (index >= m_nDataUsed)
        {
            return 0.0;
        }

        switch (m_pData[index].nType)
        {
        case VTYPE_BOOL:
            return double(m_pData[index].boolValue);
        case VTYPE_INT:
            return double(m_pData[index].intValue);
        case VTYPE_INT64:
            return double(m_pData[index].int64Value);
        case VTYPE_FLOAT:
            return double(m_pData[index].floatValue);
        case VTYPE_DOUBLE:
            return m_pData[index].doubleValue;
        case VTYPE_STRING:
            return atof(m_pBuffer + m_pData[index].stringValue);
        default:
            break;
        }

        return 0.0;
    }

    virtual const char* StringVal(size_t index) const
    {
        if (index >= m_nDataUsed)
        {
            return "";
        }

        switch (m_pData[index].nType)
        {
        case VTYPE_STRING:
            return m_pBuffer + m_pData[index].stringValue;
        default:
            break;
        }

        return "";
    }

    virtual const wchar_t* WideStrVal(size_t index) const
    {
        if (index >= m_nDataUsed)
        {
            return L"";
        }

        switch (m_pData[index].nType)
        {
        case VTYPE_WIDESTR:
            return (wchar_t*)(m_pBuffer + m_pData[index].widestrValue);
        default:
            break;
        }

        return L"";
    }

    virtual PERSISTID ObjectVal(size_t index) const
    {
        if (index >= m_nDataUsed)
        {
            return PERSISTID();
        }

        switch (m_pData[index].nType)
        {
        case VTYPE_OBJECT:
            return PERSISTID(m_pData[index].objectIdent, 
                m_pData[index].objectSerial);
        default:
            break;
        }

        return PERSISTID();
    }

    virtual void* PointerVal(size_t index) const
    {
        if (index >= m_nDataUsed)
        {
            return NULL;
        }

        if (m_pData[index].nType != VTYPE_POINTER)
        {
            return NULL;
        }

        return m_pData[index].pointerValue;
    }

    virtual const void* UserDataVal(size_t index, size_t& size) const
    {
        if (index >= m_nDataUsed)
        {
            size = 0;
            return NULL;
        }

        if (m_pData[index].nType != VTYPE_USERDATA)
        {
            size = 0;
            return NULL;
        }

        char* p = m_pBuffer + m_pData[index].userdataValue;

        size = IVar::GetUserDataSize(p);

        return IVar::GetUserDataPointer(p);
    }

    virtual void* RawUserDataVal(size_t index) const
    {
        if (index >= m_nDataUsed)
        {
            return NULL;
        }

        if (m_pData[index].nType != VTYPE_USERDATA)
        {
            return NULL;
        }

        return m_pBuffer + m_pData[index].userdataValue;
    }

private:
    var_data_t* AddVarData()
    {
        if (m_nDataUsed >= m_nDataSize)
        {
            size_t new_size = m_nDataSize * 2;
            var_data_t* p = (var_data_t*)m_Alloc.Alloc(
                new_size * sizeof(var_data_t));

            memcpy(p, m_pData, m_nDataUsed * sizeof(var_data_t));

            if (m_nDataSize > DATA_SIZE)
            {
                m_Alloc.Free(m_pData, m_nDataSize * sizeof(var_data_t));
            }

            m_pData = p;
            m_nDataSize = new_size;
        }

        return m_pData + m_nDataUsed++;
    }

    char* AddBuffer(size_t need_size)
    {
        size_t new_used = m_nBufferUsed + need_size;

        if (new_used > m_nBufferSize)
        {
            size_t new_size = m_nBufferSize * 2;

            if (new_used > new_size)
            {
                new_size = new_used * 2;
            }

            char* p = (char*)m_Alloc.Alloc(new_size);

            memcpy(p, m_pBuffer, m_nBufferUsed);

            if (m_nBufferSize > BUFFER_SIZE)
            {
                m_Alloc.Free(m_pBuffer, m_nBufferSize);
            }

            m_pBuffer = p;
            m_nBufferSize = new_size;
        }

        char* ret = m_pBuffer + m_nBufferUsed;

        m_nBufferUsed = new_used;

        return ret;
    }

    void InnerAppend(const IVarList& src, size_t start, size_t end)
    {
        for (size_t i = start; i < end; ++i)
        {
            switch (src.GetType(i))
            {
            case VTYPE_BOOL:
                AddBool(src.BoolVal(i));
                break;
            case VTYPE_INT:
                AddInt(src.IntVal(i));
                break;
            case VTYPE_INT64:
                AddInt64(src.Int64Val(i));
                break;
            case VTYPE_FLOAT:
                AddFloat(src.FloatVal(i));
                break;
            case VTYPE_DOUBLE:
                AddDouble(src.DoubleVal(i));
                break;
            case VTYPE_STRING:
                AddString(src.StringVal(i));
                break;
            case VTYPE_WIDESTR:
                AddWideStr(src.WideStrVal(i));
                break;
            case VTYPE_OBJECT:
                AddObject(src.ObjectVal(i));
                break;
            case VTYPE_POINTER:
                AddPointer(src.PointerVal(i));
                break;
            case VTYPE_USERDATA:
                {
                    size_t size;
                    const void* pdata = src.UserDataVal(i, size);

                    AddUserData(pdata, size);
                }
                break;
            default:
                Assert(0);
                break;
            }
        }
    }

private:
    ALLOC m_Alloc;
    var_data_t m_DataStack[DATA_SIZE];
    var_data_t* m_pData;
    size_t m_nDataSize;
    size_t m_nDataUsed;
    char m_BufferStack[BUFFER_SIZE];
    char* m_pBuffer;
    size_t m_nBufferSize;
    size_t m_nBufferUsed;
};

typedef TVarList<8, 128> CVarList;

#endif // _PUBLIC_VAR_LIST_H

