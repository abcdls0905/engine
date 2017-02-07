-----------------------------------------------------
--文件名：	lua\form_stage_main\form_cool_summer_introduce.lua
--内容：	  清凉一夏_介绍
--说明：	
--日期：	2015.8.25
--作者：	
-----------------------------------------------------




--
function main_form_init(form)
	--nx_msgbox("init:" .. nx_string(form.Name))
	--form.Fixed = false	--使窗口可以移动	
end

--
function main_form_open(form)
	nx_log("LUALOG" .. "form_cool_summer_exchange open")
	--nx_msgbox("main_form_open")
	--最大化
	nx_function("ext_gui_maximize_form", form)
	
	local group_main = form.group_main
	--居中
	nx_function("ext_gui_center", group_main)
	
	--
	--local xml_render = nx_create("FastReadItemXml")
	--local file_name = nx_resource_path() .. "share/rule/festival/summer_exchange_rule.xml"
	--xml_render:SetFileName(file_name)
	--xml_render:LoadFromFile()
	--local need_item =  xml_render:ReadString(0, "NeedItems", "ss")
	--nx_msgbox(need_item)
	
	--
	local gbox_need_item = form.groupbox_need_item
	gbox_need_item.Visible = false
	
	local data_query_manager = nx_value("data_query_manager")
	local max_row = data_query_manager:GetMaxRows(412)
	if max_row>0 then
		local needItems = data_query_manager:Query(412, nx_string(1), "NeedItems")
		--nx_msgbox(needItems)
		
		CreateExchangeRule(1, form)
		AssignRuleID(1, form)
	end	
	
	--注册网络消息回调
	local CustomMessageHandler = nx_value("CustomMessageHandler")
	CustomMessageHandler:RegisterCustomMsgHandler(nx_int(32611), nx_current(), "on_msg_summer")
end

--
function main_form_close(form)
	--nx_msgbox("main_form_close")
	
		--注册网络消息回调
	local CustomMessageHandler = nx_value("CustomMessageHandler")
	CustomMessageHandler:UnRegisterCustomMsgHandler(nx_int(32611), 2)
end


--关闭
function on_click_btn_close(self)
	--nx_msgbox("btn_click_close")
	local form = self.ParentForm
	if  nx_is_valid(form) then
		form:Close()
	end
end

--左
function on_click_btn_left(self)
	--nx_msgbox("on_click_btn_left")
	local rule_id = self.rule_id
	local form = nx_value("form_stage_main\\form_cool_summer\\form_summer_exchange")
	
	--nx_msgbox(nx_string(rule_id) .. "   " .. nx_string(form))
	
	CreateExchangeRule(rule_id, form)
	AssignRuleID(rule_id, form)
end 

--右
function on_click_btn_right(self)
	--nx_msgbox("on_click_btn_right")
	local rule_id = self.rule_id
	local form = nx_value("form_stage_main\\form_cool_summer\\form_summer_exchange")
	
	--nx_msgbox(nx_string(rule_id) .. "   " .. nx_string(form))
	
	CreateExchangeRule(rule_id, form)
	AssignRuleID(rule_id, form)
end 

--单击需要兑换的物品图片
function  on_click_lbl_item_photo_ex(self)
	--nx_msgbox("on_click_lbl_item_photo_ex")
	local CapitalQuery = nx_value("CapitalQuery")
	
	local configID = self.ConfigID
	if configID == "" then
		return
	end
	
	--
	if CapitalQuery:IsCapitalDefined(nx_string(configID)) then
		return
	end
	
	--
	local formItemTips = nx_lookup("FormItemTips")
	if not nx_is_valid(formItemTips) then
		formItemTips = nx_create("FormItemTips")
	end
	nx_set_value("FormItemTips", formItemTips)
	
	--
	formItemTips:ShowGoodsTipEx(configID, nx_boolean("false") )
end 

--单机兑换按钮
function  on_click_btn_exchange(self)
	--nx_msgbox("on_click_btn_exchange")
	local rule_id = self.rule_id
	
	local game_client_lua = nx_value("game_client_lua")
	game_client_lua:Custom(nx_boolean("false"), nx_boolean("false"), nx_int(159), nx_int(102), nx_int(rule_id))
end

