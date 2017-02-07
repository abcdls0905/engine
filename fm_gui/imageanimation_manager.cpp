//--------------------------------------------------------------------
// 文件名:		ImageAnimationManager.cpp
// 内  容:		聊天或其它使用到的动画表情管理器
// 说  明:		
// 创建日期:	2008年8月22日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "imageanimation_manager.h"
#include "../tools/string_util.h"
#include "../visual/i_painter.h"
#include "../visual/vis_utils.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_file.h"
#include "../extern/rapidxml/rapidxml.hpp"
#include "../public/portable.h"
using namespace rapidxml;

//#include "../sdk/tinyxml/tinyxml.h"

const long g_lAnimationOutTime = 5000;

/// entity: ImageAnimationManager
/// \brief 动画表情管理器实体
DECLARE_ENTITY(ImageAnimationManager, 0, IEntity);

/// property: string FilePath
/// \brief 相对于资源的文件路径
DECLARE_PROPERTY(const char*, ImageAnimationManager, FilePath, 
    GetFilePath, SetFilePath);
#if 0

/// method: table GetAnimationList();
/// \brief  获取所有动画的名称
DECLARE_METHOD_N(ImageAnimationManager, GetAnimationList);

/// method: bool StartAnimation(const char* pName);
/// \brief  启动动画,默认创建好就启动
/// \param pName动画名
DECLARE_METHOD_1(bool, ImageAnimationManager, StartAnimation, const char*);

/// method: bool SetAnimationLoop(string name, bool,value)
/// \brief 设置动画是否循环,默认为是
DECLARE_METHOD_2(bool, ImageAnimationManager, SetAnimationLoop, const char*, 
	bool);

/// method: float GetAnimationTime(const char* pName);
/// \brief  获取此动画的当前播放时间
/// \param pName动画名
DECLARE_METHOD_1(float, ImageAnimationManager, GetAnimationCurTime, 
	const char*);
/// method: float SetAnimationTime(const char* pName, int millionseconds);
/// \brief  设置动画当前播放时间
/// \param pName动画名
DECLARE_METHOD_2(bool, ImageAnimationManager, SetAnimationCurTime, 
	const char*, int);
DECLARE_METHOD_1(float, ImageAnimationManager, GetAnimationTotalTime, 
	const char*);

/// method: bool Register(const char* pName, const char* pLuaFile, const char* pLuaFunc, PERSISTID id)
/// \brief  注册动画事件
/// \param pName动画名
/// \param pLuaFile 回调脚本名
/// \param pLuaFunc 回调函数名
/// \param id 注册对像ID
DECLARE_METHOD_4(bool, ImageAnimationManager, Register, const char*, 
	const char*, const char*, PERSISTID);

/// method: bool UnRegister(const char* pName, const char* pLuaFile, const char* pLuaFunc, PERSISTID id);
/// \brief  去除注册动画事件
/// \param pName动画名
/// \param pLuaFile 回调脚本名
/// \param pLuaFunc 回调函数名
/// \param id 注册对像ID
DECLARE_METHOD_4(bool, ImageAnimationManager, UnRegister, const char*, 
	const char*, const char*, PERSISTID);

// \brief 清理动画
DECLARE_METHOD_0(bool, ImageAnimationManager, Clear);

// \brief 重载动画
DECLARE_METHOD_0(bool, ImageAnimationManager, Load);

/// event: int on_animation_event(object self, string ani_name, string mode, luafile, luafunc, id)
/// \brief 动画事件
/// \param self本控件
/// \param animation 动画名称
DECLARE_EVENT(on_animation_event);
#endif

ImageAnimationManager::ImageAnimationManager()
{
    m_szFilePath = "skin\\ImageAnimations.xml";
	m_pPainter = NULL;
}

ImageAnimationManager::~ImageAnimationManager()
{
	for (size_t i = 0; i < m_CallBacks.size(); ++i)
	{
		CORE_DELETE(m_CallBacks[i]);
	}
}
class xml_parser_node;
class xml_parser : public rapidxml::xml_document<char>
{
	typedef TAutoMem<char, 256> XMLStringBuffer;
	typedef xml_document<char> BaseType;
public:
	xml_parser():m_auto_buf(NULL){}
	~xml_parser(){ if(m_auto_buf) delete m_auto_buf; }
	bool Parse(const char* szPathFile);
	xml_parser_node* first_node(const char* Name){ return (xml_parser_node*)BaseType::first_node(Name); }
private:
	XMLStringBuffer* m_auto_buf;
	static const char* const EMPTY_STRING;

};

class xml_parser_node:public rapidxml::xml_node<char>
{
	typedef xml_node<char> BaseType;
public:
	const char* Attribute(const char* name, const char* szDefaultValue = STRING_DEFALUT_VALUE);
	xml_parser_node* FirstChildElement(const char* Name){ return (xml_parser_node*)BaseType::first_node(Name); }
	xml_parser_node* NextSiblingElement(const char* Name = NULL);
protected:
	static const char* STRING_DEFALUT_VALUE;
};
const char* xml_parser_node::STRING_DEFALUT_VALUE = NULL;

const char* xml_parser_node::Attribute(const char* name, const char* szDefaultValue/*=STRING_DEFALUT_VALUE*/)
{
	xml_attribute<char>* attribute = BaseType::first_attribute(name);
	return attribute? attribute->value(): szDefaultValue;
}
xml_parser_node* xml_parser_node::NextSiblingElement(const char* Name/* = NULL*/)
{
	return (xml_parser_node*)BaseType::next_sibling(Name);
}


const char* const xml_parser::EMPTY_STRING = "";
bool xml_parser::Parse( const char* szPathFile )
{
	using namespace rapidxml;

	if(!szPathFile) return false;

	FILE* fp = core_file::fopen(szPathFile, "rb");

	if (NULL == fp)
	{
		return false;
	}

	core_file::fseek(fp, 0, SEEK_END);

	size_t file_size = core_file::ftell(fp);

	core_file::fseek(fp, 0, SEEK_SET);

	if(m_auto_buf) delete m_auto_buf;
	m_auto_buf = new XMLStringBuffer(file_size + 1);

	char* buffer = m_auto_buf->GetBuffer();

	if (core_file::fread(buffer, sizeof(char), file_size, fp) != file_size)
	{
		core_file::fclose(fp);
		return false;
	}

	core_file::fclose(fp);

	buffer[file_size] = 0;

	try
	{
		this->parse<parse_declaration_node>(buffer);
	}
	catch (parse_error& e)
	{
		CORE_TRACE("(load_xml_doc parse xml error");
		CORE_TRACE(e.what());
		return false;
	}

	return true;
}

void ImageAnimationManager::SetFilePath(const char* pFile)
{
    m_szFilePath = pFile;
}

const char* ImageAnimationManager::GetFilePath()
{
    return m_szFilePath.c_str();
}

bool ImageAnimationManager::Init(const IVarList& args)
{
	const char* pPainterName = args.StringVal(0);

	m_pPainter = (IPainter*)GetCore()->GetInterface(pPainterName);

	if( !m_pPainter )
	{
		m_pPainter = (IPainter*)GetCore()->GetInterface("Painter");
	}

	if (NULL == m_pPainter)
	{
		return false;
	}

    Load();

    GetCore()->AddExecute(this);

	// 设置成单件实体
	//GetCore()->SetSingle(this);

	return true;
}

bool ImageAnimationManager::Shut()
{
	if (m_pPainter)
	{
		for (size_t i = 0; i < m_vFaces.size(); ++i)
		{
			for (size_t j = 0; j < m_vFaces[i].vFrames.size(); ++j)
			{
				m_pPainter->ReleaseGlyph(m_vFaces[i].vFrames[j].ImageID);
				//if (m_vFaces[i].IsPng)
				//{
				//	m_pPainter->ReleaseGlyph(m_vFaces[i].vFrames[j].ImageID);
				//}
				//else
				//{
				//	m_pPainter->ReleaseTexture(m_vFaces[i].vFrames[j].ImageID);
				//}
			}
		}
	}
    else
    {
        return true;
    }

	m_vFaces.clear();

    GetCore()->RemoveExecute(this);

    SAFE_RELEASE(m_pPainter);

	return true;
}

// 更新动画动态加载释放计时
bool ImageAnimationManager::UpdateDynamicCount(const char* pName)
{
    if (NULL == pName)
    {
        return false;
    }

	//long index = m_mNameToIndexMap[pName] - 1;

    //if (index < 0 || index >= (long)m_vFaces.size())
    //{
        // 未找到则返回
    //    return false;
    //}

	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return false;
	}

    FaceItem& faceitem = m_vFaces[index];

    faceitem.lLastDrawTime = Port_GetTickCount();

    return true;
}

// 初始化控制器
bool ImageAnimationManager::InitControlParam(const char* pName, 
	ControlParam& control)
{
    if (NULL == pName)
    {
        return false;
    }

    //long index = m_mNameToIndexMap[pName] - 1;

    //if (index < 0 || index >= (long)m_vFaces.size())
    //{
        // 未找到则返回
    //    return false;
    //}

	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return false;
	}

    FaceItem& faceitem = m_vFaces[index];

    control = faceitem.control;

    return true;
}

bool ImageAnimationManager::UpdateAnimationByName(const char* pName, 
	ControlParam& control, long lDelta_Time)
{
    Assert(pName != NULL);
	//if (NULL == pName)
    //{
    //    return false;
    //}

    //long index = m_mNameToIndexMap[pName];

    //if (index <= 0)
    //{
        // 未找到则返回
    //    return false;
    //}
	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return false;
	}

	return UpdateAnimation(m_vFaces[index], control, lDelta_Time);
    //return UpdateAnimation(index-1, control, lDelta_Time);
}

//bool ImageAnimationManager::UpdateAnimation(size_t index, 
//	FaceItem& faceItem, long lDelta_Time)
//{
  //  return UpdateAnimation(index, faceItem.control, lDelta_Time);
//}

