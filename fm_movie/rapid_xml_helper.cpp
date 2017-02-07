//--------------------------------------------------------------------
// 文件名:		rapid_xml_helper.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年6月26日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "rapid_xml_helper.h"

#include "../public/inlines.h"
#include "../extern/rapidxml/rapidxml_print.hpp"
#include "../utils/stream_writer.h"
#include "../public/portable.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

using namespace rapidxml;

xml_node<>* xml_add_node(xml_document<>* doc, xml_node<>* node, 
						 const char* name, node_type type)
{
	assert(doc != NULL);

	char* node_name = NULL;
	if (name != NULL)
	{
		node_name = doc->allocate_string(name);
	}
	
	xml_node<>* new_node = doc->allocate_node(type, node_name);

	if (node)
	{
		node->append_node(new_node);
	}

	return new_node;
}

void xml_add_attr(xml_document<>* doc, xml_node<>* node, 
				  const char* name, const char* value)
{
	assert(doc != NULL);
	assert(node != NULL);
	assert(name != NULL);
	assert(value != NULL);

	char* attr_name = doc->allocate_string(name);
	char* attr_value = doc->allocate_string(value);

	xml_attribute<>* new_attr = doc->allocate_attribute(attr_name, attr_value);

	node->append_attribute(new_attr);
}

void xml_add_attr(xml_document<>* doc, xml_node<>* node, 
				  const char* name, int value)
{
	char buf[32];
	SafeSprintf(buf, sizeof(buf), "%d", value);
	xml_add_attr(doc, node, name, buf);
}

void xml_add_attr(xml_document<>* doc, xml_node<>* node, 
				  const char* name, float value)
{
	char buf[32];
	SafeSprintf(buf, sizeof(buf), "%f", value);
	xml_add_attr(doc, node, name, buf);
}

bool xml_save_file(rapidxml::xml_document<>* doc, const char* file_path)
{
	assert(doc != NULL);
	assert(file_path != NULL);

	TStreamWriter<char, 256> writer;
	TStreamWriter<char, 256>::iterator it = writer.Begin();

	print(it, *doc, 0);

	char* buf = writer.GetData();
	size_t length = writer.GetCount();
	FILE* fp = Port_FileOpen(file_path, "wb");

	if (NULL == fp)
	{
		assert(0);
		return false;
	}

	if (fwrite(buf, sizeof(char), length, fp) != length)
	{
		assert(0);
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}
