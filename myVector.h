#ifndef _MYVECTOR_H
#define _MYVECTOR_H
#include "myAllocator.h"
#include "myAlgorithm.h"
template<class T, class Alloc = myAlloc>
class myVector
{
public:
    /****************************特性*****************************/
    typedef T   value_type;
    typedef value_type* pointer;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef myTraits::random_access_iterator_tag iterator_category;

    /****************************常用成员函数*****************************/
    iterator begin() { return start; }
    iterator end() { return finish; }
    size_type size() const { return size_type(finish - start); }
    size_type capacity() const { return size_type(end_of_storage - start); }
    bool empty() { return begin() == end(); }
    reference front() { return *begin(); }
    reference back() { return *(end() - 1); }
    reference operator[](size_type n) { return *(begin() + n); }
    
    /****************************构造、析构函数*****************************/
    myVector() : start(0), finish(0), end_of_storage(0) {} 
    myVector(size_type n, const T& value) { fill_initialize(n, value); }
    myVector(int n, const T& value) { fill_initialize(n, value); }
    myVector(long n, const T& value) { fill_initialize(n, value); }
    explicit myVector(size_type n) { fill_initialize(n, T()); }
    ~myVector() { 
        destory(start, finish);
        deallocate();
    }
    /****************************常用成员函数*****************************/
    void push_back(const T& x) {
        if (finish != end_of_storage) {
            construct(finish, x);
            ++finish;
        }
        else
            insert_aux(end(), x);
    }
    void pop_back() {
        --finish;
        destory(finish);
    }
    iterator erase(iterator position) {
        if (position + 1 != end())
            copy(position + 1, finish, position);
        --finish;
        destory(finish);
        return position;
    }
    iterator erase(iterator first, iterator last) {
        iterator i = copy(last, finish, first);
        destory(i, finish);
        finish = finish - (last - first);
        return first;
    }
    void resize(size_type new_size, const T& x) {
        if (new_size < size()) 
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), x); 
    }
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear() { erase(begin(), end()); }
    void insert(iterator position, size_type n, const T& x);
    void insert_aux(iterator position, const T& x);

protected:
    typedef simple_alloc<T, Alloc> data_allocator;
    iterator start;
    iterator finish;
    iterator end_of_storage;
    /****************************内存相关成员函数*****************************/
    iterator allocate_and_fill(size_type n, const T& x) {
        iterator result = data_allocator::allocate(n);
        myAlgorithm::uninitialized_fill_n(result, n, x);
        return result;
    }
    void fill_initialize(size_type n, const T& value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }
    void deallocate() {
        data_allocator::deallocate(start, capacity());
    }
};
/****************************成员的插入函数*****************************/
template <class T, class Alloc>
void myVector<T, Alloc>:: insert_aux(iterator position, const T& x) {
    if (finish != end_of_storage) {
        construct(finish, *(finish - 1));
        ++finish;
        T xCopy = x;
        myAlgorithm::copy_backward(position, finish - 2, finish - 1);
        *position = xCopy;
    }
    else {
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;
        iterator new_start = data_allocator::allocate(len);
        iterator new_finish = new_start;
        try {
            new_finish = myAlgorithm::uninitialized_copy(start, position, new_start);
            construct(new_finish, x);
            ++new_finish;
            new_finish = myAlgorithm::uninitialized_copy(position, finish, new_finish);
        }
        catch(...) {
            destory(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }
        destory(begin(), end());
        deallocate();

        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}

template <class T, class Alloc>
void myVector<T, Alloc>::insert(iterator position, size_type n, const T& x) {
    if (n == 0) return;
    if (size_type(end_of_storage - finish) >= n) {
        T xCopy = x;
        const size_type elems_after = finish - position;
        iterator old_finish = finish;
        if (elems_after > n) {//分两次复制的目的是防止交叉的部分被覆盖
            myAlgorithm::uninitialized_copy(finish - n, finish, finish);
            finish += n;
            myAlgorithm::copy_backward(position, position + n, old_finish);
            myAlgorithm::fill(position, position + n, xCopy);
        }
        else {
            myAlgorithm::uninitialized_fill_n(finish, n - elems_after, xCopy);
            finish += n;
            myAlgorithm::uninitialized_copy(position, old_finish, finish);
            finish += elems_after;
            myAlgorithm::fill(position, old_finish, xCopy);
        }
    }
    else {
        const size_type old_size = size();
        const size_type len = old_size + max(old_size, n);
        iterator new_start = data_allocator::allocate(len);
        iterator new_finish = new_start;
        try {
            new_finish = myAlgorithm::uninitialized_copy(start, position, new_start);
            new_finish = myAlgorithm::uninitialized_fill_n(new_finish, n, x);
            new_finish = myAlgorithm::uninitialized_copy(position, finish, new_finish);
        }
        catch(...) {
            destory(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }
        destory(begin(), end());
        deallocate();

        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}
#endif