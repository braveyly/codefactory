#include <iostream>
#include <vector>
#include <list>
#include <deque>

using namespace::std;

vector<int> vec;
vector<int> vec2;
list<int> ls;
deque<int> de;

void normal_traversal()
{
    cout << "begin to traversal vector" << endl;
    vector<int>::iterator vec_iter;
    for ( vec_iter = vec.begin(); vec_iter != vec.end(); vec_iter++ )
        cout << *vec_iter << endl;
    cout << "end to traversal vector" << endl;

    cout << "begin to traversal list" << endl;
    list<int>::iterator ls_iter;
    for ( ls_iter = ls.begin(); ls_iter != ls.end(); ls_iter++ )
        cout << *ls_iter << endl;
    cout << "end to traversal list" << endl;

    cout << "begin to traversal deque" << endl;
    deque<int>::iterator de_iter;
    for ( de_iter = de.begin(); de_iter != de.end(); de_iter++ )
        cout << *de_iter << endl;
    cout << "end to traversal list" << endl;
}

void normal_insert_by_push_back() //insert the element at the end of sequence container
{
    vec.push_back ( 1 );  //return is void
    ls.push_back ( 11 );
    de.push_back ( 111 );
}

void normal_insert_by_insert() //insert the element at front of the iterator
{
    vector<int>::iterator vec_iter = vec.begin();
    list<int>::iterator ls_iter = ls.begin();
    deque<int>::iterator de_iter = de.begin();

    vec_iter = vec.insert ( vec_iter, 3 );//return is the iterator of newly insertion element
    ls_iter = ls.insert ( ls_iter, 33 );
    de_iter = de.insert ( de_iter, 333 );
    cout << "normal_insert_by_insert" << *vec_iter << " " << *ls_iter << " " << *de_iter << endl;
}

void normal_access()
{
    cout << vec.back() << " " << vec.front() << endl;//return is the reference of the first and last elements
    cout << ls.back() << " " << ls.front() << endl;
    cout << de.back() << " " << de.front() << endl;
}

void normal_size() //for vector list and deque
{
    vector<int>::size_type size = vec.size();
    vector<int>::size_type capacity = vec.capacity();
    vector<int>::size_type max_size = vec.max_size();
    cout << "size=" << size << "capacity" << capacity << "max-size" << max_size << endl;
}

void normal_iterator_operation()
{
    vector<int>::iterator iter_begin = vec.begin();
    vector<int>::iterator iter_end = vec.end();
    /*
     list<int>::iterator iter_begin = ls.begin();
     list<int>::iterator iter_end = ls.end();
     deque<int>::iterator iter_begin = de.begin();
     deque<int>::iterator iter_end = de.end();
    */
    if ( iter_begin == iter_end )
        cout << "sequence container number is 0" << endl;

    ++iter_begin;
    if ( iter_begin == iter_end )
        cout << "sequence container number is 1" << endl;

    ++iter_begin;
    if ( iter_begin == iter_end )
        cout << "sequence container number is 2" << endl;

    ++iter_begin;
    if ( iter_begin == iter_end )
        cout << "sequence container number is 3" << endl;

}

void normal_erase ( int type ) //for vector list and deque
{
    if ( 1 == type )
    {
        vector<int>::iterator vec_iter = vec.begin();
        vec_iter = vec.erase ( vec_iter );//return is the iterator after the deleting element
        cout << "the element after the deletion one is " << *vec_iter << endl;
    }

    if ( 2 == type )
        vec.pop_back();//delete the last element, return is void

    if ( 3 == type )
        vec.clear();//delete all elements

    if ( 4 == type )
    {
        vector<int>::iterator iter_begin = vec.begin();
        vector<int>::iterator iter_end = vec.end();
        vec.erase ( iter_begin, iter_end );//return the iterator after multiple deleting elements
    }
}

void normal_assign()
{
    vec2 = vec;//1
    vector<int>::iterator vec_iter;
    for ( vec_iter = vec2.begin(); vec_iter != vec2.end(); vec_iter++ )
        cout << *vec_iter << endl;
    cout << "end to traversal vector 2" << endl;
	
    vec2.swap ( vec );//2
    for ( vec_iter = vec2.begin(); vec_iter != vec2.end(); vec_iter++ )
        cout << *vec_iter << endl;
    cout << "end to traversal vector 2" << endl;
	
    vec2.assign ( vec.begin(), vec.end() );//3
    for ( vec_iter = vec2.begin(); vec_iter != vec2.end(); vec_iter++ )
        cout << *vec_iter << endl;
    cout << "end to traversal vector 2" << endl;
	
    vec2.assign ( 10, 3 );//4
    for ( vec_iter = vec2.begin(); vec_iter != vec2.end(); vec_iter++ )
        cout << *vec_iter << endl;
    cout << "end to traversal vector 2" << endl;
	
}

void special_traversal()
{
    int vec_size = vec.size();
    int de_size = de.size();
    int i ;
    cout << "special traversal of vector by at" << endl;
    for ( i = 0; i < vec_size; i++ ) //the reference of the responding element
        cout << vec.at ( i ) << endl;
    cout << "special traversal of deque by []" << endl;
    for ( i = 0; i < de_size; i++ )
        cout << de[i] << endl;
}

void special_insert()
{
    ls.push_front ( 22 );//return is void
    de.push_front ( 222 );
}

void special_access()
{
    cout << "special access of vector by at" << endl;
    cout << vec.at ( 0 ) <<" "<< vec.at ( 1 ) << endl;
    cout << "special access of deque by []" << endl;
    cout << de[0] <<" "<< de[1] << endl;
}

void special_iterator_operation() //only for vector and deque
{
    vector<int>::iterator iter_begin = vec.begin();
    vector<int>::iterator iter_end = vec.end();
    vector<int>::size_type size = vec.size();
    /*
     deque<int>::iterator iter_begin = de.begin();
     deque<int>::iterator iter_end = de.end();
     deque<int>::size_type size = de.size();
    */
    cout << "middle of vector/deque is " << * ( iter_begin + size / 2 ) << endl;
    if ( iter_begin < iter_end )
        cout << "iter_begin<iter_end" << endl;
    else if ( iter_begin == iter_end )
        cout << "iter_begin == iter_end" << endl;
    else
        cout << "iter_begin > iter_end" << endl;
}

void special_erase() //only for list and deque
{
    ls.pop_front();
    de.pop_front();
}

int main()
{
    normal_iterator_operation();

    normal_insert_by_push_back();
    normal_insert_by_insert();
    special_insert();
	
    normal_traversal();
	special_traversal();

    normal_size();

    normal_access();
	special_access();

    normal_iterator_operation();
	special_iterator_operation();

	normal_assign();

	normal_erase(1);
	special_erase();
	
	normal_traversal();
}