bool ImageAnimationManager::UpdateAnimation(FaceItem& faceitem, 
	ControlParam& control, long lDelta_Time)
{
    //if (index < 0 || index >= m_vFaces.size())
    //{
        // 未找到则返回
    //    return false;
    //}

    //FaceItem& faceitem = m_vFaces[index];

    if (!faceitem.IsCreated)
    {
        //未创建的话就直接返回
        return false;
    }

    // 检查是否超时
    if (faceitem.IsDynamicCreate && faceitem.IsCreated)
    {
        long lCurTime = Port_GetTickCount();

        if (lCurTime - faceitem.lLastDrawTime > g_lAnimationOutTime)
        {
            // 如果已超时,则删除帧图，释放资源
			size_t frame_num = faceitem.vFrames.size();

            for (size_t j = 0; j < frame_num; ++j)
            {
				m_pPainter->ReleaseGlyph(faceitem.vFrames[j].ImageID);
                //if (faceitem.IsPng)
                //{
                  //  m_pPainter->ReleaseGlyph(faceitem.vFrames[j].ImageID);
                //}
                //else
                //{
                //    m_pPainter->ReleaseTexture(faceitem.vFrames[j].ImageID);
                //}
            }

			size_t part_motive_num = faceitem.vPartMotive.size();

            for (size_t j = 0; j < part_motive_num; ++j)
            {
				m_pPainter->ReleaseGlyph(faceitem.vPartMotive[j].ImageID);
                //if (faceitem.IsPng)
                //{
                //    m_pPainter->ReleaseGlyph(faceitem.vPartMotive[j].ImageID);
                //}
                //else
                //{
                //    m_pPainter->ReleaseTexture(faceitem.vPartMotive[j].ImageID);
                //}
            }

            faceitem.IsCreated = false;
            control.lTimeCount = 0;
            return false;
        }
    }

    // 未超时，则继续处理动画
    bool bResult = false;

    if (control.athwart)
    {
        // 逆序
        bResult = AthwardUpdate(faceitem, control, lDelta_Time);
    }
    else
    {
        // 正序
        bResult = OrderUpdate(faceitem, control, lDelta_Time);
    }

    if (! bResult)
    {
        // bResult == false ,表示动画已达终点，则直接返回
        return false;
    }

    if (faceitem.IsHasRotateMovite)
    {
        // 获取当前转角
		size_t rotate_motive_num = faceitem.vRotateMotive.size();
        size_t j = 0;

        for (; j < rotate_motive_num; ++j)
        {
            if (control.lTimeCount <= faceitem.vRotateMotive[j].starttime)
            {
                if (j == 0)
                {
                    // 则设成初态
                    control.nCurAngle = faceitem.vRotateMotive[0].angle;
                    break;
                }
                else if (j < rotate_motive_num)
                {
                    // 在第二个到最后一个之前
                    control.nCurAngle = 
                        (faceitem.vRotateMotive[j].angle - faceitem.vRotateMotive[j - 1].angle) * 
                        (control.lTimeCount - faceitem.vRotateMotive[j - 1].starttime) / 
                        (faceitem.vRotateMotive[j].starttime - faceitem.vRotateMotive[j - 1].starttime) + 
                        faceitem.vRotateMotive[j - 1].angle;
                    break;
                }
            }
        }

        if (j >= rotate_motive_num)
        {
            // 超出最后则设成最后
            control.nCurAngle = 
				faceitem.vRotateMotive[rotate_motive_num - 1].angle;
        }
    }

    if (faceitem.IsHasAlphaMotive)
    {
        size_t alpha_motive_num = faceitem.vAlphaMotive.size();
		size_t j = 0;

        for (; j < alpha_motive_num; ++j)
        {
            if (control.lTimeCount <= faceitem.vAlphaMotive[j].starttime)
            {
                if (j == 0)
                {
                    control.nCurAlpha = faceitem.vAlphaMotive[0].alpha;
                    break;
                }
                else if (j < alpha_motive_num)
                {
                    // 在第二个到最后一个之前
                    control.nCurAlpha = 
                        (faceitem.vAlphaMotive[j].alpha - faceitem.vAlphaMotive[j - 1].alpha) * 
                        (control.lTimeCount - faceitem.vAlphaMotive[j - 1].starttime) / 
                        (faceitem.vAlphaMotive[j].starttime - faceitem.vAlphaMotive[j - 1].starttime) + 
                        faceitem.vAlphaMotive[j - 1].alpha;
                    break;
                }
            }
        }

        if (j >= alpha_motive_num)
        {
            // 如果超时则设到最后
            control.nCurAlpha = 
				faceitem.vAlphaMotive[alpha_motive_num - 1].alpha;
        }
    }

    if (faceitem.IsHasScaleMotive)
    {
        // 获取当前转角
		size_t scale_motive_num = faceitem.vScaleMotive.size();
        size_t j = 0;

        for (; j < scale_motive_num; ++j)
        {
            if (control.lTimeCount <= faceitem.vScaleMotive[j].starttime)
            {
                if (j == 0)
                {
                    // 在放缩之前，则设成放缩初态
                    control.fCurXScaleValue = faceitem.vScaleMotive[0].xvalue;
                    control.fCurYScaleValue = faceitem.vScaleMotive[0].yvalue;
                    break;
                }
                else if (j < scale_motive_num)
                {
                    // 在第二个到最后一个之前
                    control.fCurXScaleValue = 
                        (faceitem.vScaleMotive[j].xvalue - faceitem.vScaleMotive[j-1].xvalue) * 
                        (control.lTimeCount - faceitem.vScaleMotive[j-1].starttime) / 
                        (faceitem.vScaleMotive[j].starttime - faceitem.vScaleMotive[j-1].starttime) + 
                        faceitem.vScaleMotive[j-1].xvalue;
                    control.fCurYScaleValue = 
                        (faceitem.vScaleMotive[j].yvalue - faceitem.vScaleMotive[j-1].yvalue) * 
                        (control.lTimeCount - faceitem.vScaleMotive[j-1].starttime) / 
                        (faceitem.vScaleMotive[j].starttime - faceitem.vScaleMotive[j-1].starttime) + 
                        faceitem.vScaleMotive[j-1].yvalue;
                    break;
                }
            }
        }

        if (j >= scale_motive_num)
        {
            // 在放缩时间后，则你放缩末态
            control.fCurXScaleValue = 
				faceitem.vScaleMotive[scale_motive_num - 1].xvalue;
            control.fCurYScaleValue = 
				faceitem.vScaleMotive[scale_motive_num - 1].yvalue;
        }
    }

    if (!faceitem.vSubAnimation.empty())
	{
		size_t sub_animation_num = faceitem.vSubAnimation.size();
		SubAnimation* sub_animations = &faceitem.vSubAnimation[0];
		ControlParam* sub_controls = &control.vSubControls[0];

		for (size_t j = 0; j < sub_animation_num; ++j)
		{
			if (sub_animations[j].bFollowPlay)
			{
				UpdateAnimationByName(sub_animations[j].szSubAniName.c_str(), 
					sub_controls[j], lDelta_Time);
			}
		}
	}

    return true;
}

// 正序动画
bool ImageAnimationManager::OrderUpdate(FaceItem& faceitem, 
	ControlParam& control, int delt_time)
{
    control.lTimeCount += delt_time;
    control.cevent = EVENT_PLAY_NONE;

    int nCurIndex = -1;

    for (size_t i = 0; i < faceitem.vFrames.size(); ++i)
    {
        if (control.lTimeCount < faceitem.vFrames[i].lEndTime)
        {
            nCurIndex = i;
            control.lCurIndex = i;
            break;
        }
    }

    // 关键帧动画当前帧结束
    if (nCurIndex < 0)
    {
        if (control.loop)
        {
            // 循环则重新开始
            control.cevent = EVENT_PLAY_LOOP;

            SetToBegin(faceitem, control);
            return false;
        }
        else
        {
            // 不循环则设到最终状态不再处理
            control.cevent = EVENT_PLAY_END;
            SetToEnd(faceitem, control);
            return false;
        }

        /*if (faceitem.IsDynamicCreate)
        {
            DoCallBack(faceitem.szFaceName.c_str(), "end");
            DoCallBack(faceitem.szFaceName.c_str(), "start");
        }*/
    }

    if (faceitem.vMoveMotive.size() > 1)
    {
        // 获取当前所在的位移目标点
        bool bFound = false;
        long curindex = 0;

        for (size_t k = 1; k < faceitem.vMoveMotive.size(); ++k)
        {
            if (control.lTimeCount < faceitem.vMoveMotive[k].lTimeIndex)
            {
                curindex = k - 1;
                bFound = true;
                break;
            }
        }

        // 已经超出运动时间,则不处理
        if (bFound)
        {
            long divx = faceitem.vMoveMotive[curindex + 1].x - faceitem.vMoveMotive[curindex].x;
            long divy = faceitem.vMoveMotive[curindex + 1].y - faceitem.vMoveMotive[curindex].y;

            float bili = 
                (control.lTimeCount - faceitem.vMoveMotive[curindex].lTimeIndex) * 
                1.0f /  
                (faceitem.vMoveMotive[curindex + 1].lTimeIndex - faceitem.vMoveMotive[curindex].lTimeIndex);

            control.x = (int)(divx * bili) +  faceitem.vMoveMotive[curindex].x;

            control.y = (int)(divy * bili) +  faceitem.vMoveMotive[curindex].y;
        }
        else
        {
            // 运动动画配置不正确，可能超出了关键帧动画的时间范围
            // Assert(0);
            int k=0;
        }
    }

    return true;
}

