;������±�112
WinActivate("112.txt - ���±�")

;����Ϊͨ���˵����˳�ѡ��
Send("fffffff")
Sleep(800)
;Send("!f")
ControlSend("112.txt - ���±�", "", "", "!f");
Sleep(800)
;Send("x")
ControlSend("112.txt - ���±�", "", "", "x");//ControlSend��Send���ܸ���ǿ�󣬿��Կ��Ƴ��ĸ����ڷ���

WinWait("112.txt - ���±�", "")
ControlClick("���±�", "", "[CLASS:Button; Instance:1;]");���ڵı��⣻���ڵ����ݣ����ڵİ�ť
;ControlClick("���±�", "", "[CLASS:Button; Text:ȡ��; Instance:1;]");