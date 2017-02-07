
#pragma once

#include <string>

#include "define/logdefine.h"
#include "../../extern/tinyxml/tinyxml.h"

bool TraceLog(LogLevelEnum level, const char* format, ...);

bool load_xml_doc(TiXmlDocument& doc, const char* file_name);