// 逆序动画
bool ImageAnimationManager::AthwardUpdate(FaceItem& faceitem, 
	ControlParam& control, int delt_time)
{
    control.lTimeCount -= (long)delt_time;
    control.cevent = EVENT_PLAY_NONE;

    // 当前关键帧动画结束,关键帧动画结束表示整个动画结束
    if (control.lCurIndex == 0)
    {
        if (control.lTimeCount < 0)
        {
            if (control.loop)
            {
                // 如果循环则设到最终状态
                control.cevent = EVENT_PLAY_LOOP;
                SetToEnd(faceitem, control);
                return false;
            }
            else
            {
                // 不循环则设回最初状态，返回不再处理
                control.cevent = EVENT_PLAY_END;
                SetToBegin(faceitem, control);
                return false;
            }

            /*if (faceitem.IsDynamicCreate)
            {
                DoCallBack(faceitem.szFaceName.c_str(), "end");
                DoCallBack(faceitem.szFaceName.c_str(), "start");
            }*/
        }
    }
    else if (control.lTimeCount <= faceitem.vFrames[control.lCurIndex-1].lEndTime)
    {
        // 获取下一个关键帧
        long lNexeIndex  = 
            (control.lCurIndex - 1 + faceitem.vFrames.size()) % faceitem.vFrames.size();

        // 如果当前帧为0，在逆向运动时是最后一帧，表示运动到尾部，结束
        control.lCurIndex = lNexeIndex;
    }

    // 处理移动动画，移动动画，可以比关键帧动画短，或相等，但不能比关键帧动画长
    if (faceitem.vMoveMotive.size() > 1)
    {
        // 获取当前所在的位移目标点
        bool bFound = false;
        long curindex = faceitem.vMoveMotive.size() - 2;

        for(int k = faceitem.vMoveMotive.size()-1; k >= 0 ; --k)
        {
            if (control.lTimeCount >= faceitem.vMoveMotive[k].lTimeIndex)
            {
                curindex = k;

                bFound = true;
                break;
            }
        }

        // 已经超出运动时间,则不处理
        if (bFound && curindex < (int)faceitem.vMoveMotive.size()-1)
        {
            long divx = faceitem.vMoveMotive[curindex + 1].x - faceitem.vMoveMotive[curindex].x;
            long divy = faceitem.vMoveMotive[curindex + 1].y - faceitem.vMoveMotive[curindex].y;

            float bili = 
                (control.lTimeCount - faceitem.vMoveMotive[curindex].lTimeIndex) * 
                1.0f /  
                (faceitem.vMoveMotive[curindex + 1].lTimeIndex - faceitem.vMoveMotive[curindex].lTimeIndex);

            control.x = (int)(divx * bili) +  faceitem.vMoveMotive[curindex].x;

            control.y = (int)(divy * bili) +  faceitem.vMoveMotive[curindex].y;
        }
        else
        {
            // 运动动画配置不正确，可能超出了关键帧动画的时间范围
            // Assert(0);
        }
    }

    return true;
}

void ImageAnimationManager::Execute(float seconds)
{
    /*if (seconds > 0.1f)
    {
        // 大于0.1秒表示不正常播放，不处理
        return;
    }*/

    if (NULL == m_pPainter)
    {
        return;
    }

	size_t face_num = m_vFaces.size();

	if (0 == face_num)
	{
		return;
	}

	FaceItem* face_items = &m_vFaces[0];
	long Delta_Time = (long)(seconds * 1000);

	for (size_t i = 0; i < face_num; ++i)
	{
		if (face_items[i].IsCreated)
		{
			UpdateAnimation(face_items[i], face_items[i].control, Delta_Time);
		}
	}
}

// 设到开始状态
bool ImageAnimationManager::SetToBegin(FaceItem& faceitem, ControlParam& control)
{
    control.lCurIndex = 0;
    control.lTimeCount = 0;

    // 运动初始化
    if (faceitem.vMoveMotive.size() > 0)
    {
        control.x = faceitem.vMoveMotive[0].x;
        control.y = faceitem.vMoveMotive[0].y;
    }
    else
    {
        control.x = 0;
        control.y = 0;
    }

    if (faceitem.IsHasScaleMotive && faceitem.vScaleMotive.size() > 0)
    {
        control.fCurXScaleValue = faceitem.vScaleMotive[0].xvalue;
        control.fCurYScaleValue = faceitem.vScaleMotive[0].yvalue;
    }

    if (faceitem.IsHasAlphaMotive && faceitem.vAlphaMotive.size() > 0)
    {
        control.nCurAlpha = faceitem.vAlphaMotive[0].alpha;
    }

    if (faceitem.IsHasRotateMovite && faceitem.vRotateMotive.size() > 0)
    {
        control.nCurAngle = faceitem.vRotateMotive[0].angle;
    }

    // 循环设到最后，准备重新开始
    if (faceitem.vMoveMotive.size() > 0)
    {
        control.x = faceitem.vMoveMotive[0].x;
        control.y = faceitem.vMoveMotive[0].y;
    }

    for (size_t j = 0; j < faceitem.vSubAnimation.size(); ++j)
    {
        if (faceitem.vSubAnimation[j].bFollowPlay)
        {
            //long index = m_mNameToIndexMap[faceitem.vSubAnimation[j].szSubAniName];

            //if (index <= 0)
            //{
                // 未找到则返回
            //    continue;
            //}

            //if (index > (long)m_vFaces.size())
            //{
                // 超出范围则返回
            //    continue;
            //}
			size_t index;

			if (!m_mNameToIndexMap.GetData(
				faceitem.vSubAnimation[j].szSubAniName.c_str(), index))
			{
				// 未找到则返回
				continue;
			}

			if (index >= m_vFaces.size())
			{
				// 超出范围则返回
				continue;
			}

            //FaceItem& subfaceitem = m_vFaces[index - 1];
			FaceItem& subfaceitem = m_vFaces[index];

            control.vSubControls[j].loop = control.loop;
            control.vSubControls[j].athwart = control.athwart;

            SetToBegin(subfaceitem, control.vSubControls[j]);

        }
    }
    return true;
}

// 设到结束状态
bool ImageAnimationManager::SetToEnd(FaceItem& faceitem, ControlParam& control)
{
    control.lCurIndex = faceitem.vFrames.size() - 1;
    control.lTimeCount = faceitem.control.lTime;

    if (faceitem.IsHasScaleMotive)
    {
        size_t count = faceitem.vScaleMotive.size();
        if (count > 0)
        {
            control.fCurXScaleValue = faceitem.vScaleMotive[count - 1].xvalue;
            control.fCurYScaleValue = faceitem.vScaleMotive[count - 1].yvalue;
        }
    }

    if (faceitem.IsHasAlphaMotive)
    {
        size_t count = faceitem.vAlphaMotive.size();
        if (count > 0)
        {
            control.nCurAlpha = faceitem.vAlphaMotive[count - 1].alpha;
        }
    }

    if (faceitem.IsHasRotateMovite)
    {
        size_t count = faceitem.vRotateMotive.size();
        if (count > 0)
        {
            control.nCurAngle = faceitem.vRotateMotive[count - 1].angle;
        }
    }

    // 循环设到最后，准备重新开始
    if (faceitem.vMoveMotive.size() > 0)
    {
        control.x = faceitem.vMoveMotive[faceitem.vMoveMotive.size() - 1].x;
        control.y = faceitem.vMoveMotive[faceitem.vMoveMotive.size() - 1].y;
    }

    for (size_t j = 0; j<faceitem.vSubAnimation.size(); ++j)
    {
        if (faceitem.vSubAnimation[j].bFollowPlay)
        {
            //long index = m_mNameToIndexMap[faceitem.vSubAnimation[j].szSubAniName];

            //if (index <= 0)
            //{
                // 未找到则返回
            //    continue;
            //}

            //if (index > (long)m_vFaces.size())
            //{
                // 超出范围则返回
            //    continue;
            //}

			size_t index;

			if (!m_mNameToIndexMap.GetData(
				faceitem.vSubAnimation[j].szSubAniName.c_str(), index))
			{
				// 未找到则返回
				continue;
			}

			if (index >= m_vFaces.size())
			{
				// 超出范围则返回
				continue;
			}

            //FaceItem& subfaceitem = m_vFaces[index - 1];
			FaceItem& subfaceitem = m_vFaces[index];

            control.vSubControls[j].loop = control.loop;
            control.vSubControls[j].athwart = control.athwart;

            SetToEnd(subfaceitem, control.vSubControls[j]);
        }
    }
    return true;
}

bool ImageAnimationManager::IsAnimation(const char* pName)
{
	if (NULL == pName)
	{
		return false;
	}

	return m_mNameToIndexMap.Exists(pName);

	//long index = m_mNameToIndexMap[pName];

	//if (index <= 0)
	//{
		// 未找到则返回
	//	return false;
	//}

	//return true;
}

// 是否是混合动画
bool ImageAnimationManager::IsBlendAnimation(const char* pName)
{
    if (NULL == pName)
    {
        return false;
    }

    //long index = m_mNameToIndexMap[pName];

    //if (index <= 0)
    //{
        // 未找到则返回
    //    return false;
    //}

    //if (index > (long)m_vFaces.size())
    //{
        // 超出范围则返回
    //    return false;
    //}
	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return false;
	}
	
	if (index >= m_vFaces.size())
	{
		// 超出范围则返回
		return false;
	}

    //FaceItem& faceitem = m_vFaces[index - 1];
	FaceItem& faceitem = m_vFaces[index];

    if (faceitem.nBlendMode == 0)
    {
        return false;
    }

    return true;
}

bool ImageAnimationManager::DrawAnimation(const char* pName, long x, long y, 
    rect_t& rect, int blend_color)
{
    if (NULL == pName)
    {
        return false;
    }

    //long index = m_mNameToIndexMap[pName];

    //if (index <= 0)
    //{
        // 未找到则返回
    //    return false;
    //}

    //if (index > (long)m_vFaces.size())
    //{
        // 超出范围则返回
    //    return false;
    //}
	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return false;
	}

	if (index >= m_vFaces.size())
	{
		// 超出范围则返回
		return false;
	}

    //ControlParam& ctrlparam = m_vFaces[index - 1].control;
	ControlParam& ctrlparam = m_vFaces[index].control;

    return DrawAnimation(pName, x, y, rect, ctrlparam, blend_color);
}

