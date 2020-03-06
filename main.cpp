#include "myVector.h"
#include "myList.h"
#include "myDeque.h"
int main() {
    cout << "\nvector:";
    myVector<int> test(9, 0);
    test[0] = 1;
    test[6] = 5;
    test.push_back(10);
    cout << test.size() << endl;
    test.insert(test.begin() + 3, 2, 8);
    test.erase(test.begin() + 3);
    myAlgorithm::swap(test[0], test[1]);
    for (auto iter = test.begin(); iter != test.end(); iter++) {
        cout << *iter << ' ';
    }
    cout << "\nlist:";
    myList<int> list;
    cout << list.size() << endl;
    list.push_back(0);
    list.push_front(1);
    list.push_back(2);
    list.push_back(3);
    list.pop_front();
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
    // __deque_iterator<int, 0> teest;
    // myVector<int>::iterator werwe;
    // myTraits::iterator_category(werwe);
    // for (auto iter = d.begin(); iter != d.end(); iter++) {
    //     cout << *iter << ' ';
    // }  
    system("pause");
    return 0;
}