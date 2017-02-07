//--------------------------------------------------------------------
// 文件名:		design_loader.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "design_loader.h"
#include "design_res.h"
#include "designer.h"
#include "i_control.h"
#include "form.h"
#include "font.h"
#include "cursor.h"
#include "../extern/rapidxml/rapidxml.hpp"
#include "../public/var_table.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../visual/i_painter.h"
#include "../visual/i_font.h"
using namespace rapidxml;

// Designer

CDesignLoader::CDesignLoader()
{
	m_pDesigner = NULL;
	//
	m_mapIniFile.Clear();
	m_vecFileInf.clear();
}

CDesignLoader::~CDesignLoader()
{
	m_mapIniFile.Clear();

	//
	for(size_t i = 0;i < m_vecFileInf.size();i++ )
	{
		if (m_vecFileInf[i])
		{
			delete [] m_vecFileInf[i];
		}
	}
}

void CDesignLoader::SetDesigner(Designer* value)
{
	Assert(value != NULL);
	
	m_pDesigner = value;
}

// 属性值不存在或为空
static inline bool string_null_empty(xml_attribute<>* s)
{
	return (NULL == s) || StringEmpty(s->value());
}

/*
// 加载颜色数据
static bool load_xml_color(Designer* pDesigner, const char* file, 
	xml_node<>* root, bool is_utf8)
{
	Assert(file != NULL);
	Assert(root != NULL);

	xml_attribute<>* name = root->first_attribute("name");
	xml_attribute<>* value = root->first_attribute("value");

	if ((NULL == name) || (NULL == value))
	{
		CORE_TRACE("(CDesignLoader::LoadXmlColor)parameter error");
		CORE_TRACE(file);
		return false;
	}

	if (!pDesigner->GetGui()->AddColor(name->value(), value->value()))
	{
		CORE_TRACE("(CDesignLoader::LoadXmlColor)add failed");
		CORE_TRACE(name->value());
		CORE_TRACE(file);
		return false;
	}

	return true;
}

// 加载图元数据
static bool load_xml_image(Designer* pDesigner, const char* file, 
	xml_node<>* root, bool is_utf8)
{
	Assert(file != NULL);
	Assert(root != NULL);

	xml_attribute<>* name = root->first_attribute("name");
	xml_attribute<>* texture = root->first_attribute("texture");
	xml_attribute<>* left = root->first_attribute("left");
	xml_attribute<>* top = root->first_attribute("top");
	xml_attribute<>* right = root->first_attribute("right");
	xml_attribute<>* bottom = root->first_attribute("bottom");
	xml_attribute<>* width = root->first_attribute("width");
	xml_attribute<>* height = root->first_attribute("height");

	if ((NULL == name) || (NULL == texture) || (NULL == left) || (NULL == top))
	{
		CORE_TRACE("(CDesignLoader::LoadXmlImage)parameter error");
		CORE_TRACE(file);
		return false;
	}

	int l = atoi(left->value());
	int t = atoi(top->value());
	int r = l + 16;
	int b = t + 16;

	if ((width != NULL) && (height != NULL))
	{
		r = l + atoi(width->value());
		b = t + atoi(height->value());
	}
	else if ((right != NULL) && (bottom != NULL))
	{
		r = atoi(right->value());
		b = atoi(bottom->value());
	}

	CImageData data;

	data.m_strTex = texture->value();
	data.m_nLeft = l;
	data.m_nTop = t;
	data.m_nRight = r;
	data.m_nBottom = b;

	// 读取图元动画信息
	xml_node<>* child = root->first_node("flip");

	while (child)
	{
		xml_attribute<>* flip_time = child->first_attribute("time");
		xml_attribute<>* flip_left = child->first_attribute("left");
		xml_attribute<>* flip_top = child->first_attribute("top");
		xml_attribute<>* flip_right = child->first_attribute("right");
		xml_attribute<>* flip_bottom = child->first_attribute("bottom");
		xml_attribute<>* flip_width = child->first_attribute("width");
		xml_attribute<>* flip_height = child->first_attribute("height");

		if ((NULL == flip_time) 
			|| (NULL == flip_left) 
			|| (NULL == flip_top))
		{
			CORE_TRACE("(CDesignLoader::LoadXmlFont)flip info error");
			CORE_TRACE(file);
			return false;
		}

		CImageFlip::sub_image_t sub_image;

		sub_image.fSeconds = (float)atof(flip_time->value());
		sub_image.nLeft = atoi(flip_left->value());
		sub_image.nTop = atoi(flip_top->value());
		sub_image.nRight = sub_image.nLeft + 16;
		sub_image.nBottom = sub_image.nTop + 16;

		if ((flip_width != NULL) && (flip_height != NULL))
		{
			sub_image.nRight = sub_image.nLeft + atoi(flip_width->value());
			sub_image.nBottom = sub_image.nTop + atoi(flip_height->value());
		}
		else if ((flip_right != NULL) && (flip_bottom != NULL))
		{
			sub_image.nRight = atoi(flip_right->value());
			sub_image.nBottom = atoi(flip_bottom->value());
		}

		if (sub_image.fSeconds > 0.0F)
		{
			data.CreateFlip();

			data.m_pFlip->m_SubImages.push_back(sub_image);
		}

		child = child->next_sibling("flip");
	}

	if (!pDesigner->GetGui()->AddImageData(name->value(), data))
	{
		CORE_TRACE("(CDesignLoader::LoadXmlImage)add failed");
		CORE_TRACE(name->value());
		CORE_TRACE(file);
		return false;
	}

	return true;
}
*/

