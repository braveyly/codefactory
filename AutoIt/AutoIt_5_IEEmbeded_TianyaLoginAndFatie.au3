#AutoIt3Wrapper_UseX64=n
#include <GUIConstants.au3>
#include <WindowsConstants.au3>
#include <IE.au3>
Local $ArticleNum = 10
Local $LuntanNum = 4
Local $AppTitle = "��ά������"
Local $SelectFlag = -1
Local $WhileFlag = 0
Local $Ret = 0
Local $avArray[10][20]
Dim $iProcessStatus = 0 ; 0:ҳ��δ��; 1��ҳ�����; 2:��¼�ɹ���; 9��ҳ������˹���

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
$avArray[$IndexTianya][$IndexName] = "Ů������"
$avArray[$IndexTianya][$IndexUrl] = "http://bbs.tianya.cn/compose.jsp?item=394"
$avArray[$IndexTianya][$IndexArticleNum] = $ArticleNum
$avArray[$IndexTianya][$IndexInternalTime] = "30s"
$avArray[$IndexTianya][$IndexUsernameFlag] = "vwriter" ;��¼����ǩ
$avArray[$IndexTianya][$IndexUsername] = "tigerandmouse" ;��¼��
$avArray[$IndexTianya][$IndexPasswordFlag] = "vpassword" ;�����ǩ
$avArray[$IndexTianya][$IndexPassword] = "tigerandmouse1984" ;����
$avArray[$IndexTianya][$IndexLogin] = "loginBtn" ;��¼�ύ��ť
$avArray[$IndexTianya][$IndexTitleFlag] = "" ;�����ǩ
$avArray[$IndexTianya][$IndexContentFlag] = "textAreaContainer" ;���ݱ�ǩ
$avArray[$IndexTianya][$IndexExitUrl] = "http://passport.tianya.cn/logout?returnURL=http%3A%2F%2Fwww.tianya.cn%2Fuser%2Flogout%3FreturnURl%3Dhttp%253A%252F%252Fbbs.tianya.cn%252Fcompose.jsp%253Fitem%253D394&fowardFlag=y"


$avArray[$IndexDouban][$IndexName] = "�����ռ�"
$avArray[$IndexDouban][$IndexUrl] = "https://www.douban.com/note/create"
$avArray[$IndexDouban][$IndexArticleNum] = $ArticleNum
$avArray[$IndexDouban][$IndexInternalTime] = "30s"
$avArray[$IndexDouban][$IndexUsernameFlag] = "form_email" ;��¼����ǩ
$avArray[$IndexDouban][$IndexUsername] = "aisen2015@126.com" ;��¼��
$avArray[$IndexDouban][$IndexPasswordFlag] = "form_password" ;�����ǩ
$avArray[$IndexDouban][$IndexPassword] = "aisen170910" ;����
$avArray[$IndexDouban][$IndexLogin] = "login" ;��¼�ύ��ť
$avArray[$IndexDouban][$IndexTitleFlag] = "note_title" ;�����ǩ
$avArray[$IndexDouban][$IndexContentFlag] = "note_text" ;���ݱ�ǩ
$avArray[$IndexDouban][$IndexExitUrl] = "https://www.douban.com/accounts/logout?source=main&ck=nqwX" ;ToDo:�˳���ck��������ģ���Ҫ��һ��ȷ�������ȷ�˳�


$avArray[$IndexDqc][$IndexName] = "����ǲ���"
$avArray[$IndexDqc][$IndexUrl] = "http://blog.dqccc.com/UserManager/LogAdd.aspx"
$avArray[$IndexDqc][$IndexArticleNum] = $ArticleNum
$avArray[$IndexDqc][$IndexInternalTime] = "30s"
$avArray[$IndexDqc][$IndexUsernameFlag] = "username" ;��¼����ǩ
$avArray[$IndexDqc][$IndexUsername] = "aisen2015" ;��¼��
$avArray[$IndexDqc][$IndexPasswordFlag] = "password" ;�����ǩ
$avArray[$IndexDqc][$IndexPassword] = "aisen170910" ;����
$avArray[$IndexDqc][$IndexLogin] = "btnOK" ;��¼�ύ��ť
$avArray[$IndexDqc][$IndexTitleFlag] = "LogTitle" ;�����ǩ
$avArray[$IndexDqc][$IndexContentFlag] = "ueditor_0" ;���ݱ�ǩ
$avArray[$IndexDqc][$IndexExitUrl] = "http://blog.dqccc.com/MemberLogout.aspx?ReturnURL=http://blog.dqccc.com/UserManager/LogAdd.aspx"

