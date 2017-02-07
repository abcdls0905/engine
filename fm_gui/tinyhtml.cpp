//--------------------------------------------------------------------
// 文件名:		tinyhtml.cpp
// 内  容:		解析HTML文件的类
// 说  明:		实现图文混排框数据读取解析类
// 创建日期:	2006年10月24日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "tinyhtml.h"
#include "../tools/string_util.h"
#include "../public/auto_mem.h"
#include "../public/Converts.h"
#include "../public/core_file.h"
#include <sstream>

class VoyWToke
{
private:
	std::wstring m_str;
	std::wstring m_seg;
	int m_pos;

public:
	VoyWToke(const std::wstring& str, const std::wstring& seg)
		: m_str(str), m_seg(seg), m_pos(0)
	{
	}

	bool FindNext(std::wstring& ss)
	{
		int pos = m_str.find(m_seg,m_pos);

		if (m_pos == std::wstring::npos)
		{
			return false;
		}

		int end	= pos;

		if (pos != std::wstring::npos)
		{
			pos++;

			while (true)
			{
				bool bfind = false;

				for (size_t i = 0; i < m_seg.size(); ++i)
				{
					if (m_str[pos] == m_seg[i])
					{
						pos++;
						bfind=true;
						break;
					}
				}

				if (!bfind)
				{
					break;
				}
			}
		}

		int begin = m_pos;

		m_pos = pos;		

		ss = m_str.substr(begin, end - begin);

		return true;
	}

	std::wstring& FindNext2(std::wstring& s)
	{
		if (!FindNext(s))
		{
			s = L"";
		}

		return s;
	}
};

rect_t ParseToRect(const wchar_t* xmldata)
{
	rect_t rect = {0,0,0,0};

	VoyWToke v(xmldata,L",");

	std::wstring s;

	if (!v.FindNext(s))
		return rect;

	rect.left = wcstol(s.c_str(), NULL, 10);

	if (!v.FindNext(s))
		return rect;

	rect.top = wcstol(s.c_str(), NULL, 10);

	if(!v.FindNext(s))
		return rect;

	rect.right = wcstol(s.c_str(), NULL, 10);

	if(!v.FindNext(s))
		return rect;

	rect.bottom	= wcstol(s.c_str(), NULL, 10);

	return rect;
}

TinyHtml::TinyHtml()
{
	m_defColor = 0xff00ffff;
	m_defFont  = "Default";
	m_defSize  = 12;
}

TinyHtml::~TinyHtml()
{
	Clean();
}

void TinyHtml::Clean()
{
	m_data = L"";
	m_Segments.clear();
}

bool TinyHtml::CreateFromHtml(const char* szFileName,  dword defColor,
							  const char* defFont, long defSize)
{
	Clean();

	m_defColor = defColor;
	m_defFont = defFont;
	m_defSize = defSize;

	if (LoadFile(szFileName) && Analysis())
	{
		return true;
	}

	return false;
}

bool TinyHtml::SetText(const wchar_t* szText, dword defColor, 
					   const char* defFont, long defSize)
{
	Clean();

	m_defColor = defColor;
	m_defFont = defFont;
	m_defSize = defSize;
	m_data = szText;

	if (Analysis())
	{
		return true;
	}

	return false;
}

bool TinyHtml::LoadFile( const char* filename)
{
	if (filename == NULL)
	{
		return false;
	}

	// Delete the existing data:
	m_data = L"";
	// There was a really terrifying little bug here. The code:
	//		value = filename
	// in the STL case, cause the assignment method of the std::string to
	// be called. What is strange, is that the std::string had the same
	// address as it's c_str() method, and so bad things happen. Looks
	// like a bug in the Microsoft STL implementation.
	// See STL_STRING_BUG above.
	// Fixed with the StringToBuffer class.
	std::string value = filename;

	FILE* file = core_file::fopen(value.c_str (), "rb");
	
	if (file)
	{
		// Get the file size, so we can pre-allocate the string. HUGE speed impact.
		dword length = 0;	

		core_file::fseek(file, 0, SEEK_END);

		length = core_file::ftell(file);

		core_file::fseek(file, 0, SEEK_SET);

		// Strange case, but good to handle up front.
		if (length == 0)
		{
			core_file::fclose(file);
			return false;
		}

		// If we have a file, assume it is all one big XML file, and read it in.
		// The document parser may decide the document ends sooner than the entire file, however.

		m_data.reserve(length);

		TAutoMem<char, 256> auto_buf1(length + 1);

		char* pBuf	= auto_buf1.GetBuffer();

		pBuf[length] = 0;

		//if (fgets(pBuf, length, file))
		if (core_file::fread(pBuf, sizeof(char), length, file) == length)
		{
			TAutoMem<wchar_t, 256> auto_buf2(length + 1);

			wchar_t* pWBuffer = auto_buf2.GetBuffer();

			ToWideStr(pBuf, pWBuffer, length);

			m_data = pWBuffer;
		}

		core_file::fclose(file);

		return true;
	}
	
	return false;
}

