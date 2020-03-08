#include "myVector.h"
#include "myList.h"
#include "myDeque.h"
#include "myStack.h"
#include "myQueue.h"
#include "myPriority_queue.h"
#include "mySet.h"
#include "myMap.h"
int main() {
    cout << "vector:";
    myVector<int> test(9, 0);
    test[0] = 1;
    test[6] = 5;
    test.push_back(10);
    cout << test.size() << endl;
    test.insert(test.begin() + 3, 2, 8);
    test.erase(test.begin() + 3);
    mySTL::swap(test[0], test[1]);
    for (auto iter = test.begin(); iter != test.end(); iter++) {
        cout << *iter << ' ';
    }
    cout << "\nlist:";
    myList<int> list;
    list.push_back(0);
    list.push_front(1);
    list.push_back(2);
    list.push_back(3);
    list.pop_front();
    cout << list.size() << endl;
    for (auto iter = list.begin(); iter != list.end(); iter++) {
        cout << *iter << ' ';
    }    
    cout << "\ndeque:";
    myDeque<int> d(3, 7);
    d.push_back(2);
    d.pop_front();
    d.pop_back();
    d.push_front(23);
    auto i = d.begin() + 2;
    d.erase(i);
    for (auto iter = d.begin(); iter != d.end(); iter++) 
        cout << *iter << ' ';

    cout << "\nstack:";
    myStack<int, myDeque<int>> s;
    s.push(2);
    s.push(3);
    cout << s.top() <<' ' << s.size();
    
    cout << "\nqueue:";
    myQueue<int, myDeque<int>> q;
    q.push(2);
    q.push(3);
    cout << q.front() <<' ' << q.size();

    cout << "\npriority_queue:";
    int ia[9] = {0,1,2,3,4,8,9,5,3};
    myPriority_queue<int> iqa(ia, ia + 9);
    iqa.push(11);
    cout << ' ' << iqa.size() << endl;
    while (!iqa.empty()) {
        cout << iqa.top() << ' ';
        iqa.pop();
    }

    cout << "\nset:";
    mySet<int> set;
    set.insert(3);
    set.insert(5);
    set.insert(2);
    cout << set.size() << endl;
    for (auto iter = set.begin(); iter != set.end(); iter++)
        cout << *iter << ' ';

    cout << "\nmap:";
    myMap<int, int> m;
    m[1] = 3;
    m[8] = 6;
    m[9] = 8;
    cout << m.size() << endl;
    for (auto iter = m.begin(); iter != m.end(); iter++)
        cout << iter->second << ' ';

    system("pause");
    return 0;
}