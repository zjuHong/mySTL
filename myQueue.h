#ifndef _MYQUEUE_H
#define _MYQUEUE_H
#include "myDeque.h"
template <class T, class Sequence = myDeque<T>>
class myQueue {
public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;
protected:
    Sequence c;
public:
    bool empty() { return c.empty(); }
    size_type size() { return c.size(); }
    reference front() { return c.front(); }
    const_reference front() const { return c.front(); }
    reference back() { return c.back(); }
    const_reference back() const { return c.back(); }
    void push(const value_type& x) { c.push_back(x); }
    void pop() { c.pop_front(); }
    bool operator==(const myQueue& x) { return x.c == c; }
    bool operator<(const myQueue& x) { return c < x.c; }
};
#endif