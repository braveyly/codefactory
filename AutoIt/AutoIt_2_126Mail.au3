#include <IE.au3>
$oIE = _IECreate ("httpS://mail.126.com",0,1,1,0)
$oForms = _IEFormGetCollection ($oIE)
;MsgBox(0, "表单信息", "这个页面上共有" & @extended & "个表单") 
For $Form In $oForms
;MsgBox(0, "表单名称", $Form.name)
$oQuerys = _IEFormElementGetCollection ($Form)
For $Query In $oQuerys
;MsgBox(0, "表单控件信息", "名称:"&$Query.name & " 类型:"&$Query.type)
Next
Next
;Sleep(3000)
;Send("{ENTER}")

Local $oForm = _IEFormGetObjByName ($oIE, "login126")
Local $oQuery1 = _IEFormElementGetObjByName ($oForm, "idInput")
_IEFormElementSetValue ($oQuery1, "sjliwei2002")

Local $oQuery1 = _IEFormElementGetObjByName ($oForm, "pwdInput")
_IEFormElementSetValue ($oQuery1, "07167603571liwei")

Local $oQuery = _IEFormElementGetObjByName ($oForm, "loginBtn")
_IEAction($oQuery ,"click")
;sleep(2000)
;_IEAction ($oIE, "visible")

;_IEFormSubmit ($oForm,1)