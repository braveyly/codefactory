//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Design Mode Command
//  @ File Name : MenuItem.cpp
//  @ Date : 2012-3-16
//  @ Author :braveyly
//
//


#include "MenuItem.h"

MenuItem::MenuItem(Command* cmd):Invoker(cmd) {

}

void MenuItem::ClickMenuItem() {
	DoInvoke();
}

