;激活记事本112
WinActivate("112.txt - 记事本")

;以下为通过菜单打开退出选项
Send("fffffff")
Sleep(800)
;Send("!f")
ControlSend("112.txt - 记事本", "", "", "!f");
Sleep(800)
;Send("x")
ControlSend("112.txt - 记事本", "", "", "x");//ControlSend比Send功能更加强大，可以控制朝哪个窗口发送

WinWait("112.txt - 记事本", "")
ControlClick("记事本", "", "[CLASS:Button; Instance:1;]");窗口的标题；窗口的内容；窗口的按钮
;ControlClick("记事本", "", "[CLASS:Button; Text:取消; Instance:1;]");