// 十六进制转成字符
static unsigned int hex_to_char(const char* s)
{
	size_t count = strlen(s);

	unsigned int v = 0;

	for (size_t i = 0; i < count; ++i)
	{
		v <<= 4;

		char c = s[i];

		if ((c >= '0') && (c <= '9'))
		{
			v += (unsigned int)(c - '0');
		}
		else if ((c >= 'A') && (c <= 'F'))
		{
			v += (unsigned int)(c - 'A' + 10);
		}
		else if ((c >= 'a') && (c <= 'f'))
		{
			v += (unsigned int)(c - 'a' + 10);
		}
		else
		{
			return L'\0';
		}
	}

	return v;
}

// 加载字体数据
static bool load_xml_font(Designer* pDesigner, const char* file, 
	xml_node<>* root, bool is_utf8)
{
	Assert(file != NULL);
	Assert(root != NULL);

	xml_attribute<>* name = root->first_attribute("name");
	
	if (NULL == name)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlFont)no name");
		CORE_TRACE(file);
		return false;
	}
	
	xml_attribute<>* refer_font_attr = root->first_attribute("refer_font");

	// 引用已有字体
	if (refer_font_attr != NULL)
	{
		const char* refer_font = refer_font_attr->value();
		// 获取引用字体ID
		PERSISTID refer_font_id = pDesigner->GetGui()->GetFont(refer_font);
		Font* font = (Font*)pDesigner->GetCore()->GetEntity(refer_font_id); 

		// 原字体存在，则添加引用
		if (font != NULL)
		{
			pDesigner->GetGui()->AddReferFont(name->value(), refer_font);
			return true;
		}

		return false;
	}

	//xml_attribute<>* sys_font = root->first_attribute("sys_font");
	xml_attribute<>* font_file = root->first_attribute("font_file");
	xml_attribute<>* font_tex = root->first_attribute("font_tex");
	xml_attribute<>* tex_width = root->first_attribute("tex_width");
	xml_attribute<>* tex_height = root->first_attribute("tex_height");
	xml_attribute<>* height = root->first_attribute("height");
	xml_attribute<>* interval = root->first_attribute("interval");
	xml_attribute<>* bold = root->first_attribute("bold");
	xml_attribute<>* smooth = root->first_attribute("smooth");
	xml_attribute<>* italic = root->first_attribute("italic");
	xml_attribute<>* underline = root->first_attribute("underline");
	xml_attribute<>* strikeout = root->first_attribute("strikeout");
	xml_attribute<>* var_extent = root->first_attribute("var_extent");
    xml_attribute<>* outline = root->first_attribute("outline");
	xml_attribute<>* outline_color = root->first_attribute("outline_color");
	xml_attribute<>* outline_num = root->first_attribute("outline_num");
	xml_attribute<>* fade_type = root->first_attribute("fade_type");
	xml_attribute<>* shadow_color = root->first_attribute("shadow_color");
	xml_attribute<>* shadow_offset_x = root->first_attribute("shadow_offset_x");
	xml_attribute<>* shadow_offset_y = root->first_attribute("shadow_offset_y");


	if (NULL == height)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlFont)no height");
		CORE_TRACE(file);
		return false;
	}

	//if ((NULL == sys_font) && (NULL == font_tex))
	if ((NULL == font_file) && (NULL == font_tex))
	{
		CORE_TRACE("(CDesignLoader::LoadXmlFont)no font resource name");
		CORE_TRACE(file);
		return false;
	}

	Font* font = (Font*)pDesigner->GetCore()->GetEntity(
		pDesigner->GetGui()->NewFont());

	if (NULL == font)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlFont)create failed");
		CORE_TRACE(file);
		return false;
	}

	font->SetName(name->value());
	font->SetHeight(atoi(height->value()));

