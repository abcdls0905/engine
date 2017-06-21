#include "global.h"

ICore* g_core = 0;
IWorld* g_world = 0;
IRender* g_render = 0;
IScene* g_scene = 0;
Designer* g_designeder = 0;
Gui* g_gui = 0;
IGameSock* g_gamesock = 0;
IGameSender* g_gamesender = 0;
IGameClient* g_client = 0;
ITerrain* g_terrain = 0;
GameControl* g_gamecontrol = 0;

int g_mouse_pos_x = 0;	//鼠标位置X
int g_mouse_pos_y = 0;	//鼠标位置Y
bool testsave = false;