#include <IE.au3>
Dim $iProcessStatus = 1 ; 1����ʼ״̬ ; 2: ��¼״̬;
$oIE = _IECreate ("http://bbs.tianya.cn/compose.jsp?item=394",0,1,1,0)
$oSubmit = _IEGetObjByName ($oIE, "vwriter")

If @error Then ;û���ҵ�vwriter��˵���Ѿ���¼
	MsgBox(262208, "Fail", "The string was NOT found   ")
	$iProcessStatus = 2
EndIf

If $iProcessStatus = 1 Then 
	_IEAction ($oSubmit, "focus")
	Sleep(2000)
	ControlSend("", "", "", "tigerandmouse");

	$oSubmit = _IEGetObjByName ($oIE, "vpassword")
	_IEAction ($oSubmit, "focus")
	ControlSend("", "", "", "tigerandmouse1984");

	$oSubmit = _IEGetObjByName ($oIE, "loginBtn")
	_IEAction ($oSubmit, "focus")
	ControlSend("", "", "[CLASS:Internet Explorer_Server; INSTANCE:1]", "{Enter}");
	
	$iProcessStatus = 2
	Sleep(5000);�ȴ�ҳ����ת
Endif

If $iProcessStatus = 2 Then 
	;�������ӵı���
	$oIE2 = _IEAttach("http://bbs.tianya.cn/compose.jsp?item=394","url")
	$TagA=_IETagNameGetCollection($oIE2,"input")
	For $A In $TagA
			If $A.classname='bbsTitle fl' And $A.classname<>'0' Then
					_IEAction ($A, "focus")
					ControlSend("", "", "", "��������Ů��������������ʲô����");
					;_IEFormElementSetValue($A,"1231232");���ַ������������
					Sleep(2000)
			EndIf
	Next

	;�������ӵı���
	$oSubmit = _IEGetObjByName ($oIE, "textAreaContainer")
	$hwnd = _IEPropertyGet($oIE, "hwnd")
	_IEAction ($oSubmit, "focus")
	ControlSend("", "", "", "����������λŮ�����Ը������������Һ�׷�����");

	;���������Ƿ�ԭ��
	$oIE2 = _IEAttach("http://bbs.tianya.cn/compose.jsp?item=394","url")
	$TagA=_IETagNameGetCollection($oIE2,"input")
	For $A In $TagA
			If $A.type='radio' And $A.type<>'0' And $A.value = 1 And $A.name = "isSelf" Then
					$A.click()
			EndIf
	Next

	;�ύ��������
	$oIE2 = _IEAttach("http://bbs.tianya.cn/compose.jsp?item=394","url")
	$TagA=_IETagNameGetCollection($oIE2,"input")
	For $A In $TagA
			If $A.classname='common-submitBtn fr inputbtn ty-bluebtn' And $A.classname<>'0' Then
					$A.click()
			EndIf
	Next
	;_IEFormElementRadioSelect ($oSubmit, "1", "isSelf", 1, "byValue");���ַ���û��form����ʵ�ֲ���
Endif