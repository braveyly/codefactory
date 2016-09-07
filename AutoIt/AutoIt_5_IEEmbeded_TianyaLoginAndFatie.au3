#AutoIt3Wrapper_UseX64=n
#include <GUIConstants.au3>
#include <WindowsConstants.au3>
#include <IE.au3>
Local $ArticleNum = 10
Local $LuntanNum = 4
Local $AppTitle = "赛维发帖机"
Local $SelectFlag = -1
Local $WhileFlag = 0
Local $Ret = 0
Local $avArray[10][20]
Dim $iProcessStatus = 0 ; 0:页面未打开; 1：页面打开了; 2:登录成功了; 9：页面出现了故障

Dim $IndexTianya = 0
Dim $IndexDouban = 1
Dim $IndexDqc = 2
Dim $Index818u = 3
Dim $IndexXcht = 4

Dim $IndexName = 0
Dim $IndexUrl = 1
Dim $IndexArticleNum = 2
Dim $IndexInternalTime = 3
Dim $IndexUsernameFlag = 4
Dim $IndexUsername = 5
Dim $IndexPasswordFlag = 6
Dim $IndexPassword = 7
Dim $IndexLogin = 8
Dim $IndexTitleFlag = 9
Dim $IndexContentFlag = 10
Dim $IndexExitUrl = 11
$avArray[$IndexTianya][$IndexName] = "女生宿舍"
$avArray[$IndexTianya][$IndexUrl] = "http://bbs.tianya.cn/compose.jsp?item=394"
$avArray[$IndexTianya][$IndexArticleNum] = $ArticleNum
$avArray[$IndexTianya][$IndexInternalTime] = "30s"
$avArray[$IndexTianya][$IndexUsernameFlag] = "vwriter" ;登录名标签
$avArray[$IndexTianya][$IndexUsername] = "tigerandmouse" ;登录名
$avArray[$IndexTianya][$IndexPasswordFlag] = "vpassword" ;密码标签
$avArray[$IndexTianya][$IndexPassword] = "tigerandmouse1984" ;密码
$avArray[$IndexTianya][$IndexLogin] = "loginBtn" ;登录提交按钮
$avArray[$IndexTianya][$IndexTitleFlag] = "" ;标题标签
$avArray[$IndexTianya][$IndexContentFlag] = "textAreaContainer" ;内容标签
$avArray[$IndexTianya][$IndexExitUrl] = "http://passport.tianya.cn/logout?returnURL=http%3A%2F%2Fwww.tianya.cn%2Fuser%2Flogout%3FreturnURl%3Dhttp%253A%252F%252Fbbs.tianya.cn%252Fcompose.jsp%253Fitem%253D394&fowardFlag=y"


$avArray[$IndexDouban][$IndexName] = "豆瓣日记"
$avArray[$IndexDouban][$IndexUrl] = "https://www.douban.com/note/create"
$avArray[$IndexDouban][$IndexArticleNum] = $ArticleNum
$avArray[$IndexDouban][$IndexInternalTime] = "30s"
$avArray[$IndexDouban][$IndexUsernameFlag] = "form_email" ;登录名标签
$avArray[$IndexDouban][$IndexUsername] = "aisen2015@126.com" ;登录名
$avArray[$IndexDouban][$IndexPasswordFlag] = "form_password" ;密码标签
$avArray[$IndexDouban][$IndexPassword] = "aisen170910" ;密码
$avArray[$IndexDouban][$IndexLogin] = "login" ;登录提交按钮
$avArray[$IndexDouban][$IndexTitleFlag] = "note_title" ;标题标签
$avArray[$IndexDouban][$IndexContentFlag] = "note_text" ;内容标签
$avArray[$IndexDouban][$IndexExitUrl] = "https://www.douban.com/accounts/logout?source=main&ck=nqwX" ;ToDo:退出的ck码是随机的，需要进一步确认如何正确退出