bool ImageAnimationManager::DrawAnimation(const char* pName, long x, long y, 
	rect_t& rect, ControlParam& control, int blend_color)
{
	if (NULL == pName)
	{
		return false;
	}

	//long index = m_mNameToIndexMap[pName];

	//if (index <= 0)
	//{
		// 未找到则返回
	//	return false;
	//}

	//if (index > (long)m_vFaces.size())
	//{
		// 超出范围则返回
	//	return false;
	//}

	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return false;
	}

	if (index >= m_vFaces.size())
	{
		// 超出范围则返回
		return false;
	}

	//FaceItem& faceitem = m_vFaces[index - 1];
	FaceItem& faceitem = m_vFaces[index];

    if (faceitem.IsDynamicCreate && !faceitem.IsCreated)
    {
        // 如果是动态创建且没有创建的,则需创建
        if (faceitem.vFrames.size() > 0)
        {
            // 如果此动画未被创建，则需创建
            if (!DynamicCreateAnmation(faceitem))
            {
                // 动态创建失败则返回，不能绘了
                return false;
            }
        }
    }

	FaceFrame& frame = faceitem.vFrames[control.lCurIndex];

	if (NULL == m_pPainter)
	{
		return false;
	}

    // 获取保存当前透明度
    int old_alpha = m_pPainter->GetBlendAlpha();

    int blend_alpha = ((blend_color & 0xff000000) >> 24);

    if (faceitem.IsHasAlphaMotive || blend_alpha != 255)
    {
        // 如果父界面透明了，子界面也应透明
		int new_alpha = control.nCurAlpha * old_alpha / 255;

        new_alpha = new_alpha * blend_alpha / 255;

        m_pPainter->SetBlendAlpha(new_alpha);
    }

    int x1 = x;
    int x2 = x + rect.right - rect.left;
    int y1 = y;
    int y2 = y + rect.bottom - rect.top;

    FmMat4 mat;

    FmMat4Identity(&mat);

    if (faceitem.IsHasRotateMovite)
    {
        // 旋转
        float cx = float(x1 + x2) * 0.5f;
        float cy = float(y1 + y2) * 0.5f;

		FmVec3 vec3Angle(0, 0, (float)(control.nCurAngle * 2 * 3.1415926f / 360.0f));
		FmVec3 vec3Position(cx, -cy, 0.0f);
        VisUtil_MatrixRotationTranslation(&mat, 
			&vec3Angle,
            &vec3Position);

        m_pPainter->SetRotateTransform(&mat);

        x1 -= int(cx);
        x2 -= int(cx);
        y1 -= int(cy);
        y2 -= int(cy);
    }

    if (faceitem.IsHasScaleMotive)
    {
        // 放缩
        float cx = float(x1 + x2) * 0.5f;
        float cy = float(y1 + y2) * 0.5f;

        float widthd2 = float(x2 - x1) * control.fCurXScaleValue / 2.0f;
        float heightd2 = float(y2 - y1) * control.fCurYScaleValue / 2.0f;

        x1 = int(cx - widthd2);
        x2 = int(cx + widthd2);
        y1 = int(cy - heightd2);
        y2 = int(cy + heightd2);
    }

    x1 += control.x;
    x2 += control.x;
    y1 += control.y;
    y2 += control.y;

    if (faceitem.IsPng)
    {
        m_pPainter->DrawAddGlyph(x1, y1, x2, y2, 0, 0, faceitem.lWidth, 
			faceitem.lHeight, frame.ImageID, faceitem.nBlendMode, blend_color);
    }
	else
    {
        m_pPainter->DrawGlyph(x1, y1, x2, y2, 0, 0, faceitem.lWidth, 
			faceitem.lHeight, frame.ImageID);
    }

    for (size_t p = 0; p < faceitem.vPartMotive.size(); ++p)
    {
        PartMotive& partItem = faceitem.vPartMotive[p];

        rect_t rShowPart = {0, 0, partItem.width, partItem.height};

        float ratio = (control.lTimeCount - partItem.lStartTime) 
			/ (float)(partItem.lEndTime - partItem.lStartTime);

        if (ratio > 1.0f || partItem.lEndTime == partItem.lStartTime)
        {
            ratio = 1.0f;
        }
        else if (ratio < 0)
        {
            ratio = 0;
        }

        switch(partItem.mode)
        {
        case PART_LTTOALL:  // 从左上到右下显现
            rShowPart.right = (long)(rShowPart.right * ratio);
            rShowPart.bottom = (long)(rShowPart.bottom * ratio);
            break;
        case PART_LTOALL:   // 从左到右显现
            rShowPart.right = (long)(rShowPart.right * ratio);
            break;
        case PART_LBTOALL:  // 从左下到右上显现
            rShowPart.right = (long)(rShowPart.right * ratio);
            rShowPart.top = (long)((1.0f - ratio) * rShowPart.bottom);
            break;
        case PART_BTOALL:   // 从下到上显现
            rShowPart.top = (long)((1.0f - ratio) * rShowPart.bottom);
            break;
        case PART_BRTOALL:  // 从右下到左上显现
            rShowPart.left = (long)((1.0f - ratio) * rShowPart.right);
            rShowPart.top = (long)((1.0f - ratio) * rShowPart.bottom);
            break;
        case PART_RTOALL:   // 从右到左显现
            rShowPart.left = (long)((1.0f - ratio) * rShowPart.right);
            break;
        case PART_TRTOALL:  // 从右上到左下显现
            rShowPart.left = (long)((1.0f - ratio) * rShowPart.right);
            rShowPart.bottom = (long)(rShowPart.bottom * ratio);
            break;
        case PART_TTOALL:   // 从上到下显现
            rShowPart.bottom = (long)(rShowPart.bottom * ratio);
            break;
        case PART_MTOLRSIDE:// 从中间到两旁显现
            rShowPart.left = (long)(rShowPart.right - ratio * rShowPart.right) / 2;
            rShowPart.right = (long)(rShowPart.right - rShowPart.left);
            break;
        case PART_MTOTBSIDE:// 从中间到上下显现
            rShowPart.top = (long)(rShowPart.bottom - ratio * rShowPart.bottom) / 2;
            rShowPart.bottom = (long)(rShowPart.bottom - rShowPart.top);
            break;
        case PART_CTOALL:   // 从中间到四周显现
            rShowPart.left = (long)(rShowPart.right - ratio * rShowPart.right) / 2;
            rShowPart.right = (long)(rShowPart.right - rShowPart.left);
            rShowPart.top = (long)(rShowPart.bottom - ratio * rShowPart.bottom) / 2;
            rShowPart.bottom = (long)(rShowPart.bottom - rShowPart.top);
        default:
            break;
        }

        long left = partItem.x + x + rShowPart.left;
        long top = partItem.y + y + rShowPart.top;
        long right = left + rShowPart.right - rShowPart.left;
        long bottom = top + rShowPart.bottom - rShowPart.top;

        if (faceitem.IsPng)
        {
            m_pPainter->DrawAddGlyph(left, top, right, bottom, rShowPart.left, 
				rShowPart.top, rShowPart.right, rShowPart.bottom, 
				partItem.ImageID, 0);
        }
        else
        {
            m_pPainter->DrawGlyph(left, top, right, bottom, rShowPart.left, 
				rShowPart.top, rShowPart.right, rShowPart.bottom, 
				partItem.ImageID);
        }
    }

    if (faceitem.IsDynamicCreate)
    {
        // 动态创建才需要记录
        faceitem.lLastDrawTime = Port_GetTickCount();
    }

	// 结束旋转
	if (faceitem.IsHasRotateMovite)
	{
		m_pPainter->SetRotateTransform(NULL);
	}

    // 恢复之前透明度
    if (faceitem.IsHasAlphaMotive || blend_alpha != 255)
    {
        m_pPainter->SetBlendAlpha(old_alpha);
    }

    // 开始子特效绘制
    for (size_t i = 0; i < faceitem.vSubAnimation.size(); ++i)
    {
        // 在可显示时间范围内才显示
        if (control.lTimeCount >= faceitem.vSubAnimation[i].start_time && 
            control.lTimeCount < faceitem.vSubAnimation[i].end_time)
        {        
            int basex = x;
            int basey = y;

            point_t pt = GetAnimationSize(
                faceitem.vSubAnimation[i].szSubAniName.c_str());

            if (faceitem.vSubAnimation[i].bFollowMove)
            {
                basex += control.x;
                basey += control.y;
            }

            rect_t rt = {faceitem.vSubAnimation[i].x + basex, 
                faceitem.vSubAnimation[i].y + basey, 
                faceitem.vSubAnimation[i].x + pt.x + basex, 
                faceitem.vSubAnimation[i].y + pt.y + basey};

            DrawSubAnimation(faceitem.vSubAnimation[i].szSubAniName.c_str(), 
                faceitem.vSubAnimation[i].x + basex, 
				faceitem.vSubAnimation[i].y + basey, 
                rt, control, i, faceitem.vSubAnimation[i], mat);
        }
    }

    return true;
}


