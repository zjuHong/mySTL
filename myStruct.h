#ifndef _MYSTRUCT_H
#define _MYSTRUCT_H
template <class T1, class T2>
inline void construct(T1* p, T2& value) {
    new (p) T1(value);
}

template <class T>
inline void destroy(T *pointer) {
    pointer->~T();
}
template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    for (; first != last; ++first)
        destroy(&*first);
}   
// struct __true_type { };
// struct __false_type { };
#endif