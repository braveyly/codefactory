//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Design Mode Mediator
//  @ File Name : Mediator.h
//  @ Date : 2012-3-19
//  @ Author : braveyly
//
//


#if !defined(_MEDIATOR_H)
#define _MEDIATOR_H


class Mediator {
public:
	Mediator();
	virtual void doActionFromAToB()=0;
	virtual void doActoinFromBToA()=0;
};

#endif  //_MEDIATOR_H