$avArray[$Index818u][$IndexName] = "818u"
$avArray[$Index818u][$IndexUrl] = "http://www.818u.com/basedo/poste.aspx?classid=43&citydm=beijing"
$avArray[$Index818u][$IndexArticleNum] = $ArticleNum
$avArray[$Index818u][$IndexInternalTime] = "30s"
$avArray[$Index818u][$IndexUsernameFlag] = "usernane" ;��¼����ǩ
$avArray[$Index818u][$IndexUsername] = "aisen" ;��¼��
$avArray[$Index818u][$IndexPasswordFlag] = "PASSW0RD" ;�����ǩ
$avArray[$Index818u][$IndexPassword] = "170910" ;����
$avArray[$Index818u][$IndexLogin] = "Button1" ;��¼�ύ��ť
$avArray[$Index818u][$IndexTitleFlag] = "titles" ;�����ǩ
$avArray[$Index818u][$IndexContentFlag] = "neirong" ;���ݱ�ǩ
$avArray[$Index818u][$IndexExitUrl] = "www.818u.com/members/logout.aspx";818Ҳ���ܳ��׵ǳ�

$avArray[$IndexXcht][$IndexName] = "���غ�ͬ"
$avArray[$IndexXcht][$IndexUrl] = "http://account.xici.net/login/modal?refer=b1145177%2F%23msg&t=1444643363805"
$avArray[$IndexXcht][$IndexArticleNum] = $ArticleNum
$avArray[$IndexXcht][$IndexInternalTime] = "30s"
$avArray[$IndexXcht][$IndexUsernameFlag] = "username" ;��¼����ǩ
$avArray[$IndexXcht][$IndexUsername] = "18571607079" ;��¼��
$avArray[$IndexXcht][$IndexPasswordFlag] = "password" ;�����ǩ
$avArray[$IndexXcht][$IndexPassword] = "aisen170910" ;����
$avArray[$IndexXcht][$IndexLogin] = "" ;��¼�ύ��ť
$avArray[$IndexXcht][$IndexTitleFlag] = "doc_title" ;�����ǩ
$avArray[$IndexXcht][$IndexContentFlag] = "mce_editor_0" ;���ݱ�ǩ
$avArray[$IndexXcht][$IndexExitUrl] = "http://account.xici.net/logout?refer=http%3A%2F%2Fwww.xici.net%2Fb1145177%2F%23msg"

$oIE = _IECreateEmbedded ()
GUICreate($AppTitle, 640*1.5-150, 580-150, _
        (@DesktopWidth - 640) / 2, (@DesktopHeight - 580) / 2); _
        ;$WS_OVERLAPPEDWINDOW + $WS_VISIBLE + $WS_CLIPSIBLINGS + $WS_CLIPCHILDREN)

$GUI_Button_STOP = GUICtrlCreateButton("STOP", 110, 580*0.7-30, 40, 30)
$GUI_Button_FaTie = GUICtrlCreateButton("START", 60, 580*0.7-30, 40, 30)
$GUI_Button_Circle_FaTie = GUICtrlCreateButton("LOOP", 10, 580*0.7-30, 40, 30)