$avArray[$IndexDqc][$IndexName] = "地球城博客"
$avArray[$IndexDqc][$IndexUrl] = "http://blog.dqccc.com/UserManager/LogAdd.aspx"
$avArray[$IndexDqc][$IndexArticleNum] = $ArticleNum
$avArray[$IndexDqc][$IndexInternalTime] = "30s"
$avArray[$IndexDqc][$IndexUsernameFlag] = "username" ;登录名标签
$avArray[$IndexDqc][$IndexUsername] = "aisen2015" ;登录名
$avArray[$IndexDqc][$IndexPasswordFlag] = "password" ;密码标签
$avArray[$IndexDqc][$IndexPassword] = "aisen170910" ;密码
$avArray[$IndexDqc][$IndexLogin] = "btnOK" ;登录提交按钮
$avArray[$IndexDqc][$IndexTitleFlag] = "LogTitle" ;标题标签
$avArray[$IndexDqc][$IndexContentFlag] = "ueditor_0" ;内容标签
$avArray[$IndexDqc][$IndexExitUrl] = "http://blog.dqccc.com/MemberLogout.aspx?ReturnURL=http://blog.dqccc.com/UserManager/LogAdd.aspx"

$avArray[$Index818u][$IndexName] = "818u"
$avArray[$Index818u][$IndexUrl] = "http://www.818u.com/basedo/poste.aspx?classid=43&citydm=beijing"
$avArray[$Index818u][$IndexArticleNum] = $ArticleNum
$avArray[$Index818u][$IndexInternalTime] = "30s"
$avArray[$Index818u][$IndexUsernameFlag] = "usernane" ;登录名标签
$avArray[$Index818u][$IndexUsername] = "aisen" ;登录名
$avArray[$Index818u][$IndexPasswordFlag] = "PASSW0RD" ;密码标签
$avArray[$Index818u][$IndexPassword] = "170910" ;密码
$avArray[$Index818u][$IndexLogin] = "Button1" ;登录提交按钮
$avArray[$Index818u][$IndexTitleFlag] = "titles" ;标题标签
$avArray[$Index818u][$IndexContentFlag] = "neirong" ;内容标签
$avArray[$Index818u][$IndexExitUrl] = "www.818u.com/members/logout.aspx";818也不能彻底登出

$avArray[$IndexXcht][$IndexName] = "西池胡同"
$avArray[$IndexXcht][$IndexUrl] = "http://account.xici.net/login/modal?refer=b1145177%2F%23msg&t=1444643363805"
$avArray[$IndexXcht][$IndexArticleNum] = $ArticleNum
$avArray[$IndexXcht][$IndexInternalTime] = "30s"
$avArray[$IndexXcht][$IndexUsernameFlag] = "username" ;登录名标签
$avArray[$IndexXcht][$IndexUsername] = "18571607079" ;登录名
$avArray[$IndexXcht][$IndexPasswordFlag] = "password" ;密码标签
$avArray[$IndexXcht][$IndexPassword] = "aisen170910" ;密码
$avArray[$IndexXcht][$IndexLogin] = "" ;登录提交按钮
$avArray[$IndexXcht][$IndexTitleFlag] = "doc_title" ;标题标签
$avArray[$IndexXcht][$IndexContentFlag] = "mce_editor_0" ;内容标签
$avArray[$IndexXcht][$IndexExitUrl] = "http://account.xici.net/logout?refer=http%3A%2F%2Fwww.xici.net%2Fb1145177%2F%23msg"

$oIE = _IECreateEmbedded ()
GUICreate($AppTitle, 640*1.5-150, 580-150, _
        (@DesktopWidth - 640) / 2, (@DesktopHeight - 580) / 2); _
        ;$WS_OVERLAPPEDWINDOW + $WS_VISIBLE + $WS_CLIPSIBLINGS + $WS_CLIPCHILDREN)

$GUI_Button_STOP = GUICtrlCreateButton("STOP", 110, 580*0.7-30, 40, 30)
$GUI_Button_FaTie = GUICtrlCreateButton("START", 60, 580*0.7-30, 40, 30)
$GUI_Button_Circle_FaTie = GUICtrlCreateButton("LOOP", 10, 580*0.7-30, 40, 30)

