//--------------------------------------------------------------------
// �ļ���:		rapid_xml_helper.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��6��26��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#pragma once
#include "../extern/rapidxml/rapidxml.hpp"

// rapidxml��������

// �����ļ�
bool xml_save_file(rapidxml::xml_document<>* doc, const char* file_path);

// ��ӽڵ�
rapidxml::xml_node<>* xml_add_node(rapidxml::xml_document<>* doc, 
								   rapidxml::xml_node<>* node = NULL, 
								   const char* name = NULL, 
								   rapidxml::node_type type = rapidxml::node_element);

// �������
void xml_add_attr(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* node, 
				  const char* name, const char* value);
void xml_add_attr(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* node,
				  const char* name, int value);
void xml_add_attr(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* node,
				  const char* name, float value);
void xml_add_attr(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* node,
				  const char* name, bool value);