--网络消息回调
function  on_msg_summer(...)
	--nx_msgbox("on_msg_summer")
	local gui = nx_value("gui")
	local TextManager = gui:GetTextManagerID()
	local GuiManager = nx_value("GuiManager")
	
	
	local form = nx_value("form_stage_main\\form_cool_summer\\form_summer_exchange")
	
	if not form.Visible then
		return
	end
	
	
	--
	local count = table.getn(arg)
	--nx_msgbox(nx_string(count))
	
	if  nx_int(count) < nx_int(3) then
		return
	end
	
	
	--
	local msg_type =  arg[3]
	
	if nx_int(msg_type) == nx_int(102) then
		local text_title = TextManager:GetFormatText("ui_summer_exchange_07")
		
		local content = ""
		for i = 4, count do
				if content ~= "" then
					content = content .. "|" .. nx_string(arg[i])
				else
					content = nx_string(arg[i])
				end
		end
		--nx_msgbox(nx_string(content))
		
		--
		local FormGiftPackage = GuiManager:GetFormLogicByName("FormGiftPackage")
		GuiManager:SendMsgToLogicForm(FormGiftPackage, "show")
		GuiManager:SendMsgToLogicFormArgs(FormGiftPackage, "SetTitle", nx_widestr(text_title))
		GuiManager:SendMsgToLogicFormArgs(FormGiftPackage, "ShowItemsInfo", nx_string(content))
		
		--
		RefreshForm(form)
	end
	
end

--
function CreateExchangeRule(id, form)
	CreateNeedItems(id, form)
	
	CreateExchangeItem(id, form)
end



--需要物品
function CreateNeedItems(id, form)
	local ItemsQuery = nx_value("ItemQuery")
	local game_visual = nx_value("game_visual")
	local CapitalQuery = nx_value("CapitalQuery")
	local data_query_manager = nx_value("data_query_manager")
	local ImageQuery = nx_value("ImageQuery")
	local gui = nx_value("gui")
	local text = gui:GetTextManagerID()
	
	local player = game_visual:GetPlayer()
	local artpackIndex = player.artpack_index
	--nx_msgbox("artpack_index:" .. nx_string(artpackIndex))
	
	--
	local groupbox_items = form.groupbox_item
	groupbox_items:DeleteAll()
	
	--需要物品
	local needItems = data_query_manager:Query(412, nx_string(id), "NeedItems")
	local valueList = nx_function("ext_split_string", needItems, ",")
	local count = table.getn(valueList)
	for i = 1, count do
		local needitem = valueList[i]
		local valueList1 = nx_function("ext_split_string", needitem, ":")
		local itemid = nx_string(valueList1[1])
		local itemnum = nx_int(valueList1[2])
		--nx_msgbox("itemid:" .. nx_string(itemid) .. "   itemnum:" .. nx_string(itemnum))
		
		if itemid ~= "" and itemnum > nx_int(0) then
			local photo = ItemsQuery:GetItemPhoto(itemid, artpackIndex, 0)
			local name = text:GetFormatText(itemid)
			local color_level = ItemsQuery:GetItemColorLevel(itemid, 0)
		
			AddItem(form, groupbox_items, itemid, photo, name, itemnum, i-1, color_level)
		end
	end
	
	--需要货币
	local needCapitals = data_query_manager:Query(412, nx_string(id), "NeedCapitals")
	valueList = nx_function("ext_split_string", needCapitals, ",")
	count = table.getn(valueList)
	local index = nx_int(count)
	--nx_msgbox(nx_string(index))
	
	for i = 1, count do
		local needitem = valueList[i]
		local valueList1 = nx_function("ext_split_string", needitem, ":")
		local itemid = nx_string(valueList1[1])
		local itemnum = nx_int(valueList1[2])
		--nx_msgbox("itemid:" .. nx_string(itemid) .. "   itemnum:" .. nx_string(itemnum))
		
		if itemid ~= "" and itemnum > nx_int(0) then
			local photo = ImageQuery:QueryFile(itemid)
			local name = text:GetFormatText(itemid)
			local color_level = ItemsQuery:GetItemColorLevel(itemid, 0)
		
			AddItem(form, groupbox_items, itemid, photo, name, itemnum, index+1, -1)
		end
	end
	
	--
	local groupbox_exchange_event = form.groupbox_exchange_event
	groupbox_exchange_event.sum_item_count = nx_int(index+count)
	--nx_msgbox(nx_string(groupbox_exchange_event.sum_item_count))
end

