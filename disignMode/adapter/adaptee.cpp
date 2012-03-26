#include "adaptee.h"

void ADAPTEE::not_need_request ( Ccord* a, Ccord* b )
{
    ( *a ).x = 1;
    ( *a ).y = 2;
    ( *b ).x = 3;
    ( *b ).y = 4;
}

