#ifndef _MYLIST_H
#define _MYLIST_H
#include "myAlgorithm.h"
#include "myAllocator.h"
#include "myTrait.h"
template<class T>
struct __list_node
{
    typedef void* void_pointer;
    void_pointer prev;
    void_pointer next;
    T data;
};

template<class T, class Ref, class Ptr>
struct __list_iterator
{
public:
    /****************************特性*****************************/
    typedef myTraits::bidirectional_iterator_tag iterator_category;
    typedef __list_iterator<T, T&, T*>  iterator;
    typedef __list_iterator<T, Ref, Ptr>  self;
    typedef T   value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef __list_node<T>* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    link_type node;
    /****************************构造、析构函数*****************************/
    __list_iterator(link_type x) : node(x) {}
    __list_iterator() {}
    __list_iterator(const iterator& x) : node(x.node){}

    /****************************迭代器重载*****************************/
    bool operator==(const self& x) const { return x.node == node; }
    bool operator!=(const self& x) const { return x.node != node; }
    reference operator*() const { return (*node).data; } 
    pointer operator->() const { return &(operator*()); }
    self& operator++() {
        node = (link_type)((*node).next);
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--() {
        node = (link_type)((*node).prev);
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
};

template<class T, class Alloc = myAlloc>
class myList {

protected:
    typedef __list_node<T> list_node;
    typedef simple_alloc<list_node, Alloc> list_node_allocator;
public:
    typedef list_node* link_type;
    typedef __list_iterator<T, T&, T*>  iterator;
    typedef size_t size_type;
    typedef link_type reference;
    typedef T value_type;
    typedef const link_type const_reference;
    /****************************常用成员函数*****************************/
    iterator begin() { return (link_type)((*node).next); }
    iterator end() { return node; }
    bool empty() { return node->next == node; }
    size_type size() {
        return myAlgorithm::distance(begin(), end());
    }
    reference front() { return *begin(); }
    reference back() { return *(--end()); }   
    myList() { empty_initialize(); } 
    void push_back(const T& x) { insert(end(), x); }
    void push_front(const T& x) { insert(begin(), x); }
    void pop_front() { erase(begin()); }
    void pop_back() { 
        iterator tmp = end();
        erase(--tmp);
    }
    void clear() {
        link_type cur = (link_type)node->next;
        while (cur != node) {
            link_type tmp = cur;
            cur = (link_type) cur->next;
            destory_node(tmp);
        }
        node->next = node;
        node->prev = node;
    }
protected:
    link_type node;
    /****************************常用成员函数*****************************/
    link_type get_node() { return list_node_allocator::allocate(); }
    void put_node(link_type p) { list_node_allocator::deallocate(p); }
    link_type creat_node(const T& x) {
        link_type p = get_node();
        construct(&p->data, x);
        return p;
    }
    void destory_node(link_type p) {
        destory(&p->data);
        put_node(p);
    }
    void empty_initialize(void) {
        node = get_node();
        node->next = node;
        node->prev = node;
    }
    iterator insert(iterator position, const T& x) {//返回插入后的下一个位置
        link_type tmp = creat_node(x);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        (link_type(position.node->prev))->next = tmp;
        position.node->prev = tmp;
        return tmp;
    }
    iterator erase(iterator position) {
        link_type next_node = link_type(position.node->next);
        link_type prev_node = link_type(position.node->prev);
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destory_node(position.node);
        return iterator(next_node);
    } 
};

#endif