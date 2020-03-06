#ifndef _MY_DEQUE_H
#define _MY_DEQUE_H
#include "myAllocator.h"
#include "myAlgorithm.h"
#include "myTrait.h"
template <class T, size_t BufSiz>
struct __deque_iterator
{        
public:
    /****************************特性*****************************/
    // using iterator_category = typename myTraits::random_access_iterator_tag();
    typedef typename myTraits::random_access_iterator_tag iterator_category;
    typedef __deque_iterator<T, BufSiz> iterator;
    typedef __deque_iterator<T, BufSiz> const_iterator;     
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T** map_pointer;
    typedef __deque_iterator self;
    /****************************常用成员函数*****************************/
    T* cur;
    T* first;
    T* last;
    map_pointer node;
    /****************************常用成员函数*****************************/
    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size());
    }
    static size_t buffer_size() { //每个map映射的缓冲区的大小
        size_t sz = sizeof(T);
        return BufSiz != 0 ? BufSiz : (sz < 512 ? size_t(512 / sz) : size_t(1)); 
    }
    /****************************构造函数*****************************/
    __deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
    __deque_iterator(pointer x, map_pointer y) : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
    __deque_iterator(const iterator &rhs) : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
    /****************************运算符重载*****************************/
    reference operator*() const { return *cur; }
    reference operator->() const { return &(operator*()); }
    difference_type operator-(const self& x) const {
        return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur); 
    }
    self& operator++() {
        ++cur;
        if (cur == last) {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--() {
        if (cur == first) {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
    self& operator+=(difference_type n) {
        difference_type offset = n + (cur - first);
        if (offset >= 0 && offset < difference_type(buffer_size()))
            cur += n;
        else {
            difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size())
                : -difference_type((-offset - 1) / buffer_size()) - 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }
    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }
    self& operator-=(difference_type n) const { return *this += -n; }
    self operator-(difference_type n) const {
        self tmp = *this;
        return tmp -= n;
    }
    reference operator[](difference_type n) const { return *(*this + n); }
    bool operator==(const self& x) const { return cur == x.cur; }
    bool operator!=(const self& x) const { return !(*this == x); }
    bool operator<(const self& x) const { return (node == x.node) ? (cur < x.cur) : (node < x.node); }
    bool operator<=(const self& x) const { return x == *this || *this < x; }
};


template <class T, class Alloc = myAlloc, size_t BufSiz = 0>
class myDeque {
public:
    /****************************特性*****************************/
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef pointer* map_pointer;
    typedef myTraits::random_access_iterator_tag iterator_category;
    typedef __deque_iterator<T, BufSiz> iterator;
    typedef simple_alloc<value_type, Alloc> data_allocator;
    typedef simple_alloc<pointer, Alloc> map_allocator;
protected:
    iterator start;
    iterator finish;
    map_pointer map;
    size_type map_size;
    /****************************构造与内存管理*****************************/
    size_type initial_map_size() { return 8;}//一个map最少管理8个节点
    pointer allocate_node() { return data_allocator::allocate(); }
    void deallocate_node(pointer p) { data_allocator::deallocate(p); }
    void creat_map_and_nodes(size_type num_elements) {
        size_type num_nodes = num_elements / iterator::buffer_size() + 1;
        map_size = max(initial_map_size(), num_nodes + 2);
        map = map_allocator::allocate(map_size);
        map_pointer nstart = map + (map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;
        map_pointer cur;
        try {
            for (cur = nstart; cur <= nfinish; ++cur)
                *cur = allocate_node();
        }
        catch(...) {
            while (cur != nstart) {
                --cur;
                deallocate_node(*cur);
                *cur = nullptr;
            }
            throw;
        }
        start.set_node(nstart);
        finish.set_node(nfinish);
        start.cur = start.first;
        finish.cur = finish.first + num_elements % iterator::buffer_size();
    }
    //map初始化
    void fill_initialize(size_type n, const value_type& value) {
        creat_map_and_nodes(n);
        map_pointer cur;
        try {
            for (cur = start.node; cur < finish.node; ++cur)
                myAlgorithm::uninitialized_fill(*cur, *cur + iterator::buffer_size(), value);
            myAlgorithm::uninitialized_fill(finish.first, finish.cur, value);
        }
        catch (...) {
            while (cur != start.node) {
                --cur;
                map_allocator::deallocate(cur, iterator::buffer_size());
                *cur = nullptr;
            }
        }
    }

public:
    /****************************成员函数*****************************/
    iterator begin() { return start; }
    iterator end() { return finish; }
    reference operator[](size_type n) { return start[difference_type(n)]; }
    reference front() { return *start; }
    reference back() {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }
    size_type size() const { return finish - start; }
    size_type max_size() const { return size_type(-1); }
    bool empty() const { return finish == start; }
    /****************************构造与内存管理*****************************/
    myDeque(int n, const value_type& value) : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value);
    }
    myDeque(int n) : start(0), finish(0), map_size(0) {
        fill_initialize(n, value_type());
    }
    ~myDeque() {
        if (map != nullptr) {
            clear();
            *start.node = nullptr;
            map_allocator::deallocate(map, map_size);
            map = nullptr;
        }
    }
    /****************************常用函数*****************************/
    void push_back(const value_type& t) {
        if (finish.cur != finish.last - 1) {
            construct(finish.cur, t);
            ++finish.cur;
        }
        else
            push_back_aux(t);
    }
    void push_front(const value_type& t) {
        if (start.cur != start.first) {
            construct(start.cur - 1, t);
            --start.cur;
        }
        else
            push_front_aux(t);
    }
    void push_back_aux(const value_type& t) {
        value_type t_copy = t;
        reserve_map_at_back();
        *(finish.node + 1) = allocate_node();
        try {
            construct(finish.cur, t_copy);
            finish.set_node(finish.node + 1);
            finish.cur = finish.first;
        }   
        catch(...) {
            deallocate_node(*(finish.node + 1));
        }
    }
    void push_front_aux(const value_type& t) {
        value_type t_copy = t;
        reserve_map_at_front();
        *(start.node - 1) = allocate_node();
        try {
            start.set_node(start.node - 1);
            start.cur = start.last - 1;
            construct(start.cur, t_copy);
        }   
        catch(...) {
            start.set_node(start.node + 1);
            start.cur = start.first;
            deallocate_node(*(start.node - 1));
            throw;
        }
    }
    void reserve_map_at_back(size_type nodes_to_add = 1) {
        if (nodes_to_add + 1 > map_size - (finish.node - map))//尾端节点不足
            reallocate_map(nodes_to_add, false);
    }
    void reserve_map_at_front(size_type nodes_to_add = 1) {
        if (nodes_to_add > size_type(start.node - map))//前端节点不足
            reallocate_map(nodes_to_add, true);
    }
    void reallocate_map(size_type nodes_to_add, bool add_at_front) {
        size_type old_num_nodes = finish.node - start.node + 1;
        size_type new_num_node = old_num_nodes + nodes_to_add;
        map_pointer new_nstart;
        if (map_size > 2 * new_num_node) {
            new_nstart = map + (map_size - new_num_node) / 2 + (add_at_front ? nodes_to_add : 0);
            if (new_nstart < start.node)
                myAlgorithm::copy(start.node, finish.node + 1, new_nstart);
            else
                myAlgorithm::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
        }
        else {//配置新空间
            size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;//两倍
            map_pointer new_map = map_allocator::allocate(new_map_size);
            new_nstart = new_map + (new_map_size - new_num_node) / 2 + (add_at_front ? nodes_to_add : 0);
            myAlgorithm::copy(start.node, finish.node, new_nstart);
            map_allocator::deallocate(map, map_size);
            map = new_map;
            map_size = new_map_size;
        }
        start.set_node(new_nstart);
        finish.set_node(new_nstart + old_num_nodes - 1);
    }
    void pop_back() {
        if (finish.cur != finish.first) {
            --finish.cur;
            destory(finish.cur);
        }
        else
            pop_back_aux();
    }
    void pop_back_aux() {
        deallocate_node(finish.first);
        finish.set_node(finish.node - 1);
        finish.cur = finish.last - 1;
        destory(finish.cur);
    }
    void pop_front() {
        if (start.cur != start.last - 1) {
            destory(start.cur);
            ++start.cur;
        }
        else
            pop_front_aux();
    }
    void pop_front_aux() {
        destory(start.cur);
        deallocate_node(start.first);
        start.set_node(start.node + 1);
        start.cur = start.first;
    }
    iterator erase(iterator pos) {
        iterator next = pos;
        ++next;
        difference_type index = pos - start;
        if (index < difference_type((size() >> 1))) {
            myAlgorithm::copy_backward(start, pos, next);
            pop_front();
        }
        else {
            myAlgorithm::copy(next, finish, pos);
            pop_back();
        }
        return start + index;
    }
    void clear() {
        for (map_pointer node = start.node + 1; node < finish.node; ++node) {
            destory(*node, *node + iterator::buffer_size());
            data_allocator::deallocate(*node, iterator::buffer_size());
        }
        if (start.node != finish.node) {
            destory(start.cur, start.last);
            destory(finish.first, finish.cur);
            data_allocator::deallocate(finish.first, iterator::buffer_size());
        }
        else 
            destory(start.cur, finish.cur);
        finish = start;
    }
    iterator erase(iterator first, iterator last) {
        if (first == start && last == finish) {
            clear();
            return finish;
        }
        else {
            difference_type n = last - first;//清除区间的长度
            difference_type elems_before = first - start;//清除区间前方元素个数
            if (elems_before < (size() - n) / 2) {//前方元素比较少
                myAlgorithm::copy_backward(start, first, last);
                iterator new_start = start + n;
                destory(start, new_start);
                for(map_pointer cur = start.node; cur < new_start.node; ++cur) 
                    data_allocator::deallocate(*cur, iterator::buffersize());
                start = new_start;
            }
            else {
                myAlgorithm::copy(last, finish, first);
                iterator new_finish = finish - n;
                destory(new_finish, finish);
                for(map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur) 
                    data_allocator::deallocate(*cur, iterator::buffersize());
                finish = new_finish;
            }
            return start + elems_before;
        }
    }
    iterator insert(iterator position, const value_type& x) {
        if (position.cur == start.cur) {
            push_front(x);
            return start;
        }
        else if (position.cur == finish.cur) {
            push_back(x);
            iterator tmp = finish;
            --tmp;
            return tmp;
        }  
        else 
            return insert_aux(position, x);
    }
    iterator insert_aux(iterator pos, const value_type& x) {
        difference_type index = pos - start;
        value_type x_copy = x;
        if (index < difference_type(size() / 2)) {//插入点之前的元素比较少
            push_front(front());
            iterator front1 = start;
            ++front1;
            iterator front2 = front1;
            ++front2;
            pos = start + index;
            iterator pos1 = pos;
            ++pos1;
            myAlgorithm::copy(front2, pos1, front1);
        }
        else {
            push_back(back());
            iterator back1 = finish;
            --back1;
            iterator back2 = back1;
            back2--;
            pos = start + index;
            myAlgorithm::copy_backward(pos, back2, back1);
        }
        *pos = x_copy;
        return pos;
    }
};
#endif