;GUISetBkColor (0x00E0FFFF)  ; 更改背景颜色
$listview = GuiCtrlCreateListView ("论坛名		  			",10,40-30,100*1.5,150*1.5);,$WS_CLIPSIBLINGS)
$item1=GuiCtrlCreateListViewItem("天涯						",$listview)
$item2=GuiCtrlCreateListViewItem("豆瓣						",$listview)
$item3=GuiCtrlCreateListViewItem("地球城博客						",$listview)
$item4=GuiCtrlCreateListViewItem("818u						",$listview)
$item5=GuiCtrlCreateListViewItem("西祠胡同						",$listview)
GuiCtrlSetState(-1,$GUI_ACCEPTFILES)   ; 允许拖放
GuiSetState()
$listview2 = GuiCtrlCreateListView ("标题    |内容                            						",10,10+160+50-30,100*1.5,150*1.2);,$LVS_SORTDESCENDING)
$item12=GuiCtrlCreateListViewItem("版本名|NULL",$listview2)
$item22=GuiCtrlCreateListViewItem("网址|NULL",$listview2)
$item32=GuiCtrlCreateListViewItem("帖子数|0",$listview2)
$item42=GuiCtrlCreateListViewItem("间隔时间|0s",$listview2)
$item52=GuiCtrlCreateListViewItem("帖子标题|NULL",$listview2)
$item62=GuiCtrlCreateListViewItem("帖子内容|NULL",$listview2)
GuiCtrlSetState(-1,$GUI_ACCEPTFILES)   ; 允许拖放
GuiSetState()
GUIRegisterMsg($WM_COMMAND, "WM_COMMAND") ;截获WM_COMMAND消息以处理
Local $flag_Interrupt = 0
$GUIActiveX = GUICtrlCreateObj($oIE, 640*0.3, 40-30, 600, 400)
_IENavigate ($oIE, "http://leisaijun.sinaapp.com/", 0)
While 1
    $msg = GUIGetMsg()
    Select
        Case $msg = $GUI_EVENT_CLOSE
            ExitLoop
        Case $msg = $GUI_Button_FaTie
			If $SelectFlag = -1 Then
				MsgBox(0, "错误", "未选择论坛。")
			Else   
				
				For $i=1 To Int($avArray[$SelectFlag][2])
					_IENavigate ($oIE, $avArray[$SelectFlag][$IndexUrl], 0)
					If -1 = search_file(String($i)) Then
						ExitLoop
					Endif
				Next 
			 Endif
			 
		Case $msg = $GUI_Button_Circle_FaTie
				For $i=1 To $ArticleNum
					For $j=0 To $LuntanNum
					_IENavigate ($oIE, $avArray[$j][$IndexUrl], 0)
					If @Error Then
						MsgBox(0, "GUI 事件", "页面打不开，超时，错误码"&String(@Error)) ;页面超时
					Else
						$SelectFlag = $j
						If -1 = search_file(String($i)) Then
							$i = $ArticleNum+1;退出外层循环
							ExitLoop							
						Endif
					Endif

					 Next
				Next 
		Case $msg = $item1
			 GUICtrlSetData($item12,"板块名|女生宿舍")
			 GUICtrlSetData($item22,"网址|http://bbs.tianya.cn/compose.jsp?item=394")
			 GUICtrlSetData($item32,"帖子数|3")
			 GUICtrlSetData($item42,"间隔时间|30s")
			 $SelectFlag = 0
		Case $msg = $item2
			 GUICtrlSetData($item12,"板块名|"&$avArray[1][0])
			 GUICtrlSetData($item22,"网址|"&$avArray[1][1])
			 GUICtrlSetData($item32,"帖子数|"&$avArray[1][2])
			 GUICtrlSetData($item42,"间隔时间|"&$avArray[1][3])
			 $SelectFlag = 1
		Case $msg = $item3
			 GUICtrlSetData($item12,"板块名|"&$avArray[2][0])
			 GUICtrlSetData($item22,"网址|"&$avArray[2][1])
			 GUICtrlSetData($item32,"帖子数|"&$avArray[2][2])
			 GUICtrlSetData($item42,"间隔时间|"&$avArray[2][3])
			 $SelectFlag = 2
		Case $msg = $item4
			 GUICtrlSetData($item12,"板块名|"&$avArray[3][0])
			 GUICtrlSetData($item22,"网址|"&$avArray[3][1])
			 GUICtrlSetData($item32,"帖子数|"&$avArray[3][2])
			 GUICtrlSetData($item42,"间隔时间|"&$avArray[3][3])
			 $SelectFlag = 3
		Case $msg = $item5
			 GUICtrlSetData($item12,"板块名|"&$avArray[4][0])
			 GUICtrlSetData($item22,"网址|"&$avArray[4][1])
			 GUICtrlSetData($item32,"帖子数|"&$avArray[4][2])
			 GUICtrlSetData($item42,"间隔时间|"&$avArray[4][3])
			 $SelectFlag = 4
    EndSelect
