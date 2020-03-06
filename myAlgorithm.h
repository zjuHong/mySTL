#ifndef _MYALGORITHM_H
#define _MYALGORITHM_H
#include "myStruct.h"
#include <cstring>
#include "myTrait.h"
namespace myAlgorithm{
/*****************************************底层算法***************************************/
    template<class ForwardIterator, class Size, class T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
        ForwardIterator cur = first;
        for (; n > 0; --n, ++cur) 
            construct(&*cur, x);
        return cur;
    }

    template<class ForwardIterator, class T>
    inline void fill(ForwardIterator first, ForwardIterator last, const T& x) {
        ForwardIterator cur = first;
        for (; cur != last; ++cur) 
            construct(&*cur, x);
    }

    template<class ForwardIterator, class T>
    inline ForwardIterator uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
        ForwardIterator cur = first;
        for (; cur != last; ++cur) 
            construct(&*cur, x);
        return cur;
    }

    template<class InputIterator, class ForwardIterator>
    inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
        ForwardIterator cur = result;
        for (; first != last; ++first, ++cur) 
            construct(&*cur, *first);
        return cur;
    }
    // 无特征萃取器版本
    template<class RandomAccessIterator, class OutputIterator>
    inline OutputIterator copy(RandomAccessIterator first, RandomAccessIterator last, 
        OutputIterator result) {
        for (; first != last; ++first, ++result)
            *result = *first;
        return result;
    }
    // template<class RandomAccessIterator, class OutputIterator, class Distance>
    // inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, 
    //     OutputIterator result, myTraits::random_access_iterator_tag, Distance*) {
    //     for (Distance n = last - first; n > 0; --n, ++first, ++result)
    //         *result = *first;
    //     return result;
    // }
    // template<class InputIterator, class OutputIterator, class Distance>
    // inline OutputIterator __copy(InputIterator first, InputIterator last, 
    //     OutputIterator result, myTraits::input_iterator_tag, Distance*) {
    //     for (; first != last; ++first, ++result)
    //         *result = *first;
    //     return result;
    // }
    // template<class InputIterator, class OutputIterator>
    // inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
    //     return __copy(first, last, result, myTraits::iterator_category(first), myTraits::distance_type(first));
    // }
    // template<class T>//原生指针偏特化版本
    // inline T* copy(T* first, T* last, T* result) {
    //     return __copy(first, last, result, myTraits::random_access_iterator_tag(), myTraits::distance_type(first));
    // }
    // inline char* copy(const char* first, const char* last, char* result) {
    //     memmove(result, first, last - first);
    //     return result + (last - first);
    // }
    // inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
    //     memmove(result, first, last - first);
    //     return result + (last - first);
    // }

    template<class BidirectionalIterator1, class BidirectionalIterator2>
    inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
        while (first != last)
            *--result = *--last;
        return result;
    }

    template<class InputIterator>
    inline typename myTraits::iterator_traits<InputIterator>::difference_type 
    __distance(InputIterator first, InputIterator last, myTraits::input_iterator_tag) {
        typename myTraits::iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            first++;
            n++;
        }
        return n;
    }

    template<class random_access_iterator>
    inline typename myTraits::iterator_traits<random_access_iterator>::difference_type 
    __distance(random_access_iterator first, random_access_iterator last, myTraits::random_access_iterator_tag) {
        return last - first;
    }
    
    template<class InputIterator>
    inline typename myTraits::iterator_traits<InputIterator>::difference_type 
    distance(InputIterator first, InputIterator last) {
        typedef typename myTraits::iterator_traits<InputIterator>::iterator_category category;
        return myAlgorithm::__distance(first, last, category());
    }

/*****************************************上层算法***************************************/
    template <class InputIterator, class T>
    ptrdiff_t count(InputIterator first, InputIterator last, const T& value) {
        ptrdiff_t n;
        for (; first != last; first++)
            if (*first == value)
                ++n;
        return n;
    }

    template <class InputIterator, class T>
    InputIterator find(InputIterator first, InputIterator last, const T& value) {
        while (first != last && *first != value) first++;
        return first;
    }

    template <class InputIterator>
    inline void swap(InputIterator& a, InputIterator& b) {
        InputIterator tmp = a;
        a = b;
        a = tmp;
    }
}
#endif