bool ImageAnimationManager::DrawSubAnimation(const char* pName, long x, 
	long y, rect_t& rect, ControlParam& parentcontrol, 
	const size_t control_index, SubAnimation& subconfig, D3DXMATRIX& matRotate)
{
    if (NULL == pName)
    {
        return false;
    }

    //long index = m_mNameToIndexMap[pName];

    //if (index <= 0)
    //{
        // 未找到则返回
    //    return false;
    //}

    //if (index > (long)m_vFaces.size())
    //{
        // 超出范围则返回
    //    return false;
    //}

	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return false;
	}

	if (index >= m_vFaces.size())
	{
		// 超出范围则返回
		return false;
	}

    //FaceItem& faceitem = m_vFaces[index - 1];
	FaceItem& faceitem = m_vFaces[index];

    ControlParam* pControl = &faceitem.control;

    if (subconfig.bFollowPlay)
    {
        pControl = &parentcontrol.vSubControls[control_index];
    }

    ControlParam &control = *pControl;

    if (faceitem.IsDynamicCreate && !faceitem.IsCreated)
    {
        // 如果是动态创建且没有创建的,则需创建
        if (faceitem.vFrames.size() > 0)
        {
            // 如果此动画未被创建，则需创建
            if (!DynamicCreateAnmation(faceitem))
            {
                // 动态创建失败则返回，不能绘了
                return false;
            }
        }
    }

    FaceFrame& frame = faceitem.vFrames[control.lCurIndex];

    if (!m_pPainter)
    {
        return false;
    }

    // 透明
    // 获取保存当前透明度
    int oldalpha = m_pPainter->GetBlendAlpha();

    if (subconfig.bFollowAlpha)
    {
        if (faceitem.IsHasAlphaMotive)
        {
            m_pPainter->SetBlendAlpha((control.nCurAlpha * 
				parentcontrol.nCurAlpha * oldalpha) / (255 * 255));
        }
        else
        {
            m_pPainter->SetBlendAlpha(parentcontrol.nCurAlpha * oldalpha / 255);
        }
    }
    else
    {
        m_pPainter->SetBlendAlpha(control.nCurAlpha * oldalpha / 255);
    }

    int x1 = x;
    int x2 = x + rect.right - rect.left;
    int y1 = y;
    int y2 = y + rect.bottom - rect.top;

    // 旋转
    FmMat4 mat;
    FmMat4Identity(&mat);
    if (subconfig.bFollowRotate)
    {
        if (faceitem.IsHasRotateMovite)
        {
            // 旋转
            float cx = float(x1 + x2) * 0.5f;
            float cy = float(y1 + y2) * 0.5f;

			FmVec3 vec3Angle(0, 0, (float)(control.nCurAngle * 2 * FM_PI / 360.0f));
			FmVec3 vec3Pos(cx, -cy, 0.0f);
            VisUtil_MatrixRotationTranslation(&mat, 
				&vec3Angle,
                &vec3Pos);

            FmMat4Multiply(&mat, &matRotate, &mat);
            m_pPainter->SetRotateTransform(&mat);

            x1 -= int(cx);
            x2 -= int(cx);
            y1 -= int(cy);
            y2 -= int(cy);
        }
        else
        {
            m_pPainter->SetRotateTransform(&matRotate);
        }
    }
    else
    {
        if (faceitem.IsHasRotateMovite)
        {
            // 旋转
            float cx = float(x1 + x2) * 0.5f;
            float cy = float(y1 + y2) * 0.5f;

            D3DXMATRIX mat;

			FmVec3 vec3Angle(0, 0, (float)(control.nCurAngle * 2 * 3.1415926 / 360.0f));
			FmVec3 vec3Position(cx, -cy, 0.0f);

            VisUtil_MatrixRotationTranslation(&mat, 
				&vec3Angle,
                &vec3Position);

            m_pPainter->SetRotateTransform(&mat);

            x1 -= int(cx);
            x2 -= int(cx);
            y1 -= int(cy);
            y2 -= int(cy);
        }
    }

    // 放缩
    if (subconfig.bFollowScale)
    {
        if (faceitem.IsHasScaleMotive)
        {
            // 放缩
            float cx = float(x1 + x2) * 0.5f;
            float cy = float(y1 + y2) * 0.5f;

            float widthd2 = float(x2 - x1) * control.fCurXScaleValue 
				* parentcontrol.fCurXScaleValue / 2.0f;
            float heightd2 = float(y2 - y1) * control.fCurYScaleValue 
				* parentcontrol.fCurYScaleValue / 2.0f;

            x1 = int(cx - widthd2);
            x2 = int(cx + widthd2);
            y1 = int(cy - heightd2);
            y2 = int(cy + heightd2);
        }
        else
        {
            // 放缩
            float cx = float(x1 + x2) * 0.5f;
            float cy = float(y1 + y2) * 0.5f;

            float widthd2 = float(x2 - x1) * parentcontrol.fCurXScaleValue / 2.0f;
            float heightd2 = float(y2 - y1) * parentcontrol.fCurYScaleValue / 2.0f;

            x1 = int(cx - widthd2);
            x2 = int(cx + widthd2);
            y1 = int(cy - heightd2);
            y2 = int(cy + heightd2);
        }
    }
    else
    {
        if (faceitem.IsHasScaleMotive)
        {
            // 放缩
            float cx = float(x1 + x2) * 0.5f;
            float cy = float(y1 + y2) * 0.5f;

            float widthd2 = float(x2 - x1) * control.fCurXScaleValue / 2.0f;
            float heightd2 = float(y2 - y1) * control.fCurYScaleValue / 2.0f;

            x1 = int(cx - widthd2);
            x2 = int(cx + widthd2);
            y1 = int(cy - heightd2);
            y2 = int(cy + heightd2);
        }
    }

    x1 += control.x;
    x2 += control.x;
    y1 += control.y;
    y2 += control.y;

    if (faceitem.IsPng)
    {
        m_pPainter->DrawAddGlyph(x1, y1, x2, y2, 0, 0, faceitem.lWidth, 
            faceitem.lHeight, frame.ImageID, faceitem.nBlendMode);
    }
    else
    {
        m_pPainter->DrawGlyph(x1, y1, x2, y2, 0, 0, faceitem.lWidth, 
            faceitem.lHeight, frame.ImageID);
    }

    for (size_t p = 0; p < faceitem.vPartMotive.size(); ++p)
    {
        PartMotive& partItem = faceitem.vPartMotive[p];

        rect_t rShowPart = {0, 0, partItem.width, partItem.height};

        float ratio = (control.lTimeCount - partItem.lStartTime) 
			/ (float)(partItem.lEndTime - partItem.lStartTime);

        if (ratio > 1.0f)
        {
            ratio = 1.0f;
        }
        else if (ratio < 0)
        {
            ratio = 0;
        }

        switch(partItem.mode)
        {
        case PART_LTTOALL:  // 从左上到右下显现
            rShowPart.right = (long)(rShowPart.right * ratio);
            rShowPart.bottom = (long)(rShowPart.bottom * ratio);
            break;
        case PART_LTOALL:   // 从左到右显现
            rShowPart.right = (long)(rShowPart.right * ratio);
            break;
        case PART_LBTOALL:  // 从左下到右上显现
            rShowPart.right = (long)(rShowPart.right * ratio);
            rShowPart.top = (long)((1.0f - ratio) * rShowPart.bottom);
            break;
        case PART_BTOALL:   // 从下到上显现
            rShowPart.top = (long)((1.0f - ratio) * rShowPart.bottom);
            break;
        case PART_BRTOALL:  // 从右下到左上显现
            rShowPart.left = (long)((1.0f - ratio) * rShowPart.right);
            rShowPart.top = (long)((1.0f - ratio) * rShowPart.bottom);
            break;
        case PART_RTOALL:   // 从右到左显现
            rShowPart.left = (long)((1.0f - ratio) * rShowPart.right);
            break;
        case PART_TRTOALL:  // 从右上到左下显现
            rShowPart.left = (long)((1.0f - ratio) * rShowPart.right);
            rShowPart.bottom = (long)(rShowPart.bottom * ratio);
            break;
        case PART_TTOALL:   // 从上到下显现
            rShowPart.bottom = (long)(rShowPart.bottom * ratio);
            break;
        case PART_MTOLRSIDE:// 从中间到两旁显现
            rShowPart.left = (long)(rShowPart.right - ratio * rShowPart.right) / 2;
            rShowPart.right = (long)(rShowPart.right - rShowPart.left);
            break;
        case PART_MTOTBSIDE:// 从中间到上下显现
            rShowPart.top = (long)(rShowPart.bottom - ratio * rShowPart.bottom) / 2;
            rShowPart.bottom = (long)(rShowPart.bottom - rShowPart.top);
            break;
        case PART_CTOALL:   // 从中间到四周显现
            rShowPart.left = (long)(rShowPart.right - ratio * rShowPart.right) / 2;
            rShowPart.right = (long)(rShowPart.right - rShowPart.left);
            rShowPart.top = (long)(rShowPart.bottom - ratio * rShowPart.bottom) / 2;
            rShowPart.bottom = (long)(rShowPart.bottom - rShowPart.top);
        default:
            break;
        }

        long left = partItem.x + x + rShowPart.left;
        long top = partItem.y + y + rShowPart.top;
        long right = left + rShowPart.right - rShowPart.left;
        long bottom = top + rShowPart.bottom - rShowPart.top;

        if (faceitem.IsPng)
        {
            m_pPainter->DrawAddGlyph(left, top, right, bottom, rShowPart.left, 
				rShowPart.top, rShowPart.right, rShowPart.bottom, 
				partItem.ImageID, 0);
        }
        else
        {
            m_pPainter->DrawGlyph(left, top, right, bottom, rShowPart.left, 
				rShowPart.top, rShowPart.right, rShowPart.bottom, 
				partItem.ImageID);
        }
    }

    if (faceitem.IsDynamicCreate)
    {
        // 动态创建才需要记录
        faceitem.lLastDrawTime = Port_GetTickCount();
    }

    // 结束旋转
    if (faceitem.IsHasRotateMovite || subconfig.bFollowAlpha)
    {
        m_pPainter->SetRotateTransform(NULL);
    }

    // 恢复之前透明度
    if (faceitem.IsHasAlphaMotive || subconfig.bFollowAlpha)
    {
        m_pPainter->SetBlendAlpha(oldalpha);
    }

    // 开始子特效绘制
    for(size_t i = 0; i < faceitem.vSubAnimation.size(); ++i)
    {
        if (faceitem.IsHasAlphaMotive)
        {
            if (faceitem.vSubAnimation[i].bFollowAlpha)
            {
                m_pPainter->SetBlendAlpha(control.nCurAlpha * oldalpha / 255);
            }
            else
            {
                // 恢复之前透明度
                m_pPainter->SetBlendAlpha(oldalpha);
            }
        }

        if (faceitem.IsHasScaleMotive)
        {
            if (faceitem.vSubAnimation[i].bFollowAlpha)
            {
                m_pPainter->SetBlendAlpha(control.nCurAlpha * oldalpha / 255);
            }
            else
            {
                // 恢复之前透明度
                m_pPainter->SetBlendAlpha(oldalpha);
            }
        }

        point_t pt = GetAnimationSize(
            faceitem.vSubAnimation[i].szSubAniName.c_str());

        if (faceitem.vSubAnimation[i].bFollowMove)
        {
            x += control.x;
            y += control.y;
        }

        rect_t rt = {faceitem.vSubAnimation[i].x + x, 
            faceitem.vSubAnimation[i].y + y, 
            faceitem.vSubAnimation[i].x + pt.x + x, 
            faceitem.vSubAnimation[i].y + pt.y + y};

        DrawSubAnimation(faceitem.vSubAnimation[i].szSubAniName.c_str(), 
            faceitem.vSubAnimation[i].x + x, faceitem.vSubAnimation[i].y + y, 
			rt, control, i, faceitem.vSubAnimation[i], mat);
    }

    // 结束旋转
    if (faceitem.IsHasRotateMovite || subconfig.bFollowRotate)
    {
        m_pPainter->SetRotateTransform(NULL);
    }

    return true;
}

