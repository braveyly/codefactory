#ifndef _PROTOTYPE_H_
#define _PROTOTYPE_H_
class PROTOTYPE
{
public:
    PROTOTYPE() {};
    PROTOTYPE ( PROTOTYPE& new_one ) {};
    virtual PROTOTYPE* clone() = 0;
    virtual void print_data() = 0;
    virtual void set_data ( int data ) = 0;

};
#endif
