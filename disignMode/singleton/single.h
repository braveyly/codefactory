#ifndef _SINGLE_H_
#define _SINGLE_H_
class SINGLE
{
private://The 1st highlight
    int test_data;
    static SINGLE* _instance;//The 2nd highlight
    SINGLE();//The 3nd hightlight
public:
    static SINGLE* get_instance();//The 4nd highlight
    void set_test_data ( int new_data );
    void print_test_data();
};
#endif
