#ifndef _MYUNOEREDER_MAP_H
#define _MYUNOEREDER_MAP_H
#include "myAllocator.h"
#include "myHashTable.h"
#include "myFunction.h"
template <class Key, class Value, class HashFcn, class EqualKey, class Alloc> class myUnordered_map;

template <class Key, class Value, class HashFcn, class EqualKey, class Alloc>
inline bool operator==(const myUnordered_map<Key, Value, HashFcn, EqualKey, Alloc> &,
                        const myUnordered_map<Key, Value, HashFcn, EqualKey, Alloc> &);

template <class Key, class Value, class HashFcn = hash<Value>, class EqualKey = equal_to<Value>,
            class Alloc = myAlloc>
class myUnordered_map {
    template <class _Key, class _Value, class _HashFcn, class _EqualKey, class _Alloc>
    friend bool operator==(const myUnordered_map<_Key, _Value, _HashFcn, _EqualKey, _Alloc> &,
                            const myUnordered_map<_Key, _Value, _HashFcn, _EqualKey, _Alloc> &);

/****************************数据结构*****************************/
private: 
    typedef myHashTable<mySTL::pair<const Key, Value>, Key, HashFcn, mySTL::select1st<mySTL::pair<const Key, 
        Value>>, EqualKey, Alloc> ht; 
    ht rep; //底层数据结构

/****************************特性*****************************/
public: 
    typedef typename ht::key_type key_type;
    typedef Value data_type;
    typedef Value mapped_type;
    typedef typename ht::reference reference;
    typedef typename ht::value_type value_type;
    typedef typename ht::hasher hasher;
    typedef typename ht::key_equal key_equal;
    typedef typename ht::iterator iterator;
    typedef typename ht::const_iterator const_iterator;
    typedef typename ht::size_type size_type;

    /****************************成员函数*****************************/
    hasher hash_funct() const noexcept { return rep.hash_funct(); }
    key_equal key_eq() const noexcept { return rep.key_eq(); }
    size_type size() const noexcept { return rep.size(); }
    size_type max_size() const noexcept { return rep.max_size(); }
    bool empty() const { return rep.empty(); }
    iterator begin() { return rep.begin(); }
    iterator end() { return rep.end(); }
    void swap(myUnordered_map &rhs) noexcept { rep.swap(rhs.rep); }
    void resize(size_type n) { rep.resize(n); }

    /****************************构造函数*****************************/
    myUnordered_map() : rep(100, hasher(), key_equal()) {}
    explicit myUnordered_map(size_type n) : rep(n, hasher(), key_equal()) {}
    myUnordered_map(size_type n, const hasher &hf) : rep(n, hf, key_equal()) {}
    myUnordered_map(size_type n, const hasher &hf, const key_equal &eql) : rep(n, hf, eql) {}
    template <class InputIterator>
    myUnordered_map(InputIterator first, InputIterator last) : rep(100, hasher(), key_equal()) {
        rep.insert_unique(first, last);
    }
    template <class InputIterator>
    myUnordered_map(InputIterator first, InputIterator last, size_type n) : rep(n, hasher(), key_equal()) {
        rep.insert_unique(first, last);
    }
    template <class InputIterator>
    myUnordered_map(InputIterator first, InputIterator last, size_type n, const hasher &hf) : rep(n, hf, key_equal()) {
        rep.insert_unique(first, last);
    }
    template <class InputIterator>
    myUnordered_map(InputIterator first, InputIterator last, size_type n, const hasher &hf, const key_equal &eql)
        : rep(n, hf, eql) {
        rep.insert_unique(first, last);
    }

    /****************************基本函数*****************************/
    pair<iterator, bool> insert(const value_type &obj) { return rep.insert_unique(obj); }
    template <class InputIterator> void insert(InputIterator first, InputIterator last) {
        rep.insert_unique(first, last);
    }
    data_type &operator[](const key_type &key) {
        return rep.find_or_insert(value_type(key, data_type())).second; // 见Effective STL Item24
    }

    size_type erase(const key_type &key) { return rep.erase(key); }
    void erase(iterator it) { rep.erase(it); }
    void erase(iterator f, iterator l) { rep.erase(f, l); }
    void clear() { rep.clear();}

    iterator find(const key_type &key) { return rep.find(key); }
    const_iterator find(const key_type &key) const { return rep.find(key); }
    size_type count(const key_type &key) const { return rep.count(key); }
    pair<iterator, iterator> equal_range(const key_type &key) { return rep.equal_range(key); }
    pair<const_iterator, const_iterator> equal_range(const key_type &key) const {
        return rep.equal_range(key);
    }
};

template <class Key, class Value, class HashFcn, class EqualKey, class Alloc>
inline bool operator==(const myUnordered_map<Key, Value, HashFcn, EqualKey, Alloc> &lhs,
                        const myUnordered_map<Key, Value, HashFcn, EqualKey, Alloc> &rhs) {
    return lhs.rep == rhs.rep;
}

template <class Key, class Value, class HashFcn, class EqualKey, class Alloc>
inline bool operator!=(const myUnordered_map<Key, Value, HashFcn, EqualKey, Alloc> &lhs,
                        const myUnordered_map<Key, Value, HashFcn, EqualKey, Alloc> &rhs) {
    return !(lhs == rhs);
}

template <class Key, class Value, class HashFcn, class EqualKey, class Alloc>
inline void swap(const myUnordered_map<Key, Value, HashFcn, EqualKey, Alloc> &lhs,
                    const myUnordered_map<Key, Value, HashFcn, EqualKey, Alloc> &rhs) {
    lhs.swap(rhs);
}
#endif