//	if (sys_font != NULL)
//	{
//		if (is_utf8 && GuiUtil_HasUtf8Char(sys_font->value()))
//		{
//			// 从UTF8转换到本地编码
//			result_wstring ws = GuiUtil_Utf8ToUtf16(sys_font->value()).c_str();
//			result_string v = GuiUtil_Utf16ToAnsi(ws.c_str()).c_str();
//
//			font->SetSysFont(v.c_str());
//		}
//		else
//		{
//			font->SetSysFont(sys_font->value());
//		}
//	}

	if (font_file != NULL)
	{
		font->SetFontFile(font_file->value());
	}

	if (font_tex != NULL)
	{
		font->SetFontTex(font_tex->value());
	}

	if (tex_width != NULL)
	{
		font->SetTexWidth(atoi(tex_width->value()));
	}

	if (tex_height != NULL)
	{
		font->SetTexHeight(atoi(tex_height->value()));
	}

	if (interval != NULL)
	{
		font->SetInterval(atoi(interval->value()));
	}

	if (bold != NULL)
	{
		if (stricmp(bold->value(), "true") == 0)
		{
			font->SetBold(true);
		}
		else
		{
			font->SetBold(false);
		}
	}

	if (smooth != NULL)
	{
		if (stricmp(smooth->value(), "true") == 0)
		{
			font->SetSmooth(true);
		}
		else
		{
			font->SetSmooth(false);
		}
	}

	if (italic != NULL)
	{
		if (stricmp(italic->value(), "true") == 0)
		{
			font->SetItalic(true);
		}
		else
		{
			font->SetItalic(false);
		}
	}

	if (underline != NULL)
	{
		if (stricmp(underline->value(), "true") == 0)
		{
			font->SetUnderline(true);
		}
		else
		{
			font->SetUnderline(false);
		}
	}

	if (strikeout != NULL)
	{
		if (stricmp(strikeout->value(), "true") == 0)
		{
			font->SetStrikeOut(true);
		}
		else
		{
			font->SetStrikeOut(false);
		}
	}

	if (var_extent != NULL)
	{
		if (stricmp(var_extent->value(), "true") == 0)
		{
			font->SetVarExtent(true);
		}
		else
		{
			font->SetVarExtent(false);
		}
	}

    if (outline_num != NULL)
    {
		font->SetOutlineNum(atol(outline_num->value()));
    }

	if(outline_color != NULL)
	{
		font->SetOutlineColor(outline_color->value());
	}

	if(fade_type != NULL)
	{
		font->SetFadeType(atoi(fade_type->value()));
	}

	if (shadow_color != NULL)
	{
		font->SetShadowColorString(shadow_color->value());
	}

	if (shadow_offset_x != NULL)
	{
		font->SetShadowOffsetX(atoi(shadow_offset_x->value()));
	}

	if (shadow_offset_y != NULL)
	{
		font->SetShadowOffsetY(atoi(shadow_offset_y->value()));
	}

	if (font_tex != NULL)
	{
		// 读取贴图字符位置信息
		xml_node<>* child = root->first_node("image");

		while (child)
		{
			xml_attribute<>* char_name = child->first_attribute("name");
			xml_attribute<>* char_left = child->first_attribute("left");
			xml_attribute<>* char_top = child->first_attribute("top");
			xml_attribute<>* char_right = child->first_attribute("right");
			xml_attribute<>* char_bottom = child->first_attribute("bottom");
			xml_attribute<>* char_width = child->first_attribute("width");
			xml_attribute<>* char_height = child->first_attribute("height");

			int ch = 0;

			if (char_name != NULL)
			{
				if (strlen(char_name->value()) == 1)
				{
					ch = char_name->value()[0];
				}
				else
				{
					const char* s = strchr(char_name->value(), 'x');

					if (s)
					{
						ch = hex_to_char(s + 1);
					}
				}
			}

			if ((NULL == char_left) || (NULL == char_top))
			{
				CORE_TRACE("(CDesignLoader::LoadXmlFont)char info error");
				CORE_TRACE(file);
				// 删除对象
				SAFE_RELEASE(font);
				return false;
			}

			int l = atoi(char_left->value());
			int t = atoi(char_top->value());
			int r = l + 16;
			int b = t + 16;

			if ((char_width != NULL) && (char_height != NULL))
			{
				r = l + atoi(char_width->value());
				b = t + atoi(char_height->value());
			}
			else if ((char_right != NULL) && (char_bottom != NULL))
			{
				r = atoi(char_right->value());
				b = atoi(char_bottom->value());
			}

			if (ch > 0)
			{
				font->AddTexCharInfo(ch, l, t, r, b);
			}

			child = child->next_sibling("image");
		}
	}

	// 所有界面资源都可被替换
	if (!pDesigner->GetGui()->AddFont(font->GetID()))
	{
		CORE_TRACE("(CDesignLoader::LoadXmlFont)add failed");
		CORE_TRACE(font->GetName());
		CORE_TRACE(file);
		// 删除对象
		SAFE_RELEASE(font);
		return false;
	}

	//设置字符偏移
	xml_node<>* child = root->first_node("char");
	IPainter* painter = (IPainter*)g_pCore->GetInterface("Painter");
	IFont* pFont = painter->GetFont(name->value());
	while(child)
	{
		xml_attribute<>* name = child->first_attribute("name");
		if(name)
		{
			xml_attribute<>* offsetx = child->first_attribute("offsetx");
			xml_attribute<>* offsety = child->first_attribute("offsety");
			result_wstring s = GuiUtil_Utf8ToUtf16(name->value());
			if(offsetx)
				pFont->SetXOffset(s.c_str()[0], atoi(offsetx->value()));
			if(offsety)
				pFont->SetYOffset(s.c_str()[0], atoi(offsety->value()));
		}
		child = child->next_sibling();
	}


	return true;
}

// 加载鼠标外观数据
static bool load_xml_cursor(Designer* pDesigner, const char* file, 
	xml_node<>* root, bool is_utf8)
{
	Assert(file != NULL);
	Assert(root != NULL);

	xml_attribute<>* name = root->first_attribute("name");
	xml_attribute<>* hotspot_x = root->first_attribute("hotspot_x");
	xml_attribute<>* hotspot_y = root->first_attribute("hotspot_y");
	xml_attribute<>* image = root->first_attribute("image");

	if (NULL == name)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlCursor)name not found");
		CORE_TRACE(file);
		return false;
	}

	Cursor* cursor = (Cursor*)pDesigner->GetCore()->GetEntity(
		pDesigner->GetGui()->NewCursor());

	if (NULL == cursor)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlCursor)create failed");
		CORE_TRACE(file);
		return false;
	}

	cursor->Create();
	cursor->SetName(name->value());

	if (hotspot_x != NULL)
	{
		cursor->SetHotSpotX(atoi(hotspot_x->value()));
	}

	if (hotspot_y != NULL)
	{
		cursor->SetHotSpotY(atoi(hotspot_y->value()));
	}

	if (image != NULL)
	{
		cursor->SetImage(image->value());
	}
	else
	{
		xml_node<>* element = root->first_node("normal");

		while (element)
		{
			xml_attribute<>* image = element->first_attribute("image");
			xml_attribute<>* interval = element->first_attribute("interval");

			if ((image != NULL) && (interval != NULL))
			{
				cursor->AddStateImage(Cursor::STATE_NORMAL,
					image->value(), (float)atof(interval->value()));
			}

			element = element->next_sibling("normal");
		}

		element = root->first_node("leftdown");

		while (element)
		{
			xml_attribute<>* image = element->first_attribute("image");
			xml_attribute<>* interval = element->first_attribute("interval");

			if ((image != NULL) && (interval != NULL))
			{
				cursor->AddStateImage(Cursor::STATE_LEFTDOWN,
					image->value(), (float)atof(interval->value()));
			}

			element = element->next_sibling("leftdown");
		}

		element = root->first_node("rightdown");

		while (element)
		{
			xml_attribute<>* image = element->first_attribute("image");
			xml_attribute<>* interval = element->first_attribute("interval");

			if ((image != NULL) && (interval != NULL))
			{
				cursor->AddStateImage(Cursor::STATE_RIGHTDOWN,
					image->value(), (float)atof(interval->value()));
			}

			element = element->next_sibling("rightdown");
		}
	}

	if (!pDesigner->GetGui()->AddCursor(cursor->GetID()))
	{
		CORE_TRACE("(CDesignLoader::LoadXmlCursor)add failed");
		CORE_TRACE(cursor->GetName());
		CORE_TRACE(file);
		// 删除对象
		SAFE_RELEASE(cursor);
		return false;
	}

	return true;
}