--需要交换的物品
function CreateExchangeItem(id, form)
	local ItemsQuery = nx_value("ItemQuery")
	local game_visual = nx_value("game_visual")
	local CapitalQuery = nx_value("CapitalQuery")
	local data_query_manager = nx_value("data_query_manager")
	local ImageQuery = nx_value("ImageQuery")
	local gui = nx_value("gui")
	local game_client_lua = nx_value("game_client_lua")
	local server_player = game_client_lua:GetPlayer()
	local text = gui:GetTextManagerID()
	
	local client_player = game_visual:GetPlayer()
	local artpackIndex = client_player.artpack_index
	
	local enable = nx_boolean("true") 
	--需要物品
	local day_count_max = data_query_manager:Query(412, nx_string(id), "DayCount")
	local limit_count_max = data_query_manager:Query(412, nx_string(id), "LimitCount")
	
	local needItems = data_query_manager:Query(412, nx_string(id), "ExchangeItem")
	local valueList = nx_function("ext_split_string", needItems, ",")
	local count = table.getn(valueList)
	
	--
	for i = 1, count do
		local needitem = valueList[i]
		local valueList1 = nx_function("ext_split_string", needitem, ":")
		local itemid = nx_string(valueList1[1])
		local itemnum = nx_int(valueList1[2])
		--nx_msgbox("itemid:" .. nx_string(itemid) .. "   itemnum:" .. nx_string(itemnum))
		
		if itemid ~= "" and itemnum > nx_int(0) then
			local photo = ItemsQuery:GetItemPhoto(itemid, artpackIndex, 0)
			local name = text:GetFormatText(itemid)
			
			--照片
			local lbl_item_photo = form.lbl_item_photo_ex
			lbl_item_photo.BackImage = photo
			lbl_item_photo.ConfigID = itemid
		
			--遮罩
			local lbl_item_cover = form.lbl_item_cover_ex
			local color_level = ItemsQuery:GetItemColorLevel(itemid, 0)
			set_label_color_level(lbl_item_cover, color_level)
			
			--数量
			local lbl_item_count = form.lbl_item_count2_ex
			if nx_int(itemnum) > nx_int(1) then
				lbl_item_count.Text = nx_widestr(itemnum)
			else
				lbl_item_count.Text = nx_widestr("")
			end
			
			--次数
			local lbl_item_amount = form.lbl_item_count_ex
			local text_show=nx_widestr("")
			local row = game_client_lua:FindRecordRow(server_player, "summer_exchange_rec", nx_int(2), nx_int(id))
			
			--
			if nx_int(row) >= nx_int(0) then
				local is_limit = game_client_lua:QueryRecordInt(server_player, "summer_exchange_rec", nx_int(row), nx_int(5))
				
				--活动周期内限制兑换数量
				if is_limit == 1 then
					local limit_count = game_client_lua:QueryRecordInt(server_player, "summer_exchange_rec", nx_int(row), nx_int(4));
					
					if limit_count >= limit_count_max then
						text_show = text:GetFormatText("ui_summer_exchange_04")
						enable = nx_boolean("false") 
					else
						text_show = text:GetFormatText("ui_summer_exchange_03", nx_int(limit_count), nx_int(limit_count_max))
					end
				--每日兑换
				else
					local day_count = game_client_lua:QueryRecordInt(server_player, "summer_exchange_rec", nx_int(row), nx_int(3));

					if day_count >= day_count_max then
						text_show = text:GetFormatText("ui_summer_exchange_05");
						enable = nx_boolean("false") 
					else
						text_show = text:GetFormatText("ui_summer_exchange_06", nx_int(day_count), nx_int(day_count_max));
					end
				end
				lbl_item_amount.Text = nx_widestr(text_show)
			else
				text_show = text:GetFormatText("ui_summer_exchange_04")
				enable = nx_boolean("false") 
				lbl_item_amount.Text = nx_widestr(text_show)
			end
			
			lbl_item_amount.rule_id = id
			
			
			--名称
			local lbl_item_name = form.lbl_item_name_ex
			lbl_item_name.Text = nx_widestr(name)
		end
	end
	
	--
	local btn_exchange = form.btn_exchange
	btn_exchange.rule_id = id
	btn_exchange.Enabled = enable
end