point_t ImageAnimationManager::GetAnimationSize(const char* pName)
{
	point_t pt = { 32, 32 };

	if (NULL == pName)
	{
		return pt;
	}

	//long index = m_mNameToIndexMap[pName];

	//if (index <= 0)
	//{
		// 未找到则返回
	//	return pt;
	//}

	//if (index > (long)m_vFaces.size())
	//{
		// 超出范围则返回
	//	return pt;
	//}

	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return pt;
	}

	if (index >= m_vFaces.size())
	{
		// 超出范围则返回
		return pt;
	}
	
	//FaceItem& faceitem = m_vFaces[index - 1];
	FaceItem& faceitem = m_vFaces[index];

	if (faceitem.IsDynamicCreate && !faceitem.IsEverCreate)
	{
		// 如果是动态创建，且从未创建过，则创建一下
		DynamicCreateAnmation(faceitem);
	}

	pt.x = faceitem.lWidth;
	pt.y = faceitem.lHeight;

	return pt;
}

bool ImageAnimationManager::SetAnimationLoop(const char* pName, bool value)
{
    //long index = m_mNameToIndexMap[pName];

    //if (index <= 0)
    //{
        // 未找到则返回
    //    return false;
    //}

    //if (index > (long)m_vFaces.size())
    //{
        // 超出范围则返回
    //    return false;
    //}

	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return false;
	}

	if (index >= m_vFaces.size())
	{
		// 超出范围则返回
		return false;
	}
    
	FaceItem& faceitem = m_vFaces[index];

    faceitem.control.loop = value;

    return true;
}

bool ImageAnimationManager::StartAnimation(const char* pName, 
	ControlParam& control)
{
    //long index = m_mNameToIndexMap[pName];

    //if (index <= 0)
    //{
        // 未找到则返回
    //    return false;
    //}

    //if (index > (long)m_vFaces.size())
    //{
        // 超出范围则返回
    //    return false;
    //}

	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return false;
	}

	if (index >= m_vFaces.size())
	{
		// 超出范围则返回
		return false;
	}
    
	FaceItem& faceitem = m_vFaces[index];

    if (faceitem.IsDynamicCreate && !faceitem.IsEverCreate)
    {
        // 如果是动态创建，且从未创建过，则创建一下
        DynamicCreateAnmation(faceitem);
    }

    if (faceitem.vFrames.size() > 0)
    {
        if (control.athwart)
        {
            // 逆序
            SetToEnd(faceitem, control);
        }
        else
        {
            // 顺序
            SetToBegin(faceitem, control);
        }

        Execute(0);
    }

    return true;
}

// 启动动画,默认创建好就启动
bool ImageAnimationManager::StartAnimation(const char* pName)
{
    //long index = m_mNameToIndexMap[pName];

    //if (index <= 0)
    //{
        // 未找到则返回
    //    return false;
    //}

    //if (index > (long)m_vFaces.size())
    //{
        // 超出范围则返回
    //    return false;
    //}

	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return false;
	}

	if (index >= m_vFaces.size())
	{
		// 超出范围则返回
		return false;
	}
    
	FaceItem& faceitem = m_vFaces[index];

    return StartAnimation(pName, faceitem.control);
}

float ImageAnimationManager::GetAnimationCurTime(const char* pName)
{
    //long index = m_mNameToIndexMap[pName];

    //if (index <= 0)
    //{
        // 未找到则返回
    //    return -1.0f;
    //}

    //if (index > (long)m_vFaces.size())
    //{
        // 超出范围则返回
    //    return -1.0f;
    //}

	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return -1.0f;
	}

	if (index >= m_vFaces.size())
	{
		// 超出范围则返回
		return -1.0f;
	}
    
	FaceItem& faceitem = m_vFaces[index];

    if (faceitem.IsCreated)
    {
        return faceitem.control.lTimeCount / 1000.0f;
    }

    return -1.0f;
}

bool ImageAnimationManager::SetAnimationCurTime(const char* pName, int time)
{
	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return false;
	}

	if (index >= m_vFaces.size())
	{
		// 超出范围则返回
		return false;
	}
    
	FaceItem& faceitem = m_vFaces[index];

	if (time > faceitem.control.lTime)
	{
		time = faceitem.control.lTime;
	}

	faceitem.control.lTimeCount = time;

	UpdateAnimation(faceitem, faceitem.control, time);
    return true;
}

float ImageAnimationManager::GetAnimationTotalTime(const char* pName)
{
    //long index = m_mNameToIndexMap[pName];

    //if (index <= 0)
    //{
        // 未找到则返回
    //    return -1.0f;
    //}

    //if (index > (long)m_vFaces.size())
    //{
        // 超出范围则返回
    //    return -1.0f;
    //}

	size_t index;

	if (!m_mNameToIndexMap.GetData(pName, index))
	{
		// 未找到则返回
		return -1.0f;
	}

	if (index >= m_vFaces.size())
	{
		// 超出范围则返回
		return -1.0f;
	}

    FaceItem& faceitem = m_vFaces[index];

    return faceitem.control.lTime / 1000.0f;
}

ImageAnimationManager::AniCallBacks& ImageAnimationManager::GetAniCallBacks(
	const char* name)
{
	Assert(name != NULL);

	size_t index;

	if (!m_CallBackIndex.GetData(name, index))
	{
		index = m_CallBacks.size();
		m_CallBackIndex.Add(name, index);
		m_CallBacks.push_back(CORE_NEW(AniCallBacks));
	}

	return *m_CallBacks[index];
}

bool ImageAnimationManager::Register(const char* pName, const char* pLuaFile, 
	const char* pLuaFunc, PERSISTID id)
{
    AniCallBacks& ani_data = GetAniCallBacks(pName);

	for (size_t i = 0; i < ani_data.size(); ++i)
	{
		if (ani_data[i].id == id && 
			ani_data[i].szLuaFile == pLuaFile && 
			ani_data[i].szLuaFunc == pLuaFunc)
		{
			return true;
		}
	}

	AniCallBackItem item;

	item.id = id;
	item.szLuaFile = pLuaFile;
	item.szLuaFunc = pLuaFunc;

	ani_data.push_back(item);

	/*
	if (m_mapCallBack.find(pName) != m_mapCallBack.end()) 
    {
        std::vector<AniCallBackItem>& ani_data = m_mapCallBack[pName];
        for (size_t i=0; i<ani_data.size(); ++i)
        {
            if (ani_data[i].id == id && 
                ani_data[i].szLuaFile == pLuaFile && 
                ani_data[i].szLuaFunc == pLuaFunc)
            {
                return true;
            }
        }

        AniCallBackItem item;
        item.id = id;
        item.szLuaFile = pLuaFile;
        item.szLuaFunc = pLuaFunc;
        ani_data.push_back(item);
    }
    else
    {
        std::vector<AniCallBackItem> ani_data;
        
        AniCallBackItem item;
        item.id = id;
        item.szLuaFile = pLuaFile;
        item.szLuaFunc = pLuaFunc;
        ani_data.push_back(item);

        m_mapCallBack[pName] = ani_data;
    }
	*/

    return true;
}

bool ImageAnimationManager::UnRegister(const char* pName, 
	const char* pLuaFile, const char* pLuaFunc, PERSISTID id)
{
	if (m_CallBackIndex.Exists(pName))
	{
		return false;
	}
	
	AniCallBacks& ani_data = GetAniCallBacks(pName);

	for (size_t i = 0; i < ani_data.size(); ++i)
	{
		if (ani_data[i].id == id && 
			ani_data[i].szLuaFile == pLuaFile && 
			ani_data[i].szLuaFunc == pLuaFunc)
		{
			if (m_bDeleteSafe)
			{
				ani_data.erase(ani_data.begin() + i);
			}
			else
			{
				ani_data[i].id = PERSISTID();

				//m_bNeedClear = true;
				m_vNeedClear.push_back(pName);
			}

			return true;
		}
	}

	return false;
    
	/*
	if (m_mapCallBack.find(pName) != m_mapCallBack.end())
    {
        std::vector<AniCallBackItem>& ani_data = m_mapCallBack[pName];

        for (size_t i = 0; i < ani_data.size(); ++i)
        {
            if (ani_data[i].id == id && 
                ani_data[i].szLuaFile == pLuaFile && 
                ani_data[i].szLuaFunc == pLuaFunc)
            {
                if (m_bDeleteSafe)
                {
                    ani_data.erase(ani_data.begin() + i);
                }
                else
                {
                    ani_data[i].id = PERSISTID();
                    m_bNeedClear = true;
                    m_vNeedClear.push_back(pName);
                }

                break;
            }
        }
    }

    return true;
	*/
}

bool ImageAnimationManager::Clear()
{
    m_vFaces.clear();
    m_mNameToIndexMap.Clear();
    return true;
}