;GUISetBkColor (0x00E0FFFF)  ; ���ı�����ɫ
$listview = GuiCtrlCreateListView ("��̳��		  			",10,40-30,100*1.5,150*1.5);,$WS_CLIPSIBLINGS)
$item1=GuiCtrlCreateListViewItem("����						",$listview)
$item2=GuiCtrlCreateListViewItem("����						",$listview)
$item3=GuiCtrlCreateListViewItem("����ǲ���						",$listview)
$item4=GuiCtrlCreateListViewItem("818u						",$listview)
$item5=GuiCtrlCreateListViewItem("������ͬ						",$listview)
GuiCtrlSetState(-1,$GUI_ACCEPTFILES)   ; �����Ϸ�
GuiSetState()
$listview2 = GuiCtrlCreateListView ("����    |����                            						",10,10+160+50-30,100*1.5,150*1.2);,$LVS_SORTDESCENDING)
$item12=GuiCtrlCreateListViewItem("�汾��|NULL",$listview2)
$item22=GuiCtrlCreateListViewItem("��ַ|NULL",$listview2)
$item32=GuiCtrlCreateListViewItem("������|0",$listview2)
$item42=GuiCtrlCreateListViewItem("���ʱ��|0s",$listview2)
$item52=GuiCtrlCreateListViewItem("���ӱ���|NULL",$listview2)
$item62=GuiCtrlCreateListViewItem("��������|NULL",$listview2)
GuiCtrlSetState(-1,$GUI_ACCEPTFILES)   ; �����Ϸ�
GuiSetState()
GUIRegisterMsg($WM_COMMAND, "WM_COMMAND") ;�ػ�WM_COMMAND��Ϣ�Դ���
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
				MsgBox(0, "����", "δѡ����̳��")
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
						MsgBox(0, "GUI �¼�", "ҳ��򲻿�����ʱ��������"&String(@Error)) ;ҳ�泬ʱ
					Else
						$SelectFlag = $j
						If -1 = search_file(String($i)) Then
							$i = $ArticleNum+1;�˳����ѭ��
							ExitLoop							
						Endif
					Endif

					 Next
				Next 
		Case $msg = $item1
			 GUICtrlSetData($item12,"�����|Ů������")
			 GUICtrlSetData($item22,"��ַ|http://bbs.tianya.cn/compose.jsp?item=394")
			 GUICtrlSetData($item32,"������|3")
			 GUICtrlSetData($item42,"���ʱ��|30s")
			 $SelectFlag = 0
		Case $msg = $item2
			 GUICtrlSetData($item12,"�����|"&$avArray[1][0])
			 GUICtrlSetData($item22,"��ַ|"&$avArray[1][1])
			 GUICtrlSetData($item32,"������|"&$avArray[1][2])
			 GUICtrlSetData($item42,"���ʱ��|"&$avArray[1][3])
			 $SelectFlag = 1
		Case $msg = $item3
			 GUICtrlSetData($item12,"�����|"&$avArray[2][0])
			 GUICtrlSetData($item22,"��ַ|"&$avArray[2][1])
			 GUICtrlSetData($item32,"������|"&$avArray[2][2])
			 GUICtrlSetData($item42,"���ʱ��|"&$avArray[2][3])
			 $SelectFlag = 2
		Case $msg = $item4
			 GUICtrlSetData($item12,"�����|"&$avArray[3][0])
			 GUICtrlSetData($item22,"��ַ|"&$avArray[3][1])
			 GUICtrlSetData($item32,"������|"&$avArray[3][2])
			 GUICtrlSetData($item42,"���ʱ��|"&$avArray[3][3])
			 $SelectFlag = 3
		Case $msg = $item5
			 GUICtrlSetData($item12,"�����|"&$avArray[4][0])
			 GUICtrlSetData($item22,"��ַ|"&$avArray[4][1])
			 GUICtrlSetData($item32,"������|"&$avArray[4][2])
			 GUICtrlSetData($item42,"���ʱ��|"&$avArray[4][3])
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
					MsgBox(262208,"���д���","�뱣�������ǰ��")
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
		;Step 2.2: ���ҳ��δ��ת�����磺������������Ҫ������֤�룩�����ڴ�ҳ��һֱѭ����ֱ���û����ֹͣ��ť����10s��ʱ
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
		
		If $dengluWebExist = 0 And $fabuWebExist = 0 Then  ;����ҳ�涼�����ڣ����10����ñ�־λΪ9��Ȼ���˳�
			If $count = 50 Then
				ConsoleWrite("$dengluWebExist = 0 And $fabuWebExist = 0"&@crlf)
				$iProcessStatus = 9
			Endif
		Elseif $dengluWebExist = 0 And $fabuWebExist = 1 Then ; ֻ�з���ҳ�棬�ñ�־λΪ2�������˳�
			ConsoleWrite("$dengluWebExist = 0 And $fabuWebExist = 1"&@crlf)
			$iProcessStatus = 2
			;$count = 50 ;ֻ�з���ҳ��ҲҪ�ȴ�10s
		Else  ;ֻҪ�е�¼ҳ�棬�ñ�־λΪ1��do nothing
				;Step 2.1: ����û��������룬�����¼
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
	If $index = $IndexXcht Or $index = $IndexDqc Then ;xcht ҳ���̫����һֱ��ʾ�հף��ȴ�5�룬������������̵���ȷ��
		Sleep(5000)
	Endif
	
	ConsoleWrite("==Start wait the web to be opened=="&$index&@crlf)