WEnd
Func CheckDengluWeb($index)
	$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexUsernameFlag])
	If @error Then 
		Return 0
	Else
		Return 1
	EndIf
EndFunc

Func CheckFabuWeb($index)
		$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexContentFlag])						
		If @error Then 
			Return 0
		Else
			Return 1
		EndIf
EndFunc

Func setUsernamePasswordAndClick($index)
			ConsoleWrite("Start To set username And password"&@crlf)		
			$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexUsernameFlag])
			_IEFormElementSetValue($oSubmit,$avArray[$index][$IndexUsername]);
			$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexPasswordFlag])
			_IEFormElementSetValue($oSubmit,$avArray[$index][$IndexPassword]);
			ConsoleWrite("End To set username And password"&@crlf)	
		
			If $index = $IndexXcht Then 
				$oIE2 = _IEAttach($AppTitle,"embedded")
				$TagA=_IETagNameGetCollection($oIE2,"button")
				For $A In $TagA
				If $A.classname='btn btn_radius' And $A.type ='submit'  Then
						ConsoleWrite("XCHT Click Denglu button"&@crlf)	
						$A.click()
				EndIf
				Next
			Else
				$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexLogin])
				If @error Then 
					MsgBox(262208,"运行错误","请保持软件在前端")
					;MsgBox(262208, "Fail", "The string was NOT found   ")
					Return -1
				Else
					ConsoleWrite("Click Denglu button"&@crlf)
					$oSubmit.click()
				Endif
			Endif
EndFunc
Func Login($index)		
		ConsoleWrite("====Start Protect denglu web==="&@crlf)
		;Step 2.2: 如果页面未跳转（例如：网速慢或者需要输入验证码），则在此页面一直循环，直到用户点击停止按钮或者10s超时
		Dim $count = 0
		Dim $setFlag = 0
		While $iProcessStatus = 1  And $count < 50
			If $flag_Interrupt = 1 Then
				$flag_Interrupt = 0
				ConsoleWrite("Stop now for protecting denglu web"&@crlf)
				Return -1
			Endif
		
		$dengluWebExist = CheckDengluWeb($index)
		$fabuWebExist = CheckFabuWeb($index)
		
		If $dengluWebExist = 0 And $fabuWebExist = 0 Then  ;两个页面都不存在，则等10秒后，置标志位为9，然后退出
			If $count = 50 Then
				ConsoleWrite("$dengluWebExist = 0 And $fabuWebExist = 0"&@crlf)
				$iProcessStatus = 9
			Endif
		Elseif $dengluWebExist = 0 And $fabuWebExist = 1 Then ; 只有发布页面，置标志位为2，立即退出
			ConsoleWrite("$dengluWebExist = 0 And $fabuWebExist = 1"&@crlf)
			$iProcessStatus = 2
			;$count = 50 ;只有发布页，也要等待10s
		Else  ;只要有登录页面，置标志位为1，do nothing
				;Step 2.1: 填充用户名和密码，点击登录
			ConsoleWrite("$dengluWebExist ="& $dengluWebExist& "And $fabuWebExist ="&$fabuWebExist&@crlf)
			;If $setFlag = 0 Then 
				setUsernamePasswordAndClick($index)	
			;	$setFlag = 1
			;Endif			
		Endif

			Sleep(200)
			$count+=1	
		WEnd 
		ConsoleWrite("====Stop Protect denglu web==="&@crlf)
EndFunc
	
Func FaTie($index, $title, $content)
	If $index = $IndexXcht Or $index = $IndexDqc Then ;xcht 页面打开太慢，一直显示空白，等待5秒，保障下面的流程的正确性
		Sleep(5000)
	Endif
	
	ConsoleWrite("==Start wait the web to be opened=="&$index&@crlf)
;Step 1: 等待10秒通过username和title判断是否打开页面,而不是通过_ieloadwait	
	Dim $count = 0
	While $count < 50
		$dengluWebExist = CheckDengluWeb($index)
		$fabuWebExist = CheckFabuWeb($index)
		If $dengluWebExist = 1 Or $fabuWebExist = 1 Then
			$count = 50
			$iProcessStatus = 1
		Endif
		Sleep(200)
		$count+=1
	Wend
	ConsoleWrite("==End wait the web to be opened=="&$index&"$dengluWebExist="&$dengluWebExist&"$fabuWebExist="&$fabuWebExist&@crlf)