// 加载界面音效数据
static bool load_xml_sound(Designer* pDesigner, const char* file, 
	xml_node<>* root, bool is_utf8)
{
	Assert(file != NULL);
	Assert(root != NULL);

	xml_attribute<>* name_attr = root->first_attribute("name");
	xml_attribute<>* file_attr = root->first_attribute("filename");

	if (NULL == name_attr)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlSound)name not found");
		CORE_TRACE(file);
		return false;
	}

	if (NULL == file_attr)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlSound)filename not found");
		CORE_TRACE(file);
		return false;
	}

	const char* name = name_attr->value();
	const char* filename = file_attr->value();

	if (!pDesigner->GetGui()->AddSound(name, filename))
	{
		CORE_TRACE("(CDesignLoader::LoadXmlSound)add failed");
		CORE_TRACE(name);
		CORE_TRACE(file);
	}

	return true;
}

bool CDesignLoader::LoadXmlResFile(int type, const char* path, 
	const char* file)
{
	Assert(size_t(type) < CDesignRes::RESTYPE_MAX);
	Assert(path != NULL);
	Assert(file != NULL);

	CDesignRes* pDesignRes = m_pDesigner->GetDesignRes();

	if (pDesignRes->FindResFile(type, file))
	{
		return false;
	}

	char fname[256];

	if (StringEmpty(path))
	{
		SafeSprintf(fname, sizeof(fname), "%s%s", 
			m_pDesigner->GetDefaultPath(), file);
	}
	else
	{
		SafeSprintf(fname, sizeof(fname), "%s%s", path, file);
	}

	FILE* fp = core_file::fopen(fname, "rb");

	if (NULL == fp)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlResFile)open file failed");
		CORE_TRACE(file);
		return false;
	}

	core_file::fseek(fp, 0, SEEK_END);
	size_t file_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);

	TAutoMem<char, 256, TCoreAlloc> auto_buf(file_size + 1);
	char* buffer = auto_buf.GetBuffer();

	if (core_file::fread(buffer, sizeof(char), file_size, fp) != file_size)
	{
		core_file::fclose(fp);
		CORE_TRACE("(CDesignLoader::LoadXmlResFile)read file error");
		CORE_TRACE(file);
		return false;
	}

	core_file::fclose(fp);

	buffer[file_size] = 0;

	xml_document<> doc;

	try
	{
		doc.parse<parse_declaration_node>(buffer);
	}
	catch (parse_error& e)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlResFile)parse xml error");
		CORE_TRACE(e.what());
		CORE_TRACE(file);
		return false;
	}

	// 获得编码方式
	xml_node<>* declare = doc.first_node();
	bool is_utf8 = false;

	if (declare)
	{
		xml_attribute<>* encoding_attr = declare->first_attribute("encoding");

		if (encoding_attr)
		{
			if (stricmp(encoding_attr->value(), "utf-8") == 0)
			{
				is_utf8 = true;
			}
		}
	}

	result_string res_name = m_pDesigner->GetDesignRes()->GetResName(type);
	result_string res_names = res_name + "s";
	xml_node<>* root = doc.first_node(res_names.c_str());

	if (NULL == root)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlResFile)no root element");
		CORE_TRACE(file);
		return NULL;
	}

	xml_node<>* element = root->first_node(res_name.c_str());

	while (element)
	{
		switch (type)
		{
		//case CDesignRes::RESTYPE_COLOR:
		//	load_xml_color(m_pDesigner, file, element, is_utf8);
		//	break;
		//case CDesignRes::RESTYPE_IMAGE:
		//	load_xml_image(m_pDesigner, file, element, is_utf8);
		//	break;
		case CDesignRes::RESTYPE_FONT:
			load_xml_font(m_pDesigner, file, element, is_utf8);
			break;
		case CDesignRes::RESTYPE_CURSOR:
			load_xml_cursor(m_pDesigner, file, element, is_utf8);
			break;
		case CDesignRes::RESTYPE_SOUND:
			load_xml_sound(m_pDesigner, file, element, is_utf8);
			break;
		default:
			Assert(0);
			break;
		}

		element = element->next_sibling(res_name.c_str());
	}

	return true;
}

// 加载附属控件数据
static IControl* load_xml_subor(Designer* pDesigner, IControl* parent, 
	const char* file, xml_node<>* root, bool is_utf8)
{
	Assert(parent != NULL);
	Assert(root != NULL);

	// 名称
	xml_attribute<>* name = root->first_attribute("name");

	if (NULL == name)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlSubor)name not found");
		CORE_TRACE(file);
		return NULL;
	}

	// 获得对应的属性
	CVar prop;

	if (!pDesigner->GetCore()->GetProperty(parent, name->value(), prop))
	{
		CORE_TRACE("(CDesignLoader::LoadXmlSubor)property not found");
		CORE_TRACE(name->value());
		CORE_TRACE(file);
		return NULL;
	}

	// 获得附属控件
	IControl* control = pDesigner->GetControl(prop.ObjectVal());

	if (NULL == control)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlSubor)subor control invalid");
		CORE_TRACE(name->value());
		CORE_TRACE(file);
		return NULL;
	}

	// 加载属性
	xml_node<>* element = root->first_node("prop");

	if (element)
	{
		xml_attribute<>* attr = element->first_attribute();
		
		while (attr)
		{
			pDesigner->SetControlProperty(control, attr->name(), 
				attr->value(), is_utf8);

			attr = attr->next_attribute();
		}
	}
	else
	{
		// 加载属性值
		element = root->first_node("property");

		while (element)
		{
			xml_attribute<>* prop_name = element->first_attribute("name");
			xml_attribute<>* prop_value = element->first_attribute("value");

			if ((prop_name != NULL) && (prop_value != NULL))
			{
				pDesigner->SetControlProperty(control, prop_name->value(), 
					prop_value->value(), is_utf8);
			}

			element = element->next_sibling("property");
		}
	}

	// 加载下一级附属控件
	element = root->first_node("subor");

	while (element)
	{
		load_xml_subor(pDesigner, control, file, element, is_utf8);

		element = element->next_sibling("subor");
	}

	return control;
}

