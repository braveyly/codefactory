#include <GUIConstants.au3>
#include <WindowsConstants.au3>
#include <IE.au3>
$oIE = _IECreateEmbedded ()
GUICreate("Embedded Web control Test", 640, 580, _
        (@DesktopWidth - 640) / 2, (@DesktopHeight - 580) / 2, _
        $WS_OVERLAPPEDWINDOW + $WS_VISIBLE + $WS_CLIPSIBLINGS + $WS_CLIPCHILDREN)
$GUIActiveX = GUICtrlCreateObj($oIE, 20, 40, 600, 360)
$GUI_Button_Back = GUICtrlCreateButton("Back", 10, 420, 100, 30)
$GUI_Button_Forward = GUICtrlCreateButton("Forward", 120, 420, 100, 30)
$GUI_Button_Home = GUICtrlCreateButton("Home", 230, 420, 100, 30)
$GUI_Button_Stop = GUICtrlCreateButton("Stop", 340, 420, 100, 30)

_IENavigate ($oIE, "http://www.baidu.com", 0)
;MsgBox(0, "GUI 事件", "您按下了“还STOP吧”按钮！")
While 1
    $msg = GUIGetMsg()
    Select
        Case $msg = $GUI_EVENT_CLOSE
            ExitLoop
        Case $msg = $GUI_Button_Home
			MsgBox(0, "GUI 事件", "您按下了“还HOME吧”按钮！")
			$oSubmit = _IEGetObjByName ($oIE, "su")
			MsgBox(0, "GUI 事件", $oSubmit.value)
            _IENavigate ($oIE, "http://www.hao123.com",0)
        Case $msg = $GUI_Button_Back
			MsgBox(0, "GUI 事件", "您按下了“还BACK吧”按钮！")
            _IEAction ($oIE, "back")
        Case $msg = $GUI_Button_Forward
			MsgBox(0, "GUI 事件", "您按下了“还FORWARD吧”按钮！")
            _IEAction ($oIE, "forward")
        Case $msg = $GUI_Button_Stop
			MsgBox(0, "GUI 事件", "您按下了“还STOP吧”按钮！")
            _IEAction ($oIE, "stop")
    EndSelect
WEnd




