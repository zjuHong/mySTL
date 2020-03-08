#ifndef _MYMAP_H
#define _MYMAP_H
#include "myAllocator.h"
#include "myRBtree.h"
#include "myFunction.h"
template <class Key, class T, class Compare = less<Key>, class Alloc = myAlloc>
class myMap {
public:
    typedef Key key_type;
    typedef T data_type;
    typedef T map_type;
    typedef mySTL::pair<const Key, T> value_type;
    typedef Compare key_compare;
    class value_compare : public mySTL::binary_function<value_type, value_type, bool> {
        friend class myMap<Key, T, Compare, Alloc>;
    protected:
        Compare comp;
        value_compare(Compare c) : comp(c) {}
    public:
        bool operator()(const value_type& x, const value_type& y) const { return comp(x.first, y.first); }
    };

private:
    typedef myRBtree<key_type, value_type, mySTL::select1st<value_type>, key_compare, Alloc> rep_type;
    rep_type t;
public:
    typedef typename rep_type::pointer pointer;
    typedef typename rep_type::const_pointer const_pointer;
    typedef typename rep_type::reference reference;
    typedef typename rep_type::const_pointer const_reference;
    typedef typename rep_type::iterator iterator;

    typedef typename rep_type::const_iterator const_iterator;
    // typedef typename rep_type::reverse_iterator reverse_iterator;
    // typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
    typedef typename rep_type::size_type size_type;
    typedef typename rep_type::difference_type difference_type;

    /****************************构造函数*****************************/
    myMap() : t(Compare()) {}
    explicit myMap(const Compare& comp) : t(comp) {} 
    template<class InputIterator>
    myMap(InputIterator first, InputIterator last) : t(Compare()) { t.insert_unique(first, last); } 
    template<class InputIterator>
    myMap(InputIterator first, InputIterator last, const Compare& comp) : t(comp) { 
        t.insert_unique(first, last); 
    }
    myMap(const myMap<Key, T, Compare, Alloc>& x) : t(x.t) {}

    myMap<Key, Compare, Alloc>& operator=(const myMap<Key, T, Compare, Alloc>& x) {
        t = x.t;
        return *this;
    }

    /****************************成员函数*****************************/
    key_compare key_comp() const { return t.key_comp(); }
    value_compare value_comp() const { return value_compare(t.key_comp()); }
    iterator begin() const { return t.begin(); }
    iterator end() const { return t.end(); }
    bool empty() const { return t.empty(); }
    size_type size() const { return t.size(); }
    size_type max_size() const { return t.max_size(); }

    T& operator[](const key_type& k) {
        return (*((insert(value_type(k, T()))).first)).second;
    }

    typedef mySTL::pair<iterator, bool> pair_iterator_bool;
    mySTL::pair<iterator, bool> insert(const value_type& x) {
        mySTL::pair<typename rep_type::iterator, bool> p = t.insert_unique(x);
        return mySTL::pair<iterator, bool>(p.first, p.second);
    }
    //还有别的insert重载函数待完善

    void clear() { t.clear(); }
    iterator find(const key_type& x) const { return t.find(x); }
    iterator count(const key_type& x) const { return t.count(x); }
    void erase(iterator position) { t.erase(position); }
    void erase(iterator first, iterator last) { t.erase(first, last); }

    inline bool operator==(const myMap<Key, T, Compare, Alloc>& x) {
        return x.t == t;
    }
    inline bool operator<(const myMap<Key, T, Compare, Alloc>& x) {
        return x.t < t;
    }
};
#endif