;Step 1: �ȴ�10��ͨ��username��title�ж��Ƿ��ҳ��,������ͨ��_ieloadwait	
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

;Step 2:ҳ��򿪺󣬲��ܽ��е�¼(0-->1)
	If $iProcessStatus = 1 Then 
		Login($index)
	Endif

;Step 3:Ĭ�ϵ�¼��(����Ҫ�����û���������)������Ҫ�ж��Ƿ��Ѿ������˷�����ҳ�棬�������ñ�ʾλ
;��һ����ʵ����ֱ������$iProcessStatus = 2����titleflag�����ж���Ϊ�˱������
;	$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexContentFlag])			
;	If @error Then 
;	Else
;		$iProcessStatus = 2
;	EndIf
	
;Step 3����¼�󣬲��ܽ�����������(1-->2)
	If $iProcessStatus = 2 Then 
		;Step4.1:�������ӵı���
		If $index = $IndexTianya Then
			$oIE2 = _IEAttach($AppTitle,"embedded")
			$TagA=_IETagNameGetCollection($oIE2,"input")
			For $A In $TagA
				If $A.classname='bbsTitle fl' And $A.classname<>'0' Then
						_IEFormElementSetValue($A,$title);���ַ������������
				EndIf
			Next
		Else
			$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexTitleFlag])
			
			If $index = $IndexXcht Then
				_IEAction ($oSubmit, "focus")
				ControlSend($AppTitle, "", "", $title);��֤��֤����Գ���
			Else
				_IEFormElementSetValue($oSubmit,$title);
			Endif
		Endif
				
		;Step4.2:�������ӵ�����
		If $index = $IndexXcht Or $index = $IndexDqc Then
			$oFrame = _IEFrameGetObjByName ($oIE, $avArray[$index][$IndexContentFlag])
			_IEBodyWriteHTML ($oFrame, $content)
		Else
			$oSubmit = _IEGetObjByName ($oIE, $avArray[$index][$IndexContentFlag])
			_IEFormElementSetValue($oSubmit,$content);
		EndIf

		;Step4.3:Others 
		If $index = $Index818u Then
			;����818U���ӹؼ���
			$oSubmit = _IEGetObjByName ($oIE, "keywords")
			_IEFormElementSetValue($oSubmit,$title);
		Endif
		
		If $index = $IndexTianya Then
			;�������������Ƿ�ԭ��
			$oIE2 = _IEAttach($AppTitle,"embedded")
			$TagA=_IETagNameGetCollection($oIE2,"input")
			For $A In $TagA
				If $A.type='radio' And $A.type<>'0' And $A.value = 0 And $A.name = "isSelf" Then
						$A.click()
				EndIf
			Next
		Endif
		
		ConsoleWrite("Step4.4��������֤��͵���ύ�Ľ�ȫ�Ա������� check "&@crlf)
		;Step4.4��������֤��͵���ύ�Ľ�ȫ�Ա�������
		Local $count = 0
		While $count < 50
			ConsoleWrite("Step4.4.1: ���û����ֹͣ��ťʱ����ֹѭ�� "&@crlf)
			;Step4.4.1: ���û����ֹͣ��ťʱ����ֹѭ��
			If $flag_Interrupt = 1 Then
				$flag_Interrupt = 0
				Return -1
			Endif
			
			;Step4.4.2: �������title��contentΪ�գ���������
			If $index = $IndexTianya Then 
				ConsoleWrite("Step4.4.2: �������title��contentΪ�գ��������� TianYa "&@crlf)
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
				ConsoleWrite("Step4.4.2: �������title��contentΪ�գ��������� No TianYa "&@crlf)
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
				ConsoleWrite("Step4.4.2: �������title��contentΪ�գ��������� $IndexXcht $IndexDqc "&@crlf)
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
				ConsoleWrite("Step4.4.2: �������title��contentΪ�գ��������� No $IndexXcht $IndexDqc "&@crlf)
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
			;Step4.4.3: ���û�ѡ������ȴ�	
			$count = $count + 1
			Sleep(200)
			If $count = 50 Then
				$PressButton = MsgBox(4100, "����", "�Ƿ�������룿�˶Ի��򽫻���10����Զ���ʧ", 10)
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
	;$oIE2 = _IEAttach($AppTitle,"embedded");ToDo:�˳�Ƕ��������
	;_IEQuit($oIE2)
