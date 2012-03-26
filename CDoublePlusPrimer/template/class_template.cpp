#include <iostream>
using namespace::std;

template<typename T> class Queue{
	public:
		Queue();
		T &front();
		void push(const T&);
};

template<typename T> Queue<T>::Queue()
{
	cout<<"Queue::Queue()"<<endl;
}

template<typename T> T& Queue<T>::front()
{
	T* tmp = new T;
	return *tmp;
}

template<typename T> void Queue<T>::push(const T& a)
{
	cout<<a<<endl;
}

int main()
{
	Queue<int> queue;
	cout<<queue.front()<<endl;
	queue.push(3);
}

