#include "myVector.h"
#include "myList.h"
int main() {
    cout << "hello world!" << endl;
    myVector<int> test(9, 0);
    test[0] = 1;
    test[6] = 5;
    test.push_back(10);
    // test.pop_back();
    cout << test.size() << endl;
    test.insert(test.begin() + 3, 2, 8);
    test.erase(test.begin() + 3);
    myAlgorithm::swap(test[0], test[1]);
    for (auto iter = test.begin(); iter != test.end(); iter++) {
        cout << *iter << ' ';
    }
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
    system("pause");
    return 0;
}