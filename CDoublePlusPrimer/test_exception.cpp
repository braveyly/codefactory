#include <iostream>
#include <stdexcept>
#include <exception>

using namespace::std;

double divide ( double x, double y ) throw ( exception )
{
    if ( 0 == y )
    {
        throw y + 23;
    }
    return x / y;
}

double divide_exception ( double x, double y )
{
    if ( 0 == y )
    {
        throw exception();
    }
    return x / y;
}

double divide_bad_alloc ( double x, double y )
{
    if ( 0 == y )
    {
        throw bad_alloc();
    }
    return x / y;
}

double divide_bad_cast ( double x, double y )
{
    if ( 0 == y )
    {
        throw bad_cast();
    }
    return x / y;
}

double divide_runtime_error ( double x, double y )
{
    if ( 0 == y )
    {
        throw runtime_error ( "123" );
    }
    return x / y;
}

double divide_limit ( double x, double y ) throw ( exception )
{
    if ( 0 == y )
    {
        throw y + 23;
    }
    return x / y;
}

double divide_no_throw ( double x, double y ) throw()
{
    if ( 0 == y )
    {
        throw y + 23;
    }
    return x / y;
}

int main()
{
    try
    {
        //cout<<divide(8.2,2)<<endl;
        cout << divide ( 8, 0 ) << endl;
        cout << divide_runtime_error ( 9, 0 ) << endl;
    }
    catch ( double& d )
    {
        cerr << "double error of divide zero:" << d << endl;
    }
    catch ( bad_alloc& e )
    {
        cerr << "bad_alloc error of divide zero:" << endl;
    }
    catch ( bad_cast& e )
    {
        cerr << "bad_cast error of divide zero:" << endl;
    }
    catch ( runtime_error& e )
    {
        cerr << "runtime_error error of divide zero:" << e.what() << endl;
    }
    catch ( exception& e )
    {
        cerr << "exception error of divide zero:" << endl;
    }
}