;Step 2:页面打开后，才能进行登录(0-->1)
	If $iProcessStatus = 1 Then 
		Login($index)
	Endif

;Step 3:默认登录了(不需要输入用户名和密码)，则需要判断是否已经进入了发帖的页面，并且设置标示位
;这一步其实可以直接设置$iProcessStatus = 2，对titleflag进行判断是为了保险起见
;	$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexContentFlag])			
;	If @error Then 
;	Else
;		$iProcessStatus = 2
;	EndIf
	
;Step 3：登录后，才能进行帖子设置(1-->2)
	If $iProcessStatus = 2 Then 
		;Step4.1:设置帖子的标题
		If $index = $IndexTianya Then
			$oIE2 = _IEAttach($AppTitle,"embedded")
			$TagA=_IETagNameGetCollection($oIE2,"input")
			For $A In $TagA
				If $A.classname='bbsTitle fl' And $A.classname<>'0' Then
						_IEFormElementSetValue($A,$title);两种方法都可以填充
				EndIf
			Next
		Else
			$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexTitleFlag])
			
			If $index = $IndexXcht Then
				_IEAction ($oSubmit, "focus")
				ControlSend($AppTitle, "", "", $title);保证验证码可以出来
			Else
				_IEFormElementSetValue($oSubmit,$title);
			Endif
		Endif
				
		;Step4.2:设置帖子的内容
		If $index = $IndexXcht Or $index = $IndexDqc Then
			$oFrame = _IEFrameGetObjByName ($oIE, $avArray[$index][$IndexContentFlag])
			_IEBodyWriteHTML ($oFrame, $content)
		Else
			$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexContentFlag])
			_IEFormElementSetValue($oSubmit,$content);
		EndIf

		;Step4.3:Others 
		If $index = $Index818u Then
			;设置818U帖子关键词
			$oSubmit = _IEGetObjByName ($oIE, "keywords")
			_IEFormElementSetValue($oSubmit,$title);
		Endif
		
		If $index = $IndexTianya Then
			;设置天涯帖子是否原创
			$oIE2 = _IEAttach($AppTitle,"embedded")
			$TagA=_IETagNameGetCollection($oIE2,"input")
			For $A In $TagA
				If $A.type='radio' And $A.type<>'0' And $A.value = 0 And $A.name = "isSelf" Then
						$A.click()
				EndIf
			Next
		Endif
		
		ConsoleWrite("Step4.4：输入验证码和点击提交的健全性保护机制 check "&@crlf)
		;Step4.4：输入验证码和点击提交的健全性保护机制
		Local $count = 0
		While $count < 50
			ConsoleWrite("Step4.4.1: 当用户点击停止按钮时，终止循环 "&@crlf)
			;Step4.4.1: 当用户点击停止按钮时，终止循环
			If $flag_Interrupt = 1 Then
				$flag_Interrupt = 0
				Return -1
			Endif
			
			;Step4.4.2: 如果帖子title和content为空，重新设置
			If $index = $IndexTianya Then 
				ConsoleWrite("Step4.4.2: 如果帖子title和content为空，重新设置 TianYa "&@crlf)
				$oIE2 = _IEAttach($AppTitle,"embedded")
				$TagA=_IETagNameGetCollection($oIE2,"input")
				For $A In $TagA
					If $A.classname='bbsTitle fl' And $A.classname<>'0' Then
						$value = _IEFormElementGetValue($A)
						If $value <> "" Then
						Else
							_IEFormElementSetValue($A,$title)
						EndIf													
					EndIf
				Next
			Else
				ConsoleWrite("Step4.4.2: 如果帖子title和content为空，重新设置 No TianYa "&@crlf)
				$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexTitleFlag])
				If @error Then 
				Else
					$value = _IEFormElementGetValue($oSubmit)
					If $value <> "" Then
					Else
						_IEFormElementSetValue($oSubmit,$title)
					EndIf	
				EndIf

			EndIf
						
			If $index = $IndexXcht Or $index = $IndexDqc Then 
				ConsoleWrite("Step4.4.2: 如果帖子title和content为空，重新设置 $IndexXcht $IndexDqc "&@crlf)
				$oFrame = _IEFrameGetObjByName ($oIE, $avArray[$index][$IndexContentFlag])
				If @error Then 
				Else
					$value = _IEBodyReadText ($oFrame)
					If $value <> "" Then	
					Else
						_IEBodyWriteHTML ($oFrame, $content)
					EndIf
				EndIf

			Else
				ConsoleWrite("Step4.4.2: 如果帖子title和content为空，重新设置 No $IndexXcht $IndexDqc "&@crlf)
				$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexContentFlag])
				If @error Then 
				Else
					$value = _IEFormElementGetValue($oSubmit)
					If $value <> "" Then	
					Else	
						_IEFormElementSetValue($oSubmit,$content);
					EndIf
				EndIf
			Endif

			$dengluWebExist = CheckDengluWeb($index)
			$fabuWebExist = CheckFabuWeb($index)
			If $dengluWebExist = 0 And $fabuWebExist = 0 Then
				$count = 50
				ConsoleWrite("Denglu and Fabu web is not existed"&@crlf)
			Endif
			
			ConsoleWrite("$dengluWebExist is "& $dengluWebExist & "$fabuWebExist is " & $fabuWebExist &@crlf)
			;Step4.4.3: 让用户选择继续等待	
			$count = $count + 1
			Sleep(200)
			If $count = 50 Then
				$PressButton = MsgBox(4100, "测试", "是否继续输入？此对话框将会在10秒后自动消失", 10)
				If $PressButton = 6 Then
					$count = 0
				Endif
			Endif
		WEnd
		ConsoleWrite("Final over "&@crlf)
	Endif
	
	If $index <> $Index818u And $index <> $IndexDouban Then 
		_IENavigate ($oIE, $avArray[$index][$IndexExitUrl], 0)
	Endif
	
	Sleep(3000)
	;$oIE2 = _IEAttach($AppTitle,"embedded");ToDo:退出嵌入的浏览器
	;_IEQuit($oIE2)
