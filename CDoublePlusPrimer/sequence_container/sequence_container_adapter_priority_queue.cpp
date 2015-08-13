#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <string>

using namespace::std;


void test_priority_queue_1()//
{
	const int MAX = 5;
	int a[MAX] = {2, 5, 3, 1, 4};
    priority_queue<int> q;
    for (int i=0; i<MAX; i++)
    {
        q.push(a[i]);
    }
    while (!q.empty())
    {
        int t = q.top();
        q.pop();
        cout << t << "  " ;
    }
    cout << endl;
}


 
struct cmp
{
    bool operator() (const int a, const int b)
    {
        return a > b;
    }
};
 
void test_priority_queue_2()
{
	const int MAX = 5;
	int a[MAX] = {2, 5, 3, 1, 4};
    priority_queue<int, vector<int>, cmp> q;
    for (int i=0; i<MAX; i++)
    {
        q.push(a[i]);
    }
    while (!q.empty())
    {
        int t = q.top();
        q.pop();
        cout << t << "  " ;
    }
    cout << endl;
} 

struct NODE
{
    int value;
    int priority;
	/*
    friend bool operator<(struct NODE n1, struct NODE n2) 
    {
        return n1.priority < n2.priority;
    }
    */

    bool operator<(const struct NODE n2) const
    {
        return priority < n2.priority;
    }
};
 
void test_priority_queue_3()
{
	const int MAX = 5;
	int value[MAX] = {2, 5, 3, 1, 4};
	int priority[MAX] = {2, 1, 3, 5, 4};
    priority_queue<struct NODE> q;
    struct NODE node;
	
    for (int i=0; i<MAX; i++)
    {
        node.priority = priority[i];
        node.value = value[i];
        q.push(node);
    }
    while (!q.empty())
    {
        node = q.top();
        q.pop();
        cout << node.value << "  " ;
    }
    cout << endl;
}

int main()
{
   test_priority_queue_3();
}