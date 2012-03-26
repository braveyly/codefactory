#ifndef _ADAPTER_H_
#define _ADAPTER_H_
#include "target.h"
#include "adaptee.h"
class ADAPTER: public TARGET, private ADAPTEE
{
public:
    ADAPTER() {};
    virtual void request ( int* a, int* b, int* c, int* d );
};
#endif
