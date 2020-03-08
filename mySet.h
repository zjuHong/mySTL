#ifndef _MYSET_H
#define _MYSET_H
#include "myAllocator.h"
#include "myRBtree.h"
#include "myFunction.h"
template <class Key, class Compare = less<Key>, class Alloc = myAlloc>
class mySet {
public:
    typedef Key key_type;
    typedef Key value_type;
    typedef Compare key_compare;
    typedef Compare value_compare;
private:
    typedef myRBtree<key_type, value_type, mySTL::identity<value_type>, key_compare, Alloc> rep_type;
    rep_type t;
public:
    typedef typename rep_type::const_pointer pointer;
    typedef typename rep_type::const_pointer const_pointer;
    typedef typename rep_type::const_reference reference;
    typedef typename rep_type::const_pointer const_reference;
    typedef typename rep_type::const_iterator iterator;

    typedef typename rep_type::const_iterator const_iterator;
    // typedef typename rep_type::const_reverse_iterator reverse_iterator;
    // typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
    typedef typename rep_type::size_type size_type;
    typedef typename rep_type::difference_type difference_type;

    mySet() : t(Compare()) {}
    explicit mySet(const Compare& comp) : t(comp) {} 
    template<class InputIterator>
    mySet(InputIterator first, InputIterator last) : t(Compare()) { t.insert_unique(first, last); } 
    template<class InputIterator>
    mySet(InputIterator first, InputIterator last, const Compare& comp) : t(comp) { 
        t.insert_unique(first, last); 
    }
    mySet(const mySet<Key, Compare, Alloc>& x) : t(x.t) {}

    mySet<Key, Compare, Alloc>& operator=(const mySet<Key, Compare, Alloc>& x) {
        t = x.t;
        return *this;
    }

    /****************************成员函数*****************************/
    key_compare key_comp() const { return t.key_comp(); }
    value_compare value_comp() const { return t.key_comp(); }
    iterator begin() const { return t.begin(); }
    iterator end() const { return t.end(); }
    bool empty() const { return t.empty(); }
    size_type size() const { return t.size(); }
    size_type max_size() const { return t.max_size(); }

    void clear() { t.clear(); }
    void erase(iterator position) { t.erase(position); }
    void erase(iterator first, iterator last) { t.erase(first, last); }
    
    typedef mySTL::pair<iterator, bool> pair_iterator_bool;
    mySTL::pair<iterator, bool> insert(const value_type& x) {
        mySTL::pair<typename rep_type::iterator, bool> p = t.insert_unique(x);
        return mySTL::pair<iterator, bool>(p.first, p.second);
    }
    //还有别的insert重载函数待完善

    iterator find(const key_type& x) const { return t.find(x); }
    iterator count(const key_type& x) const { return t.count(x); }

    inline bool operator==(const mySet<Key, Compare, Alloc>& x) {
        return x.t == t;
    }
    inline bool operator<(const mySet<Key, Compare, Alloc>& x) {
        return x.t < t;
    }
};
#endif