dword HexStrToDecValue(const wchar_t*szStr)
{
	//assert(szStr);
	if (szStr == NULL || szStr[0] == 0)
	{
		return 0L;
	};

	std::wstring szSource = szStr;

    if (szSource.size() < 2)
    {
        return 0L;
    }

	std::wstring szFirtFlag = szSource.substr(0,2);

	if (wcscmp(szFirtFlag.c_str(), L"0x") == 0)
	{
		szSource = szSource.substr(2, szSource.size() - 2);
	}

	dword Value = 0;
	long  mi = 1;//冪
	long  l	= 0;
	
	for (int i = szSource.size() - 1; i > -1; i--)
	{
		if (szSource[i] >= 97) //abcdef
		{
			l = (int)(szSource[i] - 87) * mi;
		}
		else if (szSource[i] >= 65)
		{
			l = (int)(szSource[i]-55) * mi;
		}
		else if (szSource[i] >= 48)
		{
			l = (int)(szSource[i] - 48) * mi;
		}

		Value += l;
		mi = mi * 16;
	}

	return Value;
}

bool TinyHtml::Analysis()
{
	// 转成窄字符用的缓存
	const long lStringBufferLength = 260;//MAX_PATH;#define MAX_PATH          260
	TAutoMem<char, 256> auto_buf( lStringBufferLength+1 );
	char* pStringBuffer = auto_buf.GetBuffer();

	TAutoMem<wchar_t, 256> auto_wbuf( lStringBufferLength+1 );
	wchar_t* pWStringBuffer = auto_wbuf.GetBuffer();

	long front,back;

	std::wstring szBuffer = m_data;

	front = szBuffer.find(L"<body>");
	back  = szBuffer.find(L"</body>");

    if (front != std::wstring::npos && back != std::wstring::npos && front < back)
	{
		szBuffer = szBuffer.substr(front + 6, back - front - 6);
	}

	//cout<<"总:"<<szBuffer.c_str();
	TextSegment Segment;
	bool bIsEnd = false;
	bool bProcessCenter = false;

	while (szBuffer.size() > 0)
	{
		front = szBuffer.find(L"<");

        // 找不到直接结束
        if (front == std::wstring::npos)
		{
			// 没有任何标签
			HtmlTextItem hTextItem(m_defColor,m_defFont.c_str(),m_defSize);
			hTextItem.m_wtext = szBuffer;
			Segment.m_TextItemList.push_back(hTextItem);
			Segment.m_processlist.push_back(true);
			m_Segments.push_back(Segment);
			break;
		}
        // 找到了并且中间有值时才处理,不然继续
        else if(front > 0)
		{
		    HtmlTextItem hTextItem(m_defColor,m_defFont.c_str(),m_defSize);
		    hTextItem.m_wtext = szBuffer.substr(0,front);
            szBuffer = szBuffer.substr(front, szBuffer.size() - front);
            front = 0;
		    Segment.m_TextItemList.push_back(hTextItem);
		    Segment.m_processlist.push_back(true);
		}

		if (bProcessCenter)
		{
			if (szBuffer[front + 1] == '/')
			{
				bProcessCenter = false;
				szBuffer = szBuffer.substr(front+strlen("</center>"),
					szBuffer.size()-front-strlen("</center>"));

				m_Segments.push_back(Segment);
				//Segment.TextSegment();
				{
					Segment.m_bInsertTwoSpace=false;
					Segment.m_bCenter = false;
					Segment.m_TextItemList.clear();
					Segment.m_ImageItemList.clear();
                    Segment.m_processlist.clear();
				}

				continue;
			}
		}

		back = szBuffer.find(L">");
		// 标签配置信息
		std::wstring szNamePro = L"";
		// 取前面一段标签配置
		long test = szBuffer.find(L"/>");

        if (back != std::wstring::npos)
        {
		    if (test != std::wstring::npos && back > test)
		    {
			    //<img src="XXX" /> 这种情况取img src="XXX"
			    szNamePro = szBuffer.substr(front+1,back-front-2);
		    }
		    else
		    {
			    //<font size="XX">fadfasdfsd</font>这种情况取font size="XX"
			    szNamePro = szBuffer.substr(front+1,back-front-1);
		    }
        }
        else
        {
            // 标签不配对,或异常
            HtmlTextItem hTextItem(m_defColor,m_defFont.c_str(),m_defSize);
            hTextItem.m_wtext = szBuffer;
            Segment.m_TextItemList.push_back(hTextItem);
            Segment.m_processlist.push_back(true);
            m_Segments.push_back(Segment);
            break;
        }

		szBuffer = szBuffer.substr(back+1, szBuffer.size()-back);

		VoyWToke splitHtml(szNamePro, L" ");
		std::wstring szName; //标检名
		splitHtml.FindNext(szName);
		// 字体
		if (wcsicmp(szName.c_str(), L"font")==0)
		{   
			//color face size 仅考虑这三个属性
			HtmlTextItem hTextItem(m_defColor,m_defFont.c_str(),m_defSize);
			std::wstring szProperty;

			while (splitHtml.FindNext(szProperty))
			{
				int nFind = szProperty.find(L"color");
				if (nFind != std::wstring::npos)
				{
					nFind = szProperty.find(L"#");

                    if (nFind != std::wstring::npos && nFind + 7 < (int)szProperty.size())
                    {
					    std::wstring Value = szProperty.substr(nFind+1, 6);
					    Value = L"FF" +Value;
					    hTextItem.m_Color = HexStrToDecValue(Value.c_str());
                    }
                    else
                    {
                        // 格式不对
                        // 标签不配对,或异常
                        HtmlTextItem hTextItem(m_defColor,m_defFont.c_str(),m_defSize);
                        hTextItem.m_wtext = szNamePro + szBuffer;
                        Segment.m_TextItemList.push_back(hTextItem);
                        Segment.m_processlist.push_back(true);
                        m_Segments.push_back(Segment);
                        break;
                    }
				}
				else
				{
					nFind = szProperty.find(L"face");
					if (nFind != std::wstring::npos)
					{
						long front = szProperty.find(L"\"");

                        if (front != std::wstring::npos)
                        {
                            szProperty = szProperty.substr(front+ 1, szProperty.size() - front);
                        }
						
						long back = szProperty.find(L"\"");

                        if (back != std::wstring::npos)
                        {
						    std::wstring Value = szProperty.substr(0, back);
						    ToString(Value.c_str(), pStringBuffer, lStringBufferLength);
						    hTextItem.m_Face = pStringBuffer;
                        }
					}
					else
					{
						nFind = szProperty.find(L"size");
						if (nFind != std::wstring::npos)
						{
							long front = szProperty.find(L"\"");

                            if (front != std::wstring::npos)
                            {
                                szProperty = szProperty.substr(front+1,szProperty.size()-front);
                            }

							long back = szProperty.find(L"\"");

                            if (back != std::wstring::npos)
                            {
							    hTextItem.m_FontSize = wcstol(szProperty.substr(0,back).c_str(), NULL, 10);
                            }
						}
					}
				}
			}// end while

			std::wstring endFlag = L"</" + szName + L">";

			front = szBuffer.find(endFlag.c_str());

            if (front != std::wstring::npos)
            {
			    hTextItem.m_wtext = szBuffer.substr(0, front);

			    szBuffer = szBuffer.substr(
				    front+endFlag.size(), 
				    szBuffer.size()-front-endFlag.size());

			    Segment.m_TextItemList.push_back(hTextItem);
			    Segment.m_processlist.push_back(true);

			    if (szBuffer.size() == 0)
			    {
				    bIsEnd = true;
			    }
            }
		}// end font
		// 图片
		else if (wcsicmp(szName.c_str(), L"img")==0)
		{
			//src rect halign valign only 仅考虑这三个属性
			HtmlImageItem hImageItem;

			std::wstring szProperty;

			while (splitHtml.FindNext(szProperty))
			{
				if (szProperty.size()==0) break;
				long nFind = szProperty.find(L"src");

				if (nFind != std::wstring::npos)
				{
					front = szProperty.find(L"\"");

                    if (front != std::wstring::npos)
                    {
					    szProperty = szProperty.substr(front+1, szProperty.size()-front);
                    }


					back = szProperty.find(L"\"");

                    if (back != std::wstring::npos)
                    {
                        hImageItem.m_imagesrc = szProperty.substr(0, back);
                    }
				}
				else
				{
					nFind = szProperty.find(L"rect");
                    if (nFind != std::wstring::npos)
					{
						long front = szProperty.find(L"\"");

                        if (front != std::wstring::npos)
                        {
						    szProperty = szProperty.substr(front+1,szProperty.size()-front);
                        }

						long back = szProperty.find(L"\"");

                        if (back != std::wstring::npos)
                        {
						    hImageItem.m_Rect = ParseToRect(szProperty.substr(0,back).c_str());
                        }
					}
					else
					{
						nFind = szProperty.find(L"halign");
                        if (nFind != std::wstring::npos)
						{
							int front = szProperty.find(L"\"");

                            if (front != std::wstring::npos)
                            {
							    szProperty = szProperty.substr(front+1,szProperty.size()-front);
                            }

							int back = szProperty.find(L"\"");

                            if (back != std::wstring::npos)
                            {
                                hImageItem.m_HAlign = szProperty.substr(0,back).c_str();
                            }
						}
						else
						{
							nFind = szProperty.find(L"valign");
                            if (nFind != std::wstring::npos)
							{
								int front = szProperty.find(L"\"");

                                if (front != std::wstring::npos)
                                {
								    szProperty = szProperty.substr(front+1,szProperty.size()-front);
                                }

								int back = szProperty.find(L"\"");

                                if (back != std::wstring::npos)
                                {
                                    hImageItem.m_VAlign = szProperty.substr(0,back).c_str();
                                }
							}
							else
							{
								nFind = szProperty.find(L"only");
								if (nFind != std::wstring::npos)
								{
									int front = szProperty.find(L"\"");

                                    if (front != std::wstring::npos)
                                    {
									    szProperty = szProperty.substr(front+1,szProperty.size()-front);
                                    }

                                    int back = szProperty.find(L"\"");

                                    if (front != std::wstring::npos)
                                    {
									    if (wcsicmp(L"true", szProperty.substr(0,back).c_str()) == 0)
									    {
										    hImageItem.m_nOnly = 1;
									    }
									    else if (wcsicmp(L"line", szProperty.substr(0,back).c_str()) == 0)
									    {
										    hImageItem.m_nOnly = 2;
									    }
                                    }
								}
								else
								{
									nFind = szProperty.find(L"data");

									if (nFind != std::wstring::npos)
									{
										int front = szProperty.find(L"\"");

                                        if (front != std::wstring::npos)
                                        {
										    szProperty = szProperty.substr(front+1,szProperty.size()-front);
                                        }

										int back = szProperty.find(L"\"");

                                        if (back != std::wstring::npos)
                                        {
										    std::wstring Value = szProperty.substr(0,back);
										    ToString(Value.c_str(), pStringBuffer, lStringBufferLength);
										    hImageItem.m_MarkStr = pStringBuffer;
                                        }
									}
								}
							}
						}
					}
				}
			}// end while

			Segment.m_ImageItemList.push_back(hImageItem);
			Segment.m_processlist.push_back(false);

            if (szBuffer.size() == 0)
            {
                bIsEnd = true;
            }
		}
		// 换行,分段
		else if (wcsicmp(szName.c_str(), L"br") == 0)
		{
			m_Segments.push_back(Segment);

			{
				Segment.m_bInsertTwoSpace = false;
				Segment.m_bCenter = false;
				Segment.m_TextItemList.clear();
				Segment.m_ImageItemList.clear();
                Segment.m_processlist.clear();
			}

			if (bProcessCenter)
			{
				Segment.m_bCenter = true;
			}
		}
		// 文字居中// 这里应该允许嵌font标签,或img标签--------------------
		else if (wcsicmp(szName.c_str(), L"center")==0)
		{
			std::wstring endFlag = L"</" + szName + L">";
			back = szBuffer.find(endFlag);

			if (back != std::wstring::npos)
			{
                if (Segment.m_processlist.size() > 0)
                {
				    m_Segments.push_back(Segment);
					Segment.m_bInsertTwoSpace=false;
					Segment.m_bCenter = false;
					Segment.m_TextItemList.clear();
					Segment.m_ImageItemList.clear();
                    Segment.m_processlist.clear();
                }

				Segment.m_bCenter = true;
				std::wstring szCenter = szBuffer.substr(0,back);
				long lFont,lImg;
				lFont = szCenter.find(L"font");
				lImg  = szCenter.find(L"img");

				if ((lFont != std::wstring::npos) 
					|| (lImg != std::wstring::npos))
				{
					bProcessCenter = true;
					continue;
				}
				else
				{
					HtmlTextItem hTextItem(m_defColor,m_defFont.c_str(),m_defSize);
					hTextItem.m_wtext = szBuffer.substr(0,back);
                    //</center>
                    size_t new_begin = back +  9;
                    size_t new_len = szBuffer.size() - back - 9;

					szBuffer = szBuffer.substr(new_begin, new_len);

					Segment.m_TextItemList.push_back(hTextItem);
					Segment.m_processlist.push_back(true);
					m_Segments.push_back(Segment);

					{
						Segment.m_bInsertTwoSpace=false;
						Segment.m_bCenter = false;
						Segment.m_TextItemList.clear();
						Segment.m_ImageItemList.clear();
                        Segment.m_processlist.clear();
					}
				}
			}
		}
		// 空格标签-------------------------------------------------------
		else if (wcsicmp(szName.c_str(), L"s")==0)
		{
			HtmlTextItem hTextItem(m_defColor,m_defFont.c_str(),m_defSize);
			back = szBuffer.find(L"<");
			
			if (back == std::wstring::npos)
			{
				back = szBuffer.size();
				bIsEnd = true;
			}

			std::wstring szProperty;
			splitHtml.FindNext(szProperty);
			long lFond;
			long spacesize = 0;
			std::wstring szSpaces= L"";
			// 有size属性-----------------------------------2006_10_07----
			lFond = szProperty.find(L"size");

			if (lFond != std::wstring::npos)
			{
				szProperty = szProperty.substr(lFond + 6, 
					szProperty.size() - lFond - 6);
				lFond = szProperty.find(L"\"");

                if (lFond != std::wstring::npos)
                {
				    szProperty = szProperty.substr(0, lFond);
                }

				spacesize = wcstol(szProperty.c_str(), NULL, 10);

				while (spacesize>0)
				{
					spacesize--;
					szSpaces += L" ";
				}
			}
			else//无size属性则默认空两个中文字符----------------------------------
			{
				szSpaces = L"  ";
			}

			hTextItem.m_wtext = szSpaces + szBuffer.substr(0, back);
			szBuffer = szBuffer.substr(back , szBuffer.size()-back+1);
			// 其它默认---------------------------------------------------
			Segment.m_TextItemList.push_back(hTextItem);
			Segment.m_processlist.push_back(true);
			// 仅考虑size这个属性-----------------------------------------
		}
		// 超链接标签<a>
		else if (wcsicmp(szName.c_str(), L"a") == 0)
		{
			HtmlTextItem hTextItem(m_defColor,m_defFont.c_str(),m_defSize);
			hTextItem.m_IsHyperLink = true;
			std::wstring szProperty;

			// 取出配置属性
			while (splitHtml.FindNext(szProperty))
			{
				long nFind  = szProperty.find(L"href");

				if (nFind != std::wstring::npos)
				{
					front = szProperty.find(L"\"");

                    if (front != std::wstring::npos)
                    {
					    szProperty = szProperty.substr(front + 1, 
						    szProperty.size() - front);
                    }

					back = szProperty.find(L"\"");

                    if (back != std::wstring::npos)
                    {
					    std::wstring wcsValue = szProperty.substr(0, back);

					    hTextItem.m_wcsLink = wcsValue;
                    }
				}
				else
				{
					nFind = szProperty.find(L"style");

					if (nFind != std::wstring::npos)
					{
						front = szProperty.find(L"\"");

                        if (front != std::wstring::npos)
                        {
						    szProperty = szProperty.substr(front + 1, 
							    szProperty.size() - front);
                        }

						back = szProperty.find(L"\"");

                        if (back != std::wstring::npos)
                        {
						    std::wstring wcsStyle = szProperty.substr(0, back);

						    ToString(wcsStyle.c_str(), pStringBuffer, 
							    lStringBufferLength);

						    hTextItem.m_szHPStyle = pStringBuffer;
                        }
					}
				}
			}

			// 取出配置文本
			std::wstring endFlag = L"</" + szName + L">";

			front = szBuffer.find(endFlag.c_str());

            if (front != std::wstring::npos)
            {
			    hTextItem.m_wtext = szBuffer.substr(0, front);
			    szBuffer = szBuffer.substr(front + endFlag.size(), 
				    szBuffer.size() - front-endFlag.size());
			    Segment.m_TextItemList.push_back(hTextItem);
			    Segment.m_processlist.push_back(true);
            }
            else
            {
                // 格式错误
                hTextItem.m_IsHyperLink = false;
                hTextItem.m_wtext = hTextItem.m_wcsLink.c_str();
 
                hTextItem.m_wcsLink = L"";
			    Segment.m_TextItemList.push_back(hTextItem);
			    Segment.m_processlist.push_back(true);
                m_Segments.push_back(Segment);
                break;
            }

			if (szBuffer.size() == 0)
			{
				bIsEnd = true;
			}
		}
        else
        {
            if (szBuffer.size() > 0)
            {
                HtmlTextItem hTextItem(m_defColor,m_defFont.c_str(),m_defSize);
                hTextItem.m_IsHyperLink = false;
                hTextItem.m_wtext = szBuffer;
                Segment.m_TextItemList.push_back(hTextItem);
                Segment.m_processlist.push_back(true);
            }

            bIsEnd = true;

        }

		if (bIsEnd)
		{
            if (Segment.m_processlist.size() > 0)
            {
                m_Segments.push_back(Segment);
            }

			break;
		}
	}

	/*if (Segment.m_TextItemList.size() != 0 && Segment.m_ImageItemList.size() != 0)
	{
		m_Segments.push_back(Segment);
	}*/
	return true;
}