--增加物品
function AddItem(form, control, configid, photo, name, amount, index, level)
	local capitalQuery = nx_value("CapitalQuery")
	local ViewportQuery = nx_value("ViewportQuery")
	
	local groupbox_need_item = form.groupbox_need_item
	local name_add = "_" .. nx_string(index)
	
	local gbox_need_item_temp = nx_function("gui_clone_recursion", form, control, groupbox_need_item, name_add)
	
	--每行四个道具
	local num_per_row = 4
	local mod_index = nx_int(index % num_per_row)
	local index_row = nx_int(index / num_per_row)
	local mod_width = nx_int(control.Width/num_per_row)
	--nx_msgbox(nx_string(mod_index) .. "-" .. nx_string(index_row) .. "-" .. nx_string(mod_width))
	
	gbox_need_item_temp.Left = nx_int(mod_width)*nx_int(mod_index)
	gbox_need_item_temp.Top = gbox_need_item_temp.Height*nx_int(index_row)
	gbox_need_item_temp.Visible=true
	--nx_msgbox(nx_string(gbox_need_item_temp.Left) .. "-" .. nx_string(gbox_need_item_temp.Top))
	
	--显示photo
	local photo_ctrol_name = "lbl_item_photo" .. name_add;
	local photo_ctrol = nx_function("get_control", form, photo_ctrol_name)
	photo_ctrol.BackImage = photo
	--nx_msgbox(photo .. "___" .. nx_string(photo_ctrol))
	
	nx_bind_script(photo_ctrol, nx_current())
	nx_callback(photo_ctrol, "on_click", "on_click_photo_ctrl")
	photo_ctrol.ConfigID = configid
	
	--显示遮罩
	local cover_ctrol_name = "lbl_item_cover" .. name_add
	local cover_ctrol = nx_function("get_control", form, cover_ctrol_name)
	set_label_color_level(cover_ctrol, level)
	
	--显示数量
	local amount_ctrol_name = "lbl_item_count" .. name_add
	local amount_ctrol = nx_function("get_control", form, amount_ctrol_name)
	local cur_amount = 0
	
	if capitalQuery:IsCapitalDefined(configid) then
		local cap_type = capitalQuery:QueryCapitalType(configid)
		cur_amount = capitalQuery:QueryPlayerCapital(cap_type)
	else
		cur_amount = ViewportQuery:GetItemCountEX(nx_int(2), configid)
	end
	local text_contont = nx_widestr(cur_amount) .. nx_widestr("/") .. nx_widestr(amount)
	amount_ctrol.Text = nx_widestr(text_contont)
	
	--显示名称
	local itemname_ctrol_name = "lbl_item_name" .. name_add
	local itemnam_ctrol = nx_function("get_control", form, itemname_ctrol_name)
	itemnam_ctrol.Text = nx_widestr(name)
end

--单机需要兑换的物品
function on_click_photo_ctrl(control)
	--nx_msgbox("on_click_photo_ctrl:" .. nx_string(control))
	local capitalQuery = nx_value("CapitalQuery")
	
	--
	local config_id = control.ConfigID
	if config_id == "" then
		return
	end
	
	--
	if capitalQuery:IsCapitalDefined(config_id) then
		return
	end
	
	--
	local formItemTips = nx_lookup("FormItemTips")
	if not nx_is_valid(formItemTips) then
		formItemTips = nx_create("FormItemTips")
	end
	nx_set_value("FormItemTips", formItemTips)
	
	--
	formItemTips:ShowGoodsTipEx(config_id, nx_boolean("false") )
end

--设置空间遮罩
function set_label_color_level(cover_ctrol, level)
	nx_function("SetItemColorLevelLabel", cover_ctrol, nx_int(level), nx_int(0))
end


--
function AssignRuleID(id, form)
	AssignLeftRuleID(id, form)
	AssignRightRuleID(id, form)
end

--
function AssignLeftRuleID(id, form)
		local btn_left = form.btn_left
		
		local data_query_manager = nx_value("data_query_manager")
		if not data_query_manager:FindConfigID(412, nx_string(id)) then
			btn_left.rule_id = nx_int(id)
			return
		end
		
		--
		local index = nx_int(id) - nx_int(1)
		if nx_int(index) > nx_int(0) and data_query_manager:FindConfigID(412, nx_string(index)) then
			btn_left.rule_id = nx_int(index)
			return
		end
		
		btn_left.rule_id = nx_int(id)
end

