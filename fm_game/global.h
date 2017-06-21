//常用全局变量

#pragma once

class IWorld;
class IRender;
class IScene;
class ICore;
class Designer;
class Gui;
class IGameSock;
class IGameSender;
class ITerrain;
class IGameClient;
class GameControl;

extern ICore* g_core;
extern IWorld* g_world;
extern IRender* g_render;
extern IScene* g_scene;
extern Designer* g_designeder;
extern Gui* g_gui;
extern IGameSock* g_gamesock;
extern IGameSender* g_gamesender;
extern IGameClient* g_client;
extern ITerrain* g_terrain;
extern GameControl* g_gamecontrol;

extern int g_mouse_pos_x;
extern int g_mouse_pos_y;

extern bool testsave;