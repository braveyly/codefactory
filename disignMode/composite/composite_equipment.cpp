#include "composite_equipment.h"

int COMPOSITE_EQUIPMENT::power()
{
    list<EQUIPMENT*>::iterator begin, end, iter;
	int power_total = 0;
    begin = CreateIterator ( &end );
    for ( iter = begin; iter != end; iter++ )
    {
        power_total += ( *iter )->power();
    }
    return power_total;
}

int COMPOSITE_EQUIPMENT::net_price()
{
    list<EQUIPMENT*>::iterator begin, end, iter;
	int net_price_total = 0;
    begin = CreateIterator ( &end );
    for ( iter = begin; iter != end; iter++ )
    {
        net_price_total += ( *iter )->net_price();
    }
    return net_price_total;
}

int COMPOSITE_EQUIPMENT::discount_price()
{
    list<EQUIPMENT*>::iterator begin, end, iter;
	int discount_price_total = 0;
    begin = CreateIterator ( &end );
    for ( iter = begin; iter != end; iter++ )
    {
        discount_price_total += ( *iter )->discount_price();
    }
    return discount_price_total;	
}

void COMPOSITE_EQUIPMENT::add ( EQUIPMENT* equip )
{
    _equipment.push_back ( equip );
}

void COMPOSITE_EQUIPMENT::remove ( EQUIPMENT* equip )
{
    _equipment.remove ( equip );
}

list<EQUIPMENT*>::iterator COMPOSITE_EQUIPMENT::CreateIterator ( list<EQUIPMENT*>::iterator *end )
{
    *end = _equipment.end();
    return _equipment.begin();
}
