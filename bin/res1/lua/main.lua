-----------------------------------------------------
--文件名：	lua\main.lua
--内容：	
--说明：	
--日期：	2012.3.21
--作者：	
-----------------------------------------------------

--
function init()
	nx_log("[Game::Init]  begin")
	--
	local game_client_lua = nx_create("LuaGameClient")
	nx_set_value("game_client_lua", game_client_lua)
		
    --
    local gui = nx_value("gui")
    gui.skin_path = "skin\\"
    
end

--
function shut()
	nx_log("[Game::shut]   end")
end