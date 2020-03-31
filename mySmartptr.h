#ifndef _MY_SMART_POINTER
#define _MY_SMART_POINTER
#include <stddef.h>
#include <stdlib.h>
class Counter
{
public:
    Counter() : s(0), w(0){};
    int s; //share_ptr计数
    int w; //weak_ptr计数
};

template <class T>
class myWeakPtr; //前置声明，为了用weak_ptr的lock()，来生成share_ptr用，需要拷贝构造用

template <class T>
class mySharePtr
{
public:
    /****************************构造析构函数*****************************/
    mySharePtr(T *p = 0) : _ptr(p)
    {
        cnt = new Counter();
        if (p)
            cnt->s = 1;
    }
    ~mySharePtr() { release(); }
    mySharePtr(mySharePtr<T> const &s)
    {
        _ptr = s._ptr;
        (s.cnt)->s++;
        cnt = s.cnt;
    }
    mySharePtr(myWeakPtr<T> const &w) //为了用weak_ptr的lock()，来生成share_ptr用，需要拷贝构造用
    {
        _ptr = w._ptr;
        (w.cnt)->s++;
        cnt = w.cnt;
    }
    mySharePtr<T> &operator=(mySharePtr<T> &s)
    {
        if (this != &s)
        {
            release();//把原指针先处理
            (s.cnt)->s++;
            cnt = s.cnt;
            _ptr = s._ptr;
        }
        return *this;
    }
    T &operator*() { return *_ptr; }
    T *operator->() { return _ptr; }
    friend class myWeakPtr<T>; //方便weak_ptr与share_ptr设置引用计数和赋值。

private:
    void release()
    {
        cnt->s--;
        if (cnt->s < 1)//强引用计数为零就清理指针对象内存
        {
            delete _ptr;
            if (cnt->w < 1)//弱引用为零才回收计数器内存
            {
                delete cnt;
                cnt = NULL;
            }
        }
    }
    T *_ptr;
    Counter *cnt;
};

template <class T>
class myWeakPtr
{
public: 
    /****************************构造析构函数*****************************/
    //WeakPtr不会new一个cnt
    myWeakPtr()
    {
        _ptr = 0;
        cnt = 0;
    }
    myWeakPtr(mySharePtr<T> &s) :  _ptr(s._ptr), cnt(s.cnt)
    {
        cnt->w++;
    }

    myWeakPtr(myWeakPtr<T> &w) : _ptr(w._ptr), cnt(w.cnt)
    {
        cnt->w++;
    }
    ~myWeakPtr() { release(); }
    myWeakPtr<T> &operator=(myWeakPtr<T> &w)
    {
        if (this != &w)
        {
            release();
            cnt = w.cnt;
            cnt->w++;
            _ptr = w._ptr;
        }
        return *this;
    }
    myWeakPtr<T> &operator=(mySharePtr<T> &s)
    {
        release();
        cnt = s.cnt;
        cnt->w++;
        _ptr = s._ptr;
        return *this;
    }

    mySharePtr<T> lock() { return mySharePtr<T>(*this); }

    bool expired()
    {
        if (cnt)
        {
            if (cnt->s > 0)
            {
                return false;
            }
        }
        return true;
    }
    friend class mySharePtr<T>; //方便weak_ptr与share_ptr设置引用计数和赋值。

private:
    void release()
    {
        if (cnt)
        {
            cnt->w--;
            if (cnt->w < 1 && cnt->s < 1)
            {
                // free(cnt);
                delete cnt;
                cnt = NULL;
            }
        }
    }
    T *_ptr;
    Counter *cnt;
};

#endif