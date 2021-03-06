//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Disign Mode Observe
//  @ File Name : Subject.cpp
//  @ Date : 2012-3-19
//  @ Author : braveyly
//
//


#include "Subject.h"

void Subject::attach_observe(Observe* sub) {
	_observe_list.push_back(sub);
}

void Subject::deattach_observe(Observe* sub) {
	if(_observe_list.size())
		_observe_list.remove(sub);
}

void Subject::notify() {
	list<Observe*>::iterator iter = _observe_list.begin();
	if(_observe_list.size())
		for( iter; iter != _observe_list.end(); iter++ )
		{
			(*iter)->show_view();
		}
}

