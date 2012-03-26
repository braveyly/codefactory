#ifndef _ADAPTEE_H_
#define _ADAPTEE_H_

typedef struct CCORD
{
    int x;
    int y;
}Ccord;

class ADAPTEE
{
public:
    ADAPTEE() {};
    virtual void not_need_request ( Ccord* a, Ccord* b );
};
#endif
