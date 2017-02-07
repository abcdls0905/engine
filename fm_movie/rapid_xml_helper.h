//--------------------------------------------------------------------
// 文件名:		rapid_xml_helper.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年6月26日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#pragma once
#include "../extern/rapidxml/rapidxml.hpp"

// rapidxml辅助函数

// 保存文件
bool xml_save_file(rapidxml::xml_document<>* doc, const char* file_path);

// 添加节点
rapidxml::xml_node<>* xml_add_node(rapidxml::xml_document<>* doc, 
								   rapidxml::xml_node<>* node = NULL, 
								   const char* name = NULL, 
								   rapidxml::node_type type = rapidxml::node_element);

// 添加属性
void xml_add_attr(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* node, 
				  const char* name, const char* value);
void xml_add_attr(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* node,
				  const char* name, int value);
void xml_add_attr(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* node,
				  const char* name, float value);
void xml_add_attr(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* node,
				  const char* name, bool value);