// 加载控件数据 
static IControl* load_xml_control(Designer* pDesigner, IControl* parent, 
	const char* file, xml_node<>* root, bool is_desktop, bool is_utf8, 
	bool async)
{
	Assert(root != NULL);

	// 实体名称
	xml_attribute<>* entity = root->first_attribute("entity");

	if (NULL == entity)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlControl)entity name not found");
		CORE_TRACE(file);
		return NULL;
	}

	IControl* control;

	if (is_desktop)
	{
		// 主窗口
		Assert(NULL == parent);

		control = pDesigner->GetDesktopForm();

		if (NULL == control)
		{
			CORE_TRACE("(CDesignLoader::LoadXmlControl)desktop not create");
			CORE_TRACE(file);
			return NULL;
		}

		// 必须先删除所有子控件
		control->DeleteAllChild();
		// 必须释放背景贴图
		control->RelaseBackImage();
	}
	else
	{
		control = pDesigner->CreateControl(entity->value());

		if (NULL == control)
		{
			CORE_TRACE(
				"(CDesignLoader::LoadXmlControl)create control failed");
			CORE_TRACE(entity->value());
			CORE_TRACE(file);
			return NULL;
		}
	}

	// 设置加载模式
	control->SetAsyncLoad(async);

	xml_attribute<>* script_attr = root->first_attribute("script");
	xml_attribute<>* init = root->first_attribute("init");
	xml_attribute<>* control_name = root->first_attribute("name");

	// 控件名称
	if (!string_null_empty(control_name))
	{
		control->SetName(control_name->value());

		if (pDesigner->GetDesignMode())
		{
			// 添加到窗口或窗口的成员名称到表中
			if (!pDesigner->FindMember(control_name->value()))
			{
				pDesigner->AddMember(control_name->value());
			}
			else
			{
				control->Release();

				CORE_TRACE(
					"(CDesignLoader::LoadXmlControl)member name exists");
				CORE_TRACE(control_name->value());
				CORE_TRACE(file);
				return NULL;
			}
		}
	}

//	// 脚本名
//	const char* script = "";
//
//	if (!string_null_empty(script_attr))
//	{
//		script = script_attr->value();
//	}
//
//	if (script[0] == 0)
//	{
//		if ((parent != NULL) && (parent->GetScript() != NULL))
//		{
//			script = parent->GetScript()->GetName();
//		}
//	}
//
//	if (!pDesigner->GetDesignMode())
//	{
//		if (script[0] != 0)
//		{
//			// 绑定脚本
//			if (!pDesigner->GetCore()->BindScript(control, script))
//			{
//				CORE_TRACE(
//					"(CDesignLoader::LoadXmlControl)bind script failed");
//				CORE_TRACE(script);
//				CORE_TRACE(control_name->value());
//				CORE_TRACE(file);
//			}
//		}
//	}

	// 加载属性
	xml_node<>* element = root->first_node("prop");

	if (element)
	{
		xml_attribute<>* attr = element->first_attribute();

		while (attr)
		{
			pDesigner->SetControlProperty(control, attr->name(), 
				attr->value(), is_utf8);

			attr = attr->next_attribute();
		}
	}
	else
	{
		// 旧格式的属性数据
		element = root->first_node("property");

		while (element)
		{
			xml_attribute<>* prop_name = element->first_attribute("name");
			xml_attribute<>* prop_value = element->first_attribute("value");

			if ((prop_name != NULL) && (prop_value != NULL))
			{
				pDesigner->SetControlProperty(control, prop_name->value(), 
					prop_value->value(), is_utf8);
			}

			element = element->next_sibling("property");
		}
	}

//	// 加载回调信息
//	if (pDesigner->GetDesignMode())
//	{
//		IVar& callbacks = control->GetVar()->GetChild("Callbacks");
//
//		callbacks.SetTable();
//	}

