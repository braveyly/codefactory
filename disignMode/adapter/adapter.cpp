#include "adapter.h"

void ADAPTER::request ( int* a, int* b, int* c, int* d )
{
    Ccord ca, cb;
    this->not_need_request ( &ca, &cb );
    *a = ca.x;
    *b = ca.y;
    *c = cb.x;
    *d = cb.y;
}

