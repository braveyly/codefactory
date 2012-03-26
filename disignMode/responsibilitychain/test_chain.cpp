#include "Text.h"
#include "Button.h"
#include "Dialog.h"
#include "Application.h"
#include "HelpHandler.h"

int main()
{
	Application* papp = new Application(APPLICATION_HELP_TYPE, NULL);
	Dialog* pdlg = new Dialog(DIALOG_HELP_TYPE, papp);
	//Button* pbt = new Button(BUTTON_HELP_TYPE, NULL);
	Button* pbt = new Button(BUTTON_HELP_TYPE, pdlg);
	Text* ptext = new Text(TEXT_HELP_TYPE,pbt);
	//ptext->print_helper(NONE_HELP_TYPE);
	ptext->print_helper(DIALOG_HELP_TYPE);
}