//	element = root->first_node("event");
//
//	if (element)
//	{
//		xml_attribute<>* attr = element->first_attribute();
//
//		while (attr)
//		{
//			const char* name = attr->name();
//			const char* func = attr->value();
//
//			if (!pDesigner->GetDesignMode())
//			{
//				IEntScript* pScript = control->GetScript();
//
//				if (pScript != NULL)
//				{
//					if (!pScript->AddCallback(name, "", func))
//					{
//						CORE_TRACE("(CDesignLoader::LoadXmlControl)"
//							"add callback failed");
//						CORE_TRACE(control_name->value());
//						CORE_TRACE(file);
//					}
//				}
//			}
//
//			// 加入到回调表中
//			if (pDesigner->GetDesignMode())
//			{
//				control->AddCallback(name, func);
//			}
//
//			attr = attr->next_attribute();
//		}
//	}
//	else
//	{
//		// 旧格式的回调数据
//		element = root->first_node("callback");
//
//		while (element)
//		{
//			xml_attribute<>* name = element->first_attribute("name");
//			xml_attribute<>* func = element->first_attribute("function");
//
//			if ((name != NULL) && (func != NULL))
//			{
//				if (!pDesigner->GetDesignMode())
//				{
//					IEntScript* pScript = control->GetScript();
//
//					if (pScript != NULL)
//					{
//						if (!pScript->AddCallback(name->value(), "", 
//							func->value()))
//						{
//							CORE_TRACE("(CDesignLoader::LoadXmlControl)"
//								"add callback failed");
//							CORE_TRACE(control_name->value());
//							CORE_TRACE(file);
//						}
//					}
//				}
//
//				// 加入到回调表中
//				if (pDesigner->GetDesignMode())
//				{
//					control->AddCallback(name->value(), func->value());
//				}
//			}
//
//			element = element->next_sibling("callback");
//		}
//	}
//
//	// 运行初始化脚本函数
//	if (!pDesigner->GetDesignMode()) 
//	{
//		if ((script[0] != 0) && (!string_null_empty(init)))
//		{
//			pDesigner->GetCore()->ExecAsyncProc(script, init->value(), 
//				CVarList() << control->GetID());
//		}
//	}
//
//	// 记录脚本名和初始化函数名
//	if (pDesigner->GetDesignMode())
//	{
//		if (NULL == init)
//		{
//			control->GetCustoms()->Assign("InitFunc", CVar(VTYPE_STRING, ""));
//		}
//		else
//		{
//			control->GetCustoms()->Assign("InitFunc", 
//				CVar(VTYPE_STRING, init->value()));
//		}
//
//		if (NULL == script_attr)
//		{
//			control->GetCustoms()->Assign("ScriptName", 
//				CVar(VTYPE_STRING, ""));
//		}
//		else
//		{
//			control->GetCustoms()->Assign("ScriptName",
//				CVar(VTYPE_STRING, script_attr->value()));
//		}
//	}

	// 添加到父容器中
	if (parent != NULL) 
	{
		parent->AddChild(control);
	}

	// 加载所有的附属控件
	element = root->first_node("subor");

	while (element)
	{
		load_xml_subor(pDesigner, control, file, element, is_utf8);

		element = element->next_sibling("subor");
	}

	// 加载所有的子控件
	element = root->first_node("control");

	while (element)
	{
		IControl* ctrl = load_xml_control(pDesigner, control, file, element, 
			false, is_utf8, async);

		if (ctrl != NULL)
		{
			// 将控件名作为成员添加到窗口的自定义属性中
			Form* form = (Form*)control->GetRootControl();

			form->GetCustoms()->Assign(ctrl->GetName(), 
				CVar(VTYPE_OBJECT, ctrl->GetID()));
		}
		else
		{
			CORE_TRACE("(CDesignLoader::LoadXmlControl)load child failed");
			CORE_TRACE(file);
		}

		element = element->next_sibling("control");
	}

	return control;
}

bool CDesignLoader::LoadDesktop(const char* path, const char* file,
	bool async_load)
{
	Assert(path != NULL);
	Assert(file != NULL);

	// 不能在设计状态下
	if (m_pDesigner->GetDesignMode())
	{
		CORE_TRACE("(CDesignLoader::LoadDesktop)can't in design");
		CORE_TRACE(file);
		return false;
	}

	char fname[256];

	if (StringEmpty(path))
	{
		SafeSprintf(fname, sizeof(fname), "%s%s", 
			m_pDesigner->GetDefaultPath(), file);
	}
	else
	{
		SafeSprintf(fname, sizeof(fname), "%s%s", path, file);
	}

	FILE* fp = core_file::fopen(fname, "rb");

	if (NULL == fp)
	{
		CORE_TRACE("(CDesignLoader::LoadDesktop)open file failed");
		CORE_TRACE(file);
		return false;
	}

	core_file::fseek(fp, 0, SEEK_END);
	size_t file_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);

	TAutoMem<char, 256, TCoreAlloc> auto_buf(file_size + 1);
	char* buffer = auto_buf.GetBuffer();

	if (core_file::fread(buffer, sizeof(char), file_size, fp) != file_size)
	{
		core_file::fclose(fp);
		CORE_TRACE("(CDesignLoader::LoadDesktop)read file error");
		CORE_TRACE(file);
		return false;
	}

	core_file::fclose(fp);

	buffer[file_size] = 0;

	xml_document<> doc;

	try
	{
		doc.parse<parse_declaration_node>(buffer);
	}
	catch (parse_error& e)
	{
		CORE_TRACE("(CDesignLoader::LoadDesktop)parse xml error");
		CORE_TRACE(e.what());
		CORE_TRACE(file);
		return false;
	}

	// 获得编码方式
	xml_node<>* declare = doc.first_node();
	bool is_utf8 = false;

	if (declare)
	{
		xml_attribute<>* encoding_attr = declare->first_attribute("encoding");

		if (encoding_attr)
		{
			if (stricmp(encoding_attr->value(), "utf-8") == 0)
			{
				is_utf8 = true;
			}
		}
	}

	xml_node<>* root = doc.first_node("form");

	if (NULL == root)
	{
		CORE_TRACE("(CDesignLoader::LoadXmlBase)no root element");
		CORE_TRACE(file);
		return NULL;
	}

	// 加载必要的资源集合
	xml_attribute<>* res_attr = root->first_attribute("resource");

	const char* resource;

	if (string_null_empty(res_attr))
	{
		resource = m_pDesigner->GetDefResourceName();
	}
	else
	{
		resource = res_attr->value();
	}

	char res_name[256];

	SafeSprintf(res_name, sizeof(res_name), "%s%s", path, resource);

	if (!m_pDesigner->GetDesignRes()->FindCollect(res_name))
	{
		if (!LoadResource(path, resource))
		{
			CORE_TRACE("(CDesignLoader::LoadDesktop)load resource failed");
			CORE_TRACE(res_name);
		}
	}

	xml_node<>* root_control = root->first_node("control");

	if (NULL == root_control)
	{
		CORE_TRACE("(CDesignLoader::LoadDesktop)root control not found");
		CORE_TRACE(file);
		return false;
	}

	// 根节点必须为窗口
	xml_attribute<>* entity = root_control->first_attribute("entity");

	if ((NULL == entity) || (strcmp(entity->value(), "Form") != 0))
	{
		CORE_TRACE("(CDesignLoader::LoadDesktop)root control must Form");
		CORE_TRACE(file);
		return false;
	}

	// 加载主窗口
	IControl* res = load_xml_control(m_pDesigner, NULL, file, root_control, 
		true, is_utf8, async_load);

	if (NULL == res)
	{
		CORE_TRACE("(CDesignLoader::LoadDesktop)load Form failed");
		CORE_TRACE(file);
		return false;
	}

	return true;
}

