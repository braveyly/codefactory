//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Design Mode
//  @ File Name : Row.h
//  @ Date : 2012-3-15
//  @ Author : 
//
//


#if !defined(_ROW_H)
#define _ROW_H

#include "Glyph.h"

class Row : public Glyph {
public:
	void draw(FONT*, POSITION*);
	int add_glyph(Glyph*);
	Row(int);
private:
	list<Glyph*> _list_gph;
	int _total;
};

#endif  //_ROW_H
