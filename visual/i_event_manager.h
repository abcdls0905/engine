#pragma once

#include "fastdelegate.h"

typedef fastdelegate::FastDelegate2<const char*, const IVarList&, void> EventDelegate;

//连接一事件和一个代理
extern void event_connect(const PERSISTID& sender, const char* eventname, EventDelegate recv_delegate);

//播放事件
extern int event_send(const PERSISTID& sender, const char* eventname, const IVarList& arg);

//解除sender的所有链接
extern void event_disconnect(const PERSISTID& sender);

//解除指定事件的代理
extern void event_disconnect(const PERSISTID& sender, const char* eventname, EventDelegate recv_callback);

//禁用/开启 sender的所有消息链接
void event_enableconnect(const PERSISTID& sender, bool enable);

//禁用/开启 sender的某个消息链接
void event_enableconnect(const PERSISTID& sender, const char* eventname, bool enable);


//连接任何事件和一个代理
extern void event_connect_globals(EventDelegate recv_callback);

//解除任何事件和一个代理
extern void disconnect_globals(EventDelegate recv_callback);