//
PERSISTID CDesignLoader::LoadForm(const char* path, const char* file, bool bAsyncLoad, bool bStatic)
{
	Assert(path != NULL);
	Assert(file != NULL);

	// 清空成员名表
	if (m_pDesigner->GetDesignMode())
	{
		m_pDesigner->ClearMember();
	}

	char fname[256];

	if (StringEmpty(path))
	{
		SafeSprintf(fname, sizeof(fname), "%s%s", 
			m_pDesigner->GetDefaultPath(), file);
	}
	else
	{
		SafeSprintf(fname, sizeof(fname), "%s%s", path, file);
	}

	//
	char* szBuff = NULL;

	if (bStatic)
	{
		TStringPod<char, size_t>::iterator it = m_mapIniFile.Find(file);
		if (it == m_mapIniFile.End())
		{
			FILE* fp = core_file::fopen(fname, "rb");

			if (NULL == fp)
			{
				CORE_TRACE_EX("(CDesignLoader::LoadForm)open file failed. %s", file);
				return PERSISTID();
			}

			core_file::fseek(fp, 0, SEEK_END);
			size_t file_size = core_file::ftell(fp);
			core_file::fseek(fp, 0, SEEK_SET);

			TAutoMem<char, 256, TCoreAlloc> auto_buf(file_size + 1);
			char* buff = auto_buf.GetBuffer();

			if (core_file::fread(buff, sizeof(char), file_size, fp) != file_size)
			{
				core_file::fclose(fp);
				CORE_TRACE_EX("(CDesignLoader::LoadForm)read file error %s", file);
				return PERSISTID();
			}

			core_file::fclose(fp);
			buff[file_size] = 0;


			//
			size_t iAllcSize = strlen(buff);

			if (iAllcSize>0)
			{
				szBuff = NEW char[iAllcSize+1];
				strcpy(szBuff, buff);

				char* szBuffSave = NEW char[iAllcSize+1];
				strcpy(szBuffSave, buff);

				m_mapIniFile.Add(file, m_vecFileInf.size());
				m_vecFileInf.push_back(szBuffSave);  
			}
			else
			{
				return PERSISTID();
			}
		}
		else
		{
			//
			size_t iIndex = it.GetData();
			if (iIndex < m_vecFileInf.size())
			{
				//
				size_t iAllcSize = strlen(m_vecFileInf[iIndex]);

				szBuff = NEW char[iAllcSize+1];
				strcpy(szBuff, m_vecFileInf[iIndex]);
			}
			else
			{
				return PERSISTID();
			}
		}
	}
	else
	{
		FILE* fp = core_file::fopen(fname, "rb");

		if (NULL == fp)
		{
			CORE_TRACE("(CDesignLoader::LoadForm)open file failed");
			CORE_TRACE(file);
			return PERSISTID();
		}

		core_file::fseek(fp, 0, SEEK_END);
		size_t file_size = core_file::ftell(fp);
		core_file::fseek(fp, 0, SEEK_SET);

		TAutoMem<char, 256, TCoreAlloc> auto_buf(file_size + 1);
		char*buffer = auto_buf.GetBuffer();

		if (core_file::fread(buffer, sizeof(char), file_size, fp) != file_size)
		{
			core_file::fclose(fp);
			CORE_TRACE("(CDesignLoader::LoadForm)read file error");
			CORE_TRACE(file);
			return PERSISTID();
		}

		core_file::fclose(fp);

		buffer[file_size] = 0;

		//
		size_t iAllcSize = strlen(buffer);

		if (iAllcSize>0)
		{
			szBuff = NEW char[iAllcSize+1];
			strcpy(szBuff, buffer);
		}
		else
		{
			return PERSISTID();
		}
	}

	xml_document<> doc;

	try
	{
		doc.parse<parse_declaration_node>(szBuff);
	}
	catch (parse_error& e)
	{
		CORE_TRACE("(CDesignLoader::LoadForm)parse xml error");
		CORE_TRACE(e.what());
		CORE_TRACE(file);
		return PERSISTID();
	}

	// 获得编码方式
	xml_node<>* declare = doc.first_node();
	bool is_utf8 = false;

	if (declare)
	{
		xml_attribute<>* encoding_attr = declare->first_attribute("encoding");

		if (encoding_attr)
		{
			if (stricmp(encoding_attr->value(), "utf-8") == 0)
			{
				is_utf8 = true;
			}
		}
	}

	xml_node<>* root = doc.first_node("form");

	if (NULL == root)
	{
		CORE_TRACE("(CDesignLoader::LoadForm)no root element");
		CORE_TRACE(file);
		return PERSISTID();
	}

//	// 加载必要的资源集合
//	xml_attribute<>* res_attr = root->first_attribute("resource");
//	const char* resource;
//
//	if (string_null_empty(res_attr))
//	{
//		resource = m_pDesigner->GetDefResourceName();
//	}
//	else
//	{
//		resource = res_attr->value();
//	}
//
//	char res_name[256];
//
//	SafeSprintf(res_name, sizeof(res_name), "%s%s", path, resource);
//
//	if (!m_pDesigner->GetDesignRes()->FindCollect(res_name))
//	{
//		if (!LoadResource(path, resource))
//		{
//			CORE_TRACE("(CDesignLoader::LoadForm)load resource failed");
//			CORE_TRACE(res_name);
//		}
//	}

	xml_node<>* root_control = root->first_node("control");

	if (NULL == root_control)
	{
		CORE_TRACE("(CDesignLoader::LoadForm)root control not found");
		CORE_TRACE(file);
		return PERSISTID();
	}

	// 必须为窗口
	xml_attribute<>* entity = root_control->first_attribute("entity");

	if ((NULL == entity) || (strcmp(entity->value(), "Form") != 0))
	{
		CORE_TRACE("(CDesignLoader::LoadForm)root control must Form");
		CORE_TRACE(file);
		return PERSISTID();
	}

	IControl* res = load_xml_control(m_pDesigner, NULL, file, root_control, 
		false, is_utf8, bAsyncLoad);

	if (szBuff)
	{
		delete[] szBuff;
		szBuff = NULL;
	}

	if (NULL == res)
	{
		return PERSISTID();
	}

	if (m_pDesigner->GetDesignMode())
	{
		m_pDesigner->SetDesignFormID(res->GetID());
	}

	return res->GetID();
}

