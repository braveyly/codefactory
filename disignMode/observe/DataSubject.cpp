//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Disign Mode Observe
//  @ File Name : DataSubject.cpp
//  @ Date : 2012-3-19
//  @ Author : braveyly
//
//


#include "DataSubject.h"

void DataSubject::set_data(int data) {
	cout<<"DataSubject::set_data"<<endl;
	_data = data;
	notify();
}

int DataSubject::get_data() {
	cout<<"DataSubject::get_data"<<endl;
	return _data;
}

DataSubject::DataSubject() {

}