// 重载所有动画
bool ImageAnimationManager::Load()
{
    //读取配置信息
    std::string fname = std::string(m_pPainter->GetDefaultPath()) 
        + m_szFilePath;

    //TiXmlDocument doc(fname.c_str());
    xml_parser doc;

    //if (doc.LoadFile())
    if (doc.Parse(fname.c_str()))
    {
		xml_parser_node* Root = doc.first_node("Root");
        xml_parser_node* pElement = Root->FirstChildElement(
            "Animation");

        while (pElement)
        {
            FaceItem faceItem;

            // 动画名称(这个必配)
            faceItem.szFaceName = pElement->Attribute("Name");

            const char *pValue = NULL;

            // 是否动态创建
            faceItem.IsDynamicCreate = false;

            pValue = pElement->Attribute("IsDynamicCreate");

            if (pValue != NULL && stricmp(pValue, "true") == 0)
            {
                faceItem.IsDynamicCreate = true;
            }

            faceItem.IsPng = false;
            pValue = pElement->Attribute("IsPng");

            if (pValue != NULL && stricmp(pValue, "true") == 0)
            {
                faceItem.IsPng = true;
            }

            pValue = pElement->Attribute("Loop");

            if (pValue != NULL && stricmp(pValue, "false") == 0)
            {
                faceItem.control.loop = false;
            }
            else
            {
                // 默认loop为true
                faceItem.control.loop = true;
            }

            faceItem.nBlendMode = 0;
            pValue = pElement->Attribute("BlendMode");

            if (pValue != NULL)
            {
                faceItem.nBlendMode = StringUtil::StringAsInt(pValue);
            }

            // 是否有旋转动画(旋转是以中心点旋转)
            faceItem.IsHasRotateMovite = false;

            pValue = pElement->Attribute("RotateMotive");

            if (pValue != 0)
            {
				CVarList vData;

				if (StringUtil::ParseToVector(pValue, ';', vData))
				{
					if (vData.GetCount() > 1)
					{
						faceItem.IsHasRotateMovite = true;

						for (size_t i = 0; i < vData.GetCount(); ++i)
						{
							CVarList vSubData;

							if (StringUtil::ParseToVector( 
								vData.StringVal(i), ',', vSubData))
							{
								if (vSubData.GetCount() > 1)
								{
									RotateItem rotateitem;

									rotateitem.starttime = 
										StringUtil::StringAsInt(vSubData.StringVal(0));
									rotateitem.angle = 
										StringUtil::StringAsInt(vSubData.StringVal(1));
									faceItem.vRotateMotive.push_back(
										rotateitem);
								}
							}
						}
					}
				}
                /*
				std::vector<std::string> vData;

                if (StringUtil::ParseToVector(vData, pValue, ";"))
                {
                    if (vData.size() > 1)
                    {
                        faceItem.IsHasRotateMovite = true;

                        for (size_t i = 0; i < vData.size(); ++i)
                        {
                            std::vector < std::string > vSubData;

                            if (StringUtil::ParseToVector(vSubData, 
								vData[i].c_str(), ","))
                            {
                                if (vSubData.size() > 1)
                                {
                                    RotateItem rotateitem;

                                    rotateitem.starttime = 
                                        StringUtil::StringAsInt(vSubData[0]);
                                    rotateitem.angle = 
                                        StringUtil::StringAsInt(vSubData[1]);
                                    faceItem.vRotateMotive.push_back(
                                        rotateitem);
                                }
                            }
                        }
                    }
                }
				*/
            }

            // 是否有放缩动画(放缩是以中心点放缩)
            faceItem.IsHasScaleMotive = false;

            pValue = pElement->Attribute("ScaleMotive");

            if (pValue != 0)
            {
				CVarList vData;

				if (StringUtil::ParseToVector(pValue, ';', vData))
				{
					if (vData.GetCount() > 1)
					{
						faceItem.IsHasScaleMotive = true;

						for (size_t i = 0; i < vData.GetCount(); ++i)
						{
							CVarList vSubData;

							if (StringUtil::ParseToVector( 
								vData.StringVal(i), ',', vSubData))
							{
								if (vSubData.GetCount() > 2)
								{
									// 填两个参数时可以各自控制宽高的放缩比
									ScaleItem scaleitem;

									scaleitem.starttime = 
										StringUtil::StringAsInt(vSubData.StringVal(0));
									scaleitem.xvalue = 
										StringUtil::StringAsFloat(vSubData.StringVal(1));
									scaleitem.yvalue = 
										StringUtil::StringAsFloat(vSubData.StringVal(2));

									faceItem.vScaleMotive.push_back(scaleitem);
								}
								else if (vSubData.GetCount() > 1)
								{
									// 填一个参时作为整体的放缩比
									ScaleItem scaleitem;

									scaleitem.starttime = 
										StringUtil::StringAsInt(vSubData.StringVal(0));
									scaleitem.xvalue = 
										StringUtil::StringAsFloat(vSubData.StringVal(1));
									scaleitem.yvalue = scaleitem.xvalue;

									faceItem.vScaleMotive.push_back(scaleitem);
								}
							}
						}
					}
				}
                /*
				std::vector<std::string> vData;

                if (StringUtil::ParseToVector(vData, pValue, ";"))
                {
                    if (vData.size() > 1)
                    {
                        faceItem.IsHasScaleMotive = true;

                        for (size_t i=0; i<vData.size(); ++i)
                        {
                            std::vector < std::string > vSubData;

                            if (StringUtil::ParseToVector(vSubData, 
								vData[i].c_str(), ","))
                            {
                                if (vSubData.size() > 2)
                                {
                                    // 填两个参数时可以各自控制宽高的放缩比
                                    ScaleItem scaleitem;

                                    scaleitem.starttime = 
                                        StringUtil::StringAsInt(vSubData[0]);
                                    scaleitem.xvalue = 
                                        StringUtil::StringAsFloat(vSubData[1]);
                                    scaleitem.yvalue = 
                                        StringUtil::StringAsFloat(vSubData[2]);

                                    faceItem.vScaleMotive.push_back(scaleitem);
                                }
                                else if (vSubData.size() > 1)
                                {
                                    // 填一个参时作为整体的放缩比
                                    ScaleItem scaleitem;

                                    scaleitem.starttime = 
                                        StringUtil::StringAsInt(vSubData[0]);
                                    scaleitem.xvalue = 
                                        StringUtil::StringAsFloat(vSubData[1]);
                                    scaleitem.yvalue = scaleitem.xvalue;

                                    faceItem.vScaleMotive.push_back(scaleitem);
                                }
                            }
                        }
                    }
                }
				*/
            }

            // 是否有ALHPA透明动画
            faceItem.IsHasAlphaMotive = false;

            pValue = pElement->Attribute("AlphaMotive");

            if (pValue != 0)
            {
				CVarList vData;

				if (StringUtil::ParseToVector(pValue, ';', vData))
				{
					if (vData.GetCount() > 1)
					{
						faceItem.IsHasAlphaMotive = true;

						for (size_t i = 0; i < vData.GetCount(); ++i)
						{
							CVarList vSubData;

							if (StringUtil::ParseToVector( 
								vData.StringVal(i), ',', vSubData))
							{
								if (vSubData.GetCount() > 1)
								{
									AlphaItem alphaitem;

									alphaitem.starttime = 
										StringUtil::StringAsInt(vSubData.StringVal(0));
									alphaitem.alpha = 
										StringUtil::StringAsInt(vSubData.StringVal(1));
									faceItem.vAlphaMotive.push_back(alphaitem);
								}
							}
						}
					}
				}
                /*
				std::vector<std::string> vData;

                if (StringUtil::ParseToVector(vData, pValue, ";"))
                {
                    if (vData.size() > 1)
                    {
                        faceItem.IsHasAlphaMotive = true;

                        for (size_t i = 0; i < vData.size(); ++i)
                        {
                            std::vector<std::string > vSubData;

                            if (StringUtil::ParseToVector(vSubData, 
								vData[i].c_str(), ","))
                            {
                                if (vSubData.size() > 1)
                                {
                                    AlphaItem alphaitem;

                                    alphaitem.starttime = 
                                        StringUtil::StringAsInt(vSubData[0]);
                                    alphaitem.alpha = 
                                        StringUtil::StringAsInt(vSubData[1]);
                                    faceItem.vAlphaMotive.push_back(alphaitem);
                                }
                            }
                        }
                    }
                }
				*/
            }

            long Width = 0, Height = 0;

            xml_parser_node* pSubElement = pElement->FirstChildElement(
                "ImageFrame");

            int frame_time_count = 0;
            while (pSubElement)
            {
                FaceFrame frameItem;

                frameItem.szFileName = pSubElement->Attribute("FileName");

                const char* pValue = pSubElement->Attribute("KeepTime");

                if (pValue)
                {
                    frame_time_count += StringUtil::StringAsInt(pValue);
                    frameItem.lEndTime = frame_time_count;
                }

                // 这边读进来后就预创建好贴图,以后如果觉得浪费本存的话可以去掉
                if (!faceItem.IsDynamicCreate)
                {
                    long CurWidth = 0;
                    long CurHeight = 0;

                    frameItem.ImageID = CreateGlyph(m_pPainter, 
                        faceItem.IsPng, frameItem.szFileName.c_str(), 
                        CurWidth, Height);

                    if (Width < CurWidth)
                    {
                        Width = CurWidth;
                    }

                    if (Height < CurHeight)
                    {
                        Height = CurHeight;
                    }
                }

                faceItem.vFrames.push_back(frameItem);

                pSubElement = pSubElement->NextSiblingElement("ImageFrame");
            }

            faceItem.control.lTime = frame_time_count;

            pSubElement = pElement->FirstChildElement(
                "MoveStep");

            while (pSubElement)
            {
                MoveItem moveItem;

                const char* pValue = pSubElement->Attribute("x");

                if (pValue)
                {
                    moveItem.x = StringUtil::StringAsInt(pValue);
                }

                pValue = pSubElement->Attribute("y");

                if (pValue)
                {
                    moveItem.y = StringUtil::StringAsInt(pValue);
                }

                pValue = pSubElement->Attribute("TimeIndex");

                if (pValue)
                {
                    moveItem.lTimeIndex = StringUtil::StringAsInt(pValue);
                }

                faceItem.vMoveMotive.push_back(moveItem);

                pSubElement = pSubElement->NextSiblingElement("MoveStep");
            }

            pSubElement = pElement->FirstChildElement(
                "PartMotive");

            while (pSubElement)
            {
                PartMotive partItem;

                const char* pValue = pSubElement->Attribute("Mode");

                if (pValue)
                {
                    partItem.mode = (ePartMotiveMode)StringUtil::StringAsInt(pValue);
                }

                pValue = pSubElement->Attribute("x");

                if (pValue)
                {
                    partItem.x = StringUtil::StringAsInt(pValue);
                }

                pValue = pSubElement->Attribute("y");

                if (pValue)
                {
                    partItem.y = StringUtil::StringAsInt(pValue);
                }

                pValue = pSubElement->Attribute("StartTime");

                if (pValue)
                {
                    partItem.lStartTime = StringUtil::StringAsInt(pValue);
                }

                pValue = pSubElement->Attribute("EndTime");

                if (pValue)
                {
                    partItem.lEndTime = StringUtil::StringAsInt(pValue);
                }

                pValue = pSubElement->Attribute("FileName");
                if (pValue)
                {
                    partItem.szFileName = pValue;
                }

                // 这边读进来后就预创建好贴图,以后如果觉得浪费本存的话可以去掉
                if (!faceItem.IsDynamicCreate)
                {
                    long CurWidth = 0;
                    long CurHeight = 0;

                    partItem.ImageID = CreateGlyph(m_pPainter, 
                        faceItem.IsPng, partItem.szFileName.c_str(), 
                        CurWidth, Height);

                    partItem.width = CurWidth;
                    partItem.height = Height;
                }

                faceItem.vPartMotive.push_back(partItem);

                pSubElement = pSubElement->NextSiblingElement("PartMotive");
            }

            

            pSubElement = pElement->FirstChildElement("SubAnimation");

            while (pSubElement)
            {
                SubAnimation animation;

                const char* pValue = pSubElement->Attribute("Name");

                if (pValue)
                {
                    if (strcmp(pValue, faceItem.szFaceName.c_str()) == 0)
                    {
                        // 子特效不能是自已
                        pSubElement = pSubElement->NextSiblingElement("SubAnimation");
                        continue;
                    }
                    animation.szSubAniName = pValue;
                }

                pValue = pSubElement->Attribute("x");

                if (pValue)
                {
                    animation.x = StringUtil::StringAsInt(pValue);
                }

                pValue = pSubElement->Attribute("y");

                if (pValue)
                {
                    animation.y = StringUtil::StringAsInt(pValue);
                }

                pValue = pSubElement->Attribute("StartTime");

                if (pValue)
                {
                    animation.start_time = StringUtil::StringAsInt(pValue);
                }
                else
                {
                    animation.start_time = 0;
                }
            

                pValue = pSubElement->Attribute("EndTime");

                if (pValue)
                {
                    animation.end_time = StringUtil::StringAsInt(pValue);
                }
                else
                {
                    animation.end_time = 9999999;
                }

                pValue = pSubElement->Attribute("FollowMove");

                if (pValue && stricmp(pValue , "true") == 0)
                {
                    animation.bFollowMove = true;
                }
                else
                {
                    animation.bFollowMove = false;
                }

                pValue = pSubElement->Attribute("FollowAlpha");

                if (pValue && stricmp(pValue , "true") == 0)
                {
                    animation.bFollowAlpha = true;
                }
                else
                {
                    animation.bFollowAlpha = false;
                }

                pValue = pSubElement->Attribute("FollowScale");

                if (pValue && stricmp(pValue , "true") == 0)
                {
                    animation.bFollowScale = true;
                }
                else
                {
                    animation.bFollowScale = false;
                }

                pValue = pSubElement->Attribute("FollowRotate");

                if (pValue && stricmp(pValue , "true") == 0)
                {
                    animation.bFollowRotate = true;
                }
                else
                {
                    animation.bFollowRotate = false;
                }

                pValue = pSubElement->Attribute("FollowPlay");

                if (pValue && stricmp(pValue , "true") == 0)
                {
                    animation.bFollowPlay = true;
                }
                else
                {
                    animation.bFollowPlay = false;
                }

                faceItem.vSubAnimation.push_back(animation);
                faceItem.control.vSubControls.push_back(ControlParam());
                pSubElement = pSubElement->NextSiblingElement("SubAnimation");
            }

            faceItem.lWidth = Width;
            faceItem.lHeight = Height;

            if (faceItem.IsDynamicCreate)
            {
                faceItem.UseCount = 0;
            }
            else
            {
                faceItem.IsEverCreate = true;
                faceItem.IsCreated = true;
            }

            m_mNameToIndexMap.Add(faceItem.szFaceName.c_str(), 
				m_vFaces.size());

			m_vFaces.push_back(faceItem);

            //m_mNameToIndexMap[faceItem.szFaceName] = m_vFaces.size();

            pElement = pElement->NextSiblingElement("Animation");
        }
    }

    return true;
}