--
function AssignRightRuleID(id, form)
		local btn_right = form.btn_right
		
		local data_query_manager = nx_value("data_query_manager")
		if not data_query_manager:FindConfigID(412, nx_string(id)) then
			btn_right.rule_id = nx_int(id)
			return
		end
		
		--
		local index = nx_int(id) + nx_int(1)
		if nx_int(index) > nx_int(0) and data_query_manager:FindConfigID(412, nx_string(index)) then
			btn_right.rule_id = nx_int(index)
			return
		end
		
		btn_right.rule_id = nx_int(id)
end


--
function RefreshForm(form)
	RefreshNeedItems(form)
	
	RefreshExchangeItem(form)
end


--刷新所需道具数量
function RefreshNeedItems(form)
	local capitalQuery = nx_value("CapitalQuery")
	local ViewportQuery = nx_value("ViewportQuery")
	local gbox_event = form.groupbox_exchange_event
	
	local sum_item_count = gbox_event.sum_item_count
	local lblAmount = "lbl_item_count"
	--nx_msgbox(nx_string(sum_item_count))
	
	for  i = 1, sum_item_count do
		local nameAdd = lblAmount .. "-" .. nx_string(i)
		local pLblAmount = nx_function("get_control", form, nameAdd)
		
		local configId = pLblAmount.ConfigID
		
		if configId ~= "" then
			local amount = pLblAmount.Amount
			local cur_amount = nx_int(0)
			
			if capitalQuery:IsCapitalDefined(configId) then
				local nCapitalType = capitalQuery:QueryCapitalType(configId)
				cur_amount = capitalQuery:QueryPlayerCapital(nCapitalType)
			else
				cur_amount = ViewportQuery:GetItemCount(nx_int(2), configId)
			end
			
			local text = nx_widestr(cur_amount) .. nx_widestr("/") .. nx_widestr(amount)
			pLblAmount.Text = nx_widestr(text)
		end
	end
end

--刷新兑换道具数量
function RefreshExchangeItem(form)
	local ItemsQuery = nx_value("ItemQuery")
	local game_visual = nx_value("game_visual")
	local CapitalQuery = nx_value("CapitalQuery")
	local data_query_manager = nx_value("data_query_manager")
	local ImageQuery = nx_value("ImageQuery")
	local gui = nx_value("gui")
	local game_client_lua = nx_value("game_client_lua")
	local server_player = game_client_lua:GetPlayer()
	local text = gui:GetTextManagerID()
	
	local client_player = game_visual:GetPlayer()
	
	
	--
	local pLblAmount = form.lbl_item_count_ex
	local rule_id = pLblAmount.rule_id
	
	if not data_query_manager:FindConfigID(412, nx_string(rule_id)) then
		return
	end
	
	local limit_count_max = data_query_manager:Query(412, nx_string(id), "LimitCount")
	local day_count_max = data_query_manager:Query(412, nx_string(id), "DayCount")
	local enable = nx_boolean("true") 
		
	--
	local row = game_client_lua:FindRecordRow(server_player, "summer_exchange_rec", nx_int(2), nx_int(rule_id))
			
			local text_show = ""
			--
			if nx_int(row) >= nx_int(0) then
				local is_limit = game_client_lua:QueryRecordInt(server_player, "summer_exchange_rec", nx_int(row), nx_int(5))
				
				--活动周期内限制兑换数量
				if is_limit == 1 then
					local limit_count = game_client_lua:QueryRecordInt(server_player, "summer_exchange_rec", nx_int(row), nx_int(4));
					
					if limit_count >= limit_count_max then
						text_show = text:GetFormatText("ui_summer_exchange_04")
						enable = nx_boolean("false") 
					else
						text_show = text:GetFormatText("ui_summer_exchange_03", nx_int(limit_count), nx_int(limit_count_max))
					end
				--每日兑换
				else
					local day_count = game_client_lua:QueryRecordInt(server_player, "summer_exchange_rec", nx_int(row), nx_int(3));

					if day_count >= day_count_max then
						text_show = text:GetFormatText("ui_summer_exchange_05");
						enable = nx_boolean("false") 
					else
						text_show = text:GetFormatText("ui_summer_exchange_06", nx_int(day_count), nx_int(day_count_max));
					end
				end
				pLblAmount.Text = nx_widestr(text_show)
			else
				text_show = text:GetFormatText("ui_summer_exchange_04")
				enable = nx_boolean("false") 
				pLblAmount.Text = nx_widestr(text_show)
			end
			
		--
		local btn_exchange = form.btn_exchange
		btn_exchange.Enabled = enable
end