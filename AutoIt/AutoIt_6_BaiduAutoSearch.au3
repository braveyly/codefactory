#include <IE.au3>

Local $oIE = _IECreate ("www.baidu.com",0,1,1,0)

_IEPropertySet ($oIE, "toolbar", 0) ;隐藏toolbar
_IEPropertySet ($oIE, "width", 400) ;设置ie宽800像素
_IEPropertySet ($oIE, "height", 400) ;设置ie高600像素
_IEPropertySet ($oIE, "resizable", 0) ;设置ie不可以调整大小
_IEPropertySet ($oIE, "title","我打开的百度,我做主") ;设置ie标题,取代原title
_IEPropertySet ($oIE, "statustext","欢迎访问 与 autoit3 亲密接触 hi.baidu.com/iokey") ;设置状态栏

Local $oForm = _IEFormGetObjByName ($oIE, "f")

Local $oQuery1 = _IEFormElementGetObjByName ($oForm, "wd")
_IEFormElementSetValue ($oQuery1, "与 autoit3 亲密接触")
_IEFormSubmit ($oForm,0)
