#include <IE.au3>
Dim $iProcessStatus = 1 ; 1：初始状态 ; 2: 登录状态;
$oIE = _IECreate ("http://bbs.tianya.cn/compose.jsp?item=394",0,1,1,0)
$oSubmit = _IEGetObjByName ($oIE, "vwriter")

If @error Then ;没有找到vwriter，说明已经登录
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
	Sleep(5000);等待页面跳转
Endif

If $iProcessStatus = 2 Then 
	;设置帖子的标题
	$oIE2 = _IEAttach("http://bbs.tianya.cn/compose.jsp?item=394","url")
	$TagA=_IETagNameGetCollection($oIE2,"input")
	For $A In $TagA
			If $A.classname='bbsTitle fl' And $A.classname<>'0' Then
					_IEAction ($A, "focus")
					ControlSend("", "", "", "再问再问女生的恋爱心理是什么样的");
					;_IEFormElementSetValue($A,"1231232");两种方法都可以填充
					Sleep(2000)
			EndIf
	Next

	;设置帖子的标题
	$oSubmit = _IEGetObjByName ($oIE, "textAreaContainer")
	$hwnd = _IEPropertyGet($oIE, "hwnd")
	_IEAction ($oSubmit, "focus")
	ControlSend("", "", "", "有人吗，有哪位女生可以告诉我吗，这样我好追求别人");

	;设置帖子是否原创
	$oIE2 = _IEAttach("http://bbs.tianya.cn/compose.jsp?item=394","url")
	$TagA=_IETagNameGetCollection($oIE2,"input")
	For $A In $TagA
			If $A.type='radio' And $A.type<>'0' And $A.value = 1 And $A.name = "isSelf" Then
					$A.click()
			EndIf
	Next

	;提交整个帖子
	$oIE2 = _IEAttach("http://bbs.tianya.cn/compose.jsp?item=394","url")
	$TagA=_IETagNameGetCollection($oIE2,"input")
	For $A In $TagA
			If $A.classname='common-submitBtn fr inputbtn ty-bluebtn' And $A.classname<>'0' Then
					$A.click()
			EndIf
	Next
	;_IEFormElementRadioSelect ($oSubmit, "1", "isSelf", 1, "byValue");这种方法没有form对象实现不了
Endif