EndFunc

Func search_file($file_index)
	; 显示当前目录下的所有文件的文件名，注意有返回 "." 和 ".."
	$search = FileFindFirstFile(".\tianya\"&$file_index&"*.txt")  

	; 检查搜索是否成功 
	If $search = -1 Then
		MsgBox(0, "错误", "无任何文件或文件夹与指定的搜索字符串匹配")
		return
	EndIf

	While 1
		$file_name = FileFindNextFile($search) 
		If @error Then ExitLoop
		
		$full_file = String(".\tianya\")
		$full_file &= String($file_name)
		;MsgBox(4096, "文件：", $full_file)
		
		
		$file = FileOpen($full_file, 0)

		; 检查文件是否正常打开
		If $file = -1 Then
			MsgBox(0, "错误", "无法打开目标文件。")
			Exit
		EndIf

		; 每次读取1行字符直到文件结尾（EOF，End-Of-File） 为止
		Local $file_content = ""
		While 1
			$line = FileReadLine($file)
			$file_content &= $line
			$file_content &= "\n"
			If @error = -1 Then ExitLoop
			;MsgBox(0, "读到的文本：", $line)
		Wend
		;MsgBox(0, "file_content：", $file_content)
		Local $Status = 1
		Local $title = StringMid($file_name,3,StringLen($file_name)-2-4)
		GUICtrlSetData($item32,"帖子数|" & $file_index &"/"& $ArticleNum)
		GUICtrlSetData($item52,"帖子标题|"& $file_name)
		GUICtrlSetData($item62,"帖子内容|"& $file_content)
				
		$Status = FaTie($SelectFlag,$title,$file_content)
		If $status = -1 Then
			Return $Status
		Endif

		FileClose($file)
	WEnd

	; 关闭搜索句柄
	FileClose($search)        
EndFunc

Func WM_COMMAND($hWnd, $msg, $wParam, $lParam)

        #forceref $hWnd, $Msg

        $LoWord = BitAND($wParam, 0x0000FFFF)

        If $LoWord = $GUI_Button_STOP Then 
		$flag_Interrupt = 1
		;MsgBox(0, "错误", "Stop button is clicked!!")
		Endif
		
        Return $GUI_RUNDEFMSG

EndFunc   ;==>WM_COMMAND