bool TinyHtml::IsWhiteSpace( char c )		
{ 
	return (isspace((unsigned char)c) || c == '\n' || c == '\r'); 
}

const char* TinyHtml::SkipWhiteSpace(char* p)
{
	if (!p || !*p)
	{
		return 0;
	}

	while (*p && IsWhiteSpace(*p ) || *p == '\n' || *p =='\r')
	{
		++p;
	}

	return p;
}

bool TinyHtml::RestoreToHtml(HtmlTextItem& item, 
                             std::wstring& buffer, bool bReturnFontFormat)
{
    HtmlTextItem* pChar = &item;

    if (pChar->m_IsHyperLink)
    {
        // 超链接文本和类型是必配的
        buffer += L"<a href=\"" + pChar->m_wcsLink + L"\" style=\"" 
			+ StringUtil::StringAsWideStr(pChar->m_szHPStyle.c_str()) 
			+  L"\">" + pChar->m_wtext + L"</a>";
    }
    else// if (pChar->m_Face != m_defFont || pChar->m_Color != m_defColor)
    {
        if (bReturnFontFormat)
        {
            buffer += L"<font ";

            //if (pChar->m_Face != m_defFont)
            {
                buffer += L"face=\"" + StringUtil::StringAsWideStr(
				    pChar->m_Face.c_str()) + L"\" ";
            }

            //if (pChar->m_Color != m_defColor)
            {
                //wchar_t wcsbuffer[65];                
			    //_itow_s( pChar->m_Color& 0x00ffffff, wcsbuffer, 16 ); // C4996
				std::wstringstream wcsbuffer;
				wcsbuffer<<std::hex<<(pChar->m_Color& 0x00ffffff);
                
			    std::wstring wcsColor = wcsbuffer.str();

                while (wcsColor.size() < 6)
                {
                    wcsColor = L"0" + wcsColor;
                }

                buffer = buffer + L"color=\"#" + wcsColor + L"\" ";
            }

            buffer += L">" + pChar->m_wtext + L"</font>";
        }
        else
        {
            buffer += pChar->m_wtext;
        }
    }
    /*else
    {
        buffer += pChar->m_wtext;
    }*/

    return buffer.size() != 0;
}

bool TinyHtml::RestoreToHtml(HtmlImageItem& item, std::wstring& buffer)
{
    buffer = L"<img src=\"" + item.m_imagesrc + L"\" ";

    if (wcsicmp(item.m_HAlign.c_str(), L"") != 0)
    {
        buffer += L" halign=\"" + item.m_HAlign + L"\" ";
    }

    if (wcsicmp(item.m_VAlign.c_str(), L"top") != 0)
    {
        buffer += L" valign=\"" + item.m_VAlign + L"\" ";
    }

    if (item.m_nOnly != 0)
    {
        std::wstring only;

        if (item.m_nOnly == 1)
        {
            only = L"true";
        }
        else
        {
            only = L"line";
        }

        buffer = buffer + L" only=\"" + only + L"\"";
    }

    if (item.m_MarkStr.size() > 0)
    {
        buffer += L"data=\"" + StringUtil::StringAsWideStr(
			item.m_MarkStr.c_str()) + L"\"";
    }

    buffer += L"/>";

    return buffer.size() != 0;
}

