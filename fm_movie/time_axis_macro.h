
#ifndef __TIME_AXIS_MACRO_H__
#define __TIME_AXIS_MACRO_H__

// xml文件中的节点名
#define XML_NODE_OBJECT "Object"
#define XML_NODE_CONTROLLER "Controller"
#define XML_NODE_KEYFRAME "KeyFrame"

// xml文件中的属性名
#define XML_ATTR_OBJECT_NAME "object_name"

// 对象的属性, 绕中心点做Y轴旋转
#define XML_ATTR_CORRECT_Y "correct_y"

// 对象的属性, 是否为相对坐标的路径
#define XML_ATTR_LOCAL_POS "local_position" // 是否为相对坐标
#define XML_ATTR_ORIGIN_X "origin_x" // 相对坐标系的坐标
#define XML_ATTR_ORIGIN_Y "origin_y"
#define XML_ATTR_ORIGIN_Z "origin_z"

// 对象的属性, 是否为有旋转的路径
#define XML_ATTR_ROTATION   "local_rotation"	 // 是否有旋转
#define XML_ATTR_ROTATION_X "local_rotation_x" // 相对坐标系的坐标
#define XML_ATTR_ROTATION_Y "local_rotation_y"
#define XML_ATTR_ROTATION_Z "local_rotation_z"

// 对象的属性(额外)
#define XML_ATTR_EXT_PROP "ext_prop" // 参数字符串

//#define XML_ATTR_TYPE "type"
#define XML_ATTR_TYPE_ID "type_id"
#define XML_ATTR_USAGE_MODE "usage_mode"
#define XML_ATTR_TIME "time"
#define XML_ATTR_VALUE "value"
#define XML_ATTR_PREV_CP_TIME "prev_cp_time"
#define XML_ATTR_PREV_CP_VALUE "prev_cp_value"
#define XML_ATTR_NEXT_CP_TIME "next_cp_time"
#define XML_ATTR_NEXT_CP_VALUE "next_cp_value"
#define XML_ATTR_PARAM "param"
#define XML_ATTR_TIME_LENGTH "time_length"
#define XML_ATTR_SPEED "speed"

#define PROJECT_VERSION "1.0"

#endif // __TIME_AXIS_MACRO_H__
