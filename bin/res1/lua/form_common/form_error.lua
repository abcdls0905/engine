--错误提示对话框脚本

--窗口初始化
function main_form_init(self)
	--使窗口可以移动
	self.Fixed = false
	return 1
end

--窗口打开
function main_form_open(self)

	local gui = nx_value("gui")
	nx_set_value("form_common\\form_error", self)
	
	--对话框居中
	self.Left = (gui.Width - self.Width) / 2
	self.Top = (gui.Height - self.Height) / 2
	
	--输入回车时缺省执行确定按钮
	self.Default = self.btn_ok
	return 1
end

--点击确认按钮
function on_btn_ok_click(self)
	local form = self.Parent
	
	--关闭窗口
	form:Close()
	
	--产生结果事件
	nx_gen_event(form, "error_return", "ok")
	
	--删除窗口对象
	nx_destroy(form)
	
	return 1
end


function set_error_info(info)

	local form = nx_value("form_common\\form_error")
	if not nx_is_valid(form) then
		return
	end
	
	form.lbl_1.Text = info
end






















