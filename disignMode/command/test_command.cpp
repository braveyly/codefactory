#include "Document.h"
#include "Picture.h"
#include "CopyCommand.h"
#include "PasteCommand.h"
#include "MenuItem.h"
#include "ToolBar.h"

int main()
{
	Document* pdoc = new Document();//Create one document object
	CopyCommand* pcpcmd = new CopyCommand(pdoc);//Create one copy operation for document

	ToolBar* ptb = new ToolBar(pcpcmd);//create one invoker on tool bar for copy operation of document
	ptb->ClickToolBar();//Do it.
}
