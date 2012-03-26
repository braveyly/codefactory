#ifndef _DIRECTOR_H_
#define _DIRECTOR_H_
#include "builder.h"

class DIRECTOR
{
private:
    BUILDER* _bld;
public:
    void construct();
    DIRECTOR ( BUILDER* bld );
};
#endif