EndFunc

Func search_file($file_index)
	; ��ʾ��ǰĿ¼�µ������ļ����ļ�����ע���з��� "." �� ".."
	$search = FileFindFirstFile(".\tianya\"&$file_index&"*.txt")  

	; ��������Ƿ�ɹ� 
	If $search = -1 Then
		MsgBox(0, "����", "���κ��ļ����ļ�����ָ���������ַ���ƥ��")
		return
	EndIf

	While 1
		$file_name = FileFindNextFile($search) 
		If @error Then ExitLoop
		
		$full_file = String(".\tianya\")
		$full_file &= String($file_name)
		;MsgBox(4096, "�ļ���", $full_file)
		
		
		$file = FileOpen($full_file, 0)

		; ����ļ��Ƿ�������
		If $file = -1 Then
			MsgBox(0, "����", "�޷���Ŀ���ļ���")
			Exit
		EndIf

		; ÿ�ζ�ȡ1���ַ�ֱ���ļ���β��EOF��End-Of-File�� Ϊֹ
		Local $file_content = ""
		While 1
			$line = FileReadLine($file)
			$file_content &= $line
			$file_content &= "\n"
			If @error = -1 Then ExitLoop
			;MsgBox(0, "�������ı���", $line)
		Wend
		;MsgBox(0, "file_content��", $file_content)
		Local $Status = 1
		Local $title = StringMid($file_name,3,StringLen($file_name)-2-4)
		GUICtrlSetData($item32,"������|" & $file_index &"/"& $ArticleNum)
		GUICtrlSetData($item52,"���ӱ���|"& $file_name)
		GUICtrlSetData($item62,"��������|"& $file_content)
				
		$Status = FaTie($SelectFlag,$title,$file_content)
		If $status = -1 Then
			Return $Status
		Endif

		FileClose($file)
	WEnd

	; �ر��������
	FileClose($search)        
EndFunc

Func WM_COMMAND($hWnd, $msg, $wParam, $lParam)

        #forceref $hWnd, $Msg

        $LoWord = BitAND($wParam, 0x0000FFFF)

        If $LoWord = $GUI_Button_STOP Then 
		$flag_Interrupt = 1
		;MsgBox(0, "����", "Stop button is clicked!!")
		Endif
		
        Return $GUI_RUNDEFMSG

EndFunc   ;==>WM_COMMAND