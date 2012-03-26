#ifndef _FACTORY_H_
#define _FACTORY_H_
class PRODUCT;
class FACTORY
{
public:
    virtual PRODUCT* create_product ( int product_id );
};
#endif