// 获取动画列表
void ImageAnimationManager::GetAnimationList(const IVarList& param, IVarList& result)
{
    //result.Clear();
    std::string szBuffer = "";

    for(size_t i = 0; i < m_vFaces.size(); ++i)
    {
        szBuffer += m_vFaces[i].szFaceName + ",";
    }
    
    result.AddString(szBuffer.c_str());
}

bool ImageAnimationManager::DoCallBack(const char* pName, const char* pMode)
{
    m_bDeleteSafe = false;

    size_t index;

	if (m_CallBackIndex.GetData(pName, index))
	{
		AniCallBacks& ani_data = *m_CallBacks[index];
		
		for (size_t i = 0; i < ani_data.size(); ++i)
		{
			if (GetCore()->GetEntity(ani_data[i].id) != NULL)
			{
#if 0	//"on_animation_event"
				GuiUtil_RunCallback(this, "on_animation_event", 
					CVarList() << pName << pMode << ani_data[i].szLuaFile.c_str()
					<< ani_data[i].szLuaFunc.c_str() << ani_data[i].id);
#endif
			}
			else
			{
				ani_data[i].id = PERSISTID();

				//m_bNeedClear = true;
				m_vNeedClear.push_back(pName);
			}
		}
	}

	/*
	if (m_mapCallBack.find(pName) != m_mapCallBack.end())
    {
        std::vector<AniCallBackItem>& ani_data = m_mapCallBack[pName];

        for (size_t i = 0; i < ani_data.size(); ++i)
        {
            if (GetCore()->GetEntity(ani_data[i].id) != NULL)
            {
                GuiUtil_RunCallback(this, "on_animation_event", 
                    CVarList() << pName << pMode << ani_data[i].szLuaFile
					<< ani_data[i].szLuaFunc << ani_data[i].id);
            }
            else
            {
                ani_data[i].id = PERSISTID();
                m_bNeedClear = true;
                m_vNeedClear.push_back(pName);
            }
        }
    }
	*/

    m_bDeleteSafe = true;

    //if (m_bNeedClear)
	if (!m_vNeedClear.empty())
    {
        //m_bNeedClear = false;

        // 清理删除
        for (size_t v = 0; v < m_vNeedClear.size(); ++v)
        {
            AniCallBacks& ani_data = GetAniCallBacks(m_vNeedClear[v].c_str());
			
			AniCallBacks::iterator it = ani_data.begin();

			while (it != ani_data.end())
			{
				if (GetCore()->GetEntity(it->id) == NULL)
				{
					it = ani_data.erase(it);
				}
				else
				{
					++it;
				}
			}
			/*
			if (m_mapCallBack.find(m_vNeedClear[v]) != m_mapCallBack.end())
            {
                std::vector<AniCallBackItem>& ani_data = 
					m_mapCallBack[m_vNeedClear[v]];
                std::vector<AniCallBackItem>::iterator it = ani_data.begin();

                for (; it != ani_data.end();)
                {
                    if (GetCore()->GetEntity(it->id) == NULL)
                    {
                        it = ani_data.erase(it);
                    }
                    else
                    {
                        it++;
                    }
                }
            }
			*/
        }
 
        m_vNeedClear.clear();
    }

    return true;
}

bool ImageAnimationManager::DynamicCreateAnmation(FaceItem &faceitem)
{
    if (!m_pPainter)
    {
        return false;
    }

    long Width = 0;
    long Height = 0;

    for (size_t i = 0; i < faceitem.vFrames.size(); ++i)
    {
        FaceFrame& frameItem = faceitem.vFrames[i];

        long CurWidth = 0;
        long CurHeight = 0;

        frameItem.ImageID = CreateGlyph(m_pPainter, faceitem.IsPng, 
			frameItem.szFileName.c_str(), CurWidth, CurHeight);

        if (Width < CurWidth)
        {
            Width = CurWidth;
        }

        if (Height < CurHeight)
        {
            Height = CurHeight;
        }
    }

    for (size_t i = 0; i < faceitem.vPartMotive.size(); ++i)
    {
        PartMotive& partItem = faceitem.vPartMotive[i];

        long CurWidth = 0;
        long CurHeight = 0;

        partItem.ImageID = CreateGlyph(m_pPainter, faceitem.IsPng, 
            partItem.szFileName.c_str(), CurWidth, CurHeight);

        partItem.width = CurWidth;
        partItem.height = CurHeight;
    }

    faceitem.lWidth = Width;
    faceitem.lHeight = Height;

    faceitem.UseCount = 1;

    faceitem.IsEverCreate = true;
    faceitem.IsCreated = true;


    if (faceitem.vFrames.size() > 0)
    {
        faceitem.control.lCurIndex = 0;
        faceitem.control.lTimeCount = 0;

        // 创建成功，则立即开始
        if (faceitem.IsDynamicCreate)
        {
            Execute(0.0f);
            DoCallBack(faceitem.szFaceName.c_str(), "start");
        }
    }

    return true;
}

PERSISTID ImageAnimationManager::CreateGlyph(void* Pointer, bool IsPng, 
	const char* pFileName, long& Width, long& Height)
{
    if (pFileName == NULL || pFileName[0] == 0)
    {
        return PERSISTID();
    }

    IPainter* pPainter = (IPainter*)Pointer;

    //if (IsPng)
    //{
        PERSISTID image = pPainter->CreateGlyph(pFileName, false);

        if (image.IsNull())
        {
            return image;
        }

        Width = pPainter->GetGlyphWidth(image);
        Height = pPainter->GetGlyphHeight(image);

        return image;
    //}
    //else
    //{
    //    PERSISTID image = pPainter->CreateTexture(pFileName);

    //    if (image.IsNull())
    //    {
    //        return image;
    //   }

    //    Width = pPainter->GetTextureWidth(image);
    //    Height = pPainter->GetTextureHeight(image);

    //    return image;
    //}
}
