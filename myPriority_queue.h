#ifndef _MYPRIORITY_QUEQU_H
#define _MYPRIORITY_QUEQU_H
#include "myAlgorithm.h"
#include "myVector.h"
#include "myHeap.h"
template <class T, class Sequence = myVector<T>, class Compare = less<typename Sequence::value_type>>
class myPriority_queue {
public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;
protected:
    
    Compare comp;
public:
Sequence c;
    myPriority_queue() : c() {}
    explicit myPriority_queue(const Compare& x) : c(), comp(x) {}
    template <class InputIterator>
    myPriority_queue(InputIterator first, InputIterator last, const Compare& x) : c(first, last), comp(x) {
        mySTL::make_heap(c.begin(), c.end(), comp);
    }
    template <class InputIterator>
    myPriority_queue(InputIterator first, InputIterator last) : c(first, last) {
        mySTL::make_heap(c.begin(), c.end(), comp);
    }
    bool empty() { return c.empty(); }
    size_type size() { return c.size(); }
    reference top() { return c.front(); }
    const_reference top() const { return c.front(); }
    void push(const value_type& x) { 
        try {
            c.push_back(x); 
            mySTL::push_heap(c.begin(), c.end(), comp);
        }
        catch(...) {
            c.clear();
            throw;
        }
    }
    void pop() { 
        try {
            mySTL::pop_heap(c.begin(), c.end(), comp);
            c.pop_back(); 
        }
        catch(...) {
            c.clear();
            throw;
        }
    }
    bool operator==(const myPriority_queue& x) { return x.c == c; }
    bool operator<(const myPriority_queue& x) { return c < x.c; }
};
#endif