bool CDesignLoader::LoadXmlRes(int type, const char* path, const char* file)
{
	if (!LoadXmlResFile(type, path, file))
	{
		return false;
	}

	if (m_pDesigner->GetDesignMode())
	{
		m_pDesigner->GetDesignRes()->AddResFile(type, file);
	}

	return true;
}

bool CDesignLoader::LoadResource(const char* path, const char* file)
{
	Assert(path != NULL);
	Assert(file != NULL);

	char fname[256];

	if (StringEmpty(path))
	{
		SafeSprintf(fname, sizeof(fname), "%s%s", 
			m_pDesigner->GetDefaultPath(), file);
	}
	else
	{
		SafeSprintf(fname, sizeof(fname), "%s%s", path, file);
	}

	FILE* fp = core_file::fopen(fname, "rb");

	if (NULL == fp)
	{
		CORE_TRACE("(CDesignLoader::LoadResource)open file failed");
		CORE_TRACE(file);
		return false;
	}

	core_file::fseek(fp, 0, SEEK_END);
	size_t file_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);

	TAutoMem<char, 256, TCoreAlloc> auto_buf(file_size + 1);
	char* buffer = auto_buf.GetBuffer();

	if (core_file::fread(buffer, sizeof(char), file_size, fp) != file_size)
	{
		core_file::fclose(fp);
		CORE_TRACE("(CDesignLoader::LoadResource)read file error");
		CORE_TRACE(file);
		return false;
	}

	core_file::fclose(fp);
	buffer[file_size] = 0;

	xml_document<> doc;

	try
	{
		doc.parse<parse_declaration_node>(buffer);
	}
	catch (parse_error& e)
	{
		CORE_TRACE("(CDesignLoader::LoadResource)parse xml error");
		CORE_TRACE(e.what());
		CORE_TRACE(file);
		return false;
	}

	// 获得编码方式
	xml_node<>* declare = doc.first_node();

	bool is_utf8 = false;

	if (declare)
	{
		xml_attribute<>* encoding_attr = declare->first_attribute("encoding");

		if (encoding_attr)
		{
			if (stricmp(encoding_attr->value(), "utf-8") == 0)
			{
				is_utf8 = true;
			}
		}
	}

	xml_node<>* root = doc.first_node("resource");

	if (NULL == root)
	{
		CORE_TRACE("(CDesignLoader::LoadResource)no root element");
		CORE_TRACE(file);
		return NULL;
	}

	// 清空资源文件名表
	if (m_pDesigner->GetDesignMode())
	{
		m_pDesigner->ClearResFile("");
	}

//	// 颜色
//	xml_node<>* element = root->first_node("colors");
//
//	while (element)
//	{
//		xml_attribute<>* name = element->first_attribute("name");
//
//		if (name != NULL)
//		{
//			LoadXmlRes(CDesignRes::RESTYPE_COLOR, path, name->value());
//		}
//
//		element = element->next_sibling("colors");
//	}
//
//	// 图片
//	element = root->first_node("images");
//
//	while (element)
//	{
//		xml_attribute<>* name = element->first_attribute("name");
//
//		if (name != NULL)
//		{
//			LoadXmlRes(CDesignRes::RESTYPE_IMAGE, path, name->value());
//		}
//
//		element = element->next_sibling("images");
//	}

	// 字体
	xml_node<>* element = root->first_node("fonts");

	while (element)
	{
		xml_attribute<>* name = element->first_attribute("name");

		if (name != NULL)
		{
			LoadXmlRes(CDesignRes::RESTYPE_FONT, path, name->value());
		}

		element = element->next_sibling("fonts");
	}

	// 鼠标
	element = root->first_node("cursors");

	while (element)
	{
		xml_attribute<>* name = element->first_attribute("name");

		if (name != NULL)
		{
			LoadXmlRes(CDesignRes::RESTYPE_CURSOR, path, name->value());
		}

		element = element->next_sibling("cursors");
	}

	// 声音
	element = root->first_node("sounds");

	while (element)
	{
		xml_attribute<>* name = element->first_attribute("name");

		if (name != NULL)
		{
			LoadXmlRes(CDesignRes::RESTYPE_SOUND, path, name->value());
		}

		element = element->next_sibling("sounds");
	}

	// 设置当前的资源集合文件名以防止重复加载
	if (!m_pDesigner->GetDesignRes()->FindCollect(fname))
	{
		m_pDesigner->GetDesignRes()->AddCollect(fname);
	}

	return true;
}
