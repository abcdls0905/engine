--标准确认对话框脚本
--通用对话框管理

--下面的函数用于那些会同时和其它通用对话框同时出现的情况,
--       一般通用对话框是模式框只会同时出现一个，但是有些异常情况如下，就会同时弹出，导致操作错误
--1.断开网络连接
--2.退出游戏

require("util_gui")



--窗口初始化
function main_form_init(self)
	--使窗口可以移动
	self.Fixed = false

	return 1
end

--窗口打开
function main_form_open(self)
	local gui = nx_value("gui")

	--对话框居中
	self.Left = (gui.Width - self.Width) / 2
	self.Top = (gui.Height - self.Height) / 2
 
	return 1
end

--窗口关闭
function on_main_form_close(self)
	--删除窗口对象
 
	self.mltbox_info:Clear()
	nx_destroy(self)
end


--点击取消按钮
function ok_btn_click(self)
	local form = self.ParentForm
	if nx_is_valid(form) then
		form:Close()
	end
	
	--产生取消异步事件
	nx_gen_event(form, "confirm_return", "ok")

	return 1
end

--点击取消按钮
function cancel_btn_click(self)
	local form = self.ParentForm
	
	if nx_is_valid(form) then
		form:Close()
	end
	
	--产生取消异步事件
	nx_gen_event(form, "confirm_return", "cancel")

	return 1
end


 
--text: 需要显示的信息
function show_common_text(dialog, text)
	text = nx_widestr(text)
	dialog.mltbox_info.Visible = true		
	dialog.mltbox_info:Clear()
	dialog.mltbox_info:AddHtmlText(text, -1)
		
	--文本过长的话 要重新排版
	local mltbox = dialog.mltbox_info
	local height = mltbox:GetContentHeight()
		
	if height > mltbox.Height then
		mltbox.Height  = height + 2 
	end
		 
end 


