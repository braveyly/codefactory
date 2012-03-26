#include "common_def.h"
#include "Glyph_Factory.h"
#include "Row.h"
#include "Charactor.h"

int main()
{
	FONT ft;
	POSITION pn;
	ft.color = 1;
	ft.size = 2;
	pn.x =3;
	pn.y =4;
	
	Glyph_Factory* fc = new Glyph_Factory();
	Charactor* pch1 = fc->get_charactor(1);
	pch1->draw(&ft,&pn);

	Charactor* pch2 = fc->get_charactor(1);
	pch2->draw(&ft,&pn);
	
	Charactor* pch3 = fc->get_charactor(4);
	pch3->draw(&ft,&pn);

	Charactor* pch4 = fc->get_charactor(1);
	pch4->draw(&ft,&pn);
	
	Row* rw1 = fc->get_row(3);
	string res;
	rw1->add_glyph(pch1);
	rw1->add_glyph(pch2);
	rw1->add_glyph(pch3);
	res = rw1->add_glyph(pch3)?"failed":"succeed";
	cout<< res <<endl;
	res = rw1->add_glyph(pch4)?"failed":"succeed";
	cout<< res <<endl;
	rw1->draw(&ft, &pn);
}
