#ifndef _MYSTACK_H
#define _MYSTACK_H
#include "myDeque.h"
template <class T, class Sequence = myDeque<T>>
class myStack {
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
    reference top() { return c.back(); }
    const_reference top() const { return c.back(); }
    void push(const value_type& x) { c.push_back(x); }
    void pop() { c.pop_back(); }
    bool operator==(const myStack& x) { return x.c == c; }
    bool operator<(const myStack& x) { return c < x.c; }
};
#endif