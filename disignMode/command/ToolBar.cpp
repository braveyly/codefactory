//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Design Mode Command
//  @ File Name : ToolBar.cpp
//  @ Date : 2012-3-16
//  @ Author :braveyly
//
//


#include "ToolBar.h"

ToolBar::ToolBar(Command* cmd):Invoker(cmd) {

}

void ToolBar::ClickToolBar() {
	DoInvoke();
}

