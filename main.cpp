#include "myVector.h"
#include "myList.h"
#include "myDeque.h"
#include "myStack.h"
#include "myQueue.h"
#include "myPriority_queue.h"
#include "mySet.h"
#include "myMap.h"
#include "myUnordered_map.h"
#include "myUnordered_set.h"
#include <iostream>
#include <time.h>
#include <map>
#include "myString.h"
#include "mySmartptr.h"
#include "mySkiplist.h"
using namespace std;

class parent;
class child;
class parent
{
public:
    myWeakPtr<child> ch;
};

class child
{
public:
    mySharePtr<parent> pt;
};

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
    mySTL::sort(test.begin(), test.end(), mySTL::less<int>());
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
    
    int start = clock();
    cout << "\nmap:";
    myMap<int, int> m;
    for (int i = 0; i < 10000; i++)
        m.insert(mySTL::pair<const int, int>(i, i));
    // m[1] = 3;
    // m[8] = 6;
    // m[9] = 8;
    cout << "mymap time is" << clock() - start << endl;
    start = clock();
    map<int, int> sm;
    for (int i = 0; i < 10000; i++)
        sm.insert(std::pair<const int, int>(i, i));

    // cout << m.size() << endl;
    // for (auto iter = m.begin(); iter != m.end(); iter++)
    //     cout << iter->second << ' ';
    cout << "map time is" << clock() - start << endl;

    cout << "\nunoedered_map:";
    myUnordered_map<int, int> um;
    um[1] = 3;
    um[8] = 6;
    um[9] = 8;
    um.erase(um.begin());
    cout << um.size() << endl;
    for (auto iter = um.begin(); iter != um.end(); iter++)
        cout << iter->second << ' ';
        
    cout << "\nunoedered_set:";
    myUnordered_set<int> us;
    us.insert(3);
    us.insert(5);
    us.insert(2);
    cout << us.size() << endl;
    for (auto iter = us.begin(); iter != us.end(); iter++)
        cout << *iter << ' ';

    cout << "\nstring:";
    myString str1("hello ");
    myString str2 = "world";
    myString str3 = str1 + str2;
    cout << "str1->" << str1 << " size->" << str1.length() << endl;
    cout << "str2->" << str2 << " size->" << str2.length() << endl;
    cout << "str3->" << str3 << " size->" << str3.length() << endl;

    cout << "skiplist:";
    mySkipList<int> sl;
    for (int i = 0; i < 10; i++)    
        sl.insert(i);
    sl.erase(5);
    cout << sl.find(5) << ' ' << sl.find(6);

    cout << "\nsmartptr:";
    int *elem = new int(5);
    mySharePtr<int> ms(elem);
    myWeakPtr<int> wi(ms);
    cout << wi.expired() << endl;

    system("pause");
    return 0;
}