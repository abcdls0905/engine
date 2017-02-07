
#pragma once

class IWorld;
class IRender;
class IScene;
class ICore;
class Designer;
class Gui;
class ISoundSystem;
class ITerrain;


extern ICore* g_core;
extern IWorld* g_world;
extern IRender* g_render;
extern IScene* g_scene;
extern Designer* g_designeder;
extern Gui* g_gui;
extern ITerrain* g_terrain;
extern ISoundSystem* g_soundsys;

extern int g_mouse_pos_x;
extern int g_mouse_pos_y;
