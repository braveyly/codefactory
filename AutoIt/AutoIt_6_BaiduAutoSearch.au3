#include <IE.au3>

Local $oIE = _IECreate ("www.baidu.com",0,1,1,0)

_IEPropertySet ($oIE, "toolbar", 0) ;����toolbar
_IEPropertySet ($oIE, "width", 400) ;����ie��800����
_IEPropertySet ($oIE, "height", 400) ;����ie��600����
_IEPropertySet ($oIE, "resizable", 0) ;����ie�����Ե�����С
_IEPropertySet ($oIE, "title","�Ҵ򿪵İٶ�,������") ;����ie����,ȡ��ԭtitle
_IEPropertySet ($oIE, "statustext","��ӭ���� �� autoit3 ���ܽӴ� hi.baidu.com/iokey") ;����״̬��

Local $oForm = _IEFormGetObjByName ($oIE, "f")

Local $oQuery1 = _IEFormElementGetObjByName ($oForm, "wd")
_IEFormElementSetValue ($oQuery1, "�� autoit3 ���ܽӴ�")
_IEFormSubmit ($oForm,0)
