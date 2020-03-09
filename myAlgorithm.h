#ifndef _MYALGORITHM_H
#define _MYALGORITHM_H
#include "myStruct.h"
#include <cstring>
#include "myTrait.h"
namespace mySTL{
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
    template<class RandomAccessIterator, class OutputIterator, class Distance>
    inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, 
        OutputIterator result, mySTL::random_access_iterator_tag, Distance*) {
        for (Distance n = last - first; n > 0; --n, ++first, ++result)
            *result = *first;
        return result;
    }
    template<class InputIterator, class OutputIterator, class Distance>
    inline OutputIterator __copy(InputIterator first, InputIterator last, 
        OutputIterator result, mySTL::input_iterator_tag, Distance*) {
        for (; first != last; ++first, ++result)
            *result = *first;
        return result;
    }
    template<class InputIterator, class OutputIterator>
    inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        return __copy(first, last, result, mySTL::iterator_category(first), mySTL::distance_type(first));
    }
    template<class T>//原生指针偏特化版本
    inline T* copy(T* first, T* last, T* result) {
        return __copy(first, last, result, mySTL::random_access_iterator_tag(), mySTL::distance_type(first));
    }
    inline char* copy(const char* first, const char* last, char* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }
    inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    template<class BidirectionalIterator1, class BidirectionalIterator2>
    inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
        while (first != last)
            *--result = *--last;
        return result;
    }

    template<class InputIterator>
    inline typename mySTL::iterator_traits<InputIterator>::difference_type 
    __distance(InputIterator first, InputIterator last, mySTL::input_iterator_tag) {
        typename mySTL::iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            first++;
            n++;
        }
        return n;
    }

    template<class random_access_iterator>
    inline typename mySTL::iterator_traits<random_access_iterator>::difference_type 
    __distance(random_access_iterator first, random_access_iterator last, mySTL::random_access_iterator_tag) {
        return last - first;
    }
    
    template<class InputIterator>
    inline typename mySTL::iterator_traits<InputIterator>::difference_type 
    distance(InputIterator first, InputIterator last) {
        typedef typename mySTL::iterator_traits<InputIterator>::iterator_category category;
        return mySTL::__distance(first, last, category());
    }

    /*****************************************常用接口算法***************************************/
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

    template <class ForwardIterator, class T>
    inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T &value) {
        return __lower_bound(first, last, value, distance_type(first), iterator_category(first));
    }

    template <class ForwardIterator, class T, class Distance>
    ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last, const T &value, Distance,
                                  forward_iterator_tag) {
        Distance len = 0;
        distance(first, last, len); //求取长度
        Distance half;
        ForwardIterator middle;

        while (len > 0) {
            half = len >> 1;
            middle = first;
            advance(middle, half); //令middle指向中间位置
            if (*middle < value) {
                first = middle;
                ++first; //令first指向middle下一位置
                len = len - half - 1;
            }
            else
                len = half;
        }
        return first;
    }

    template <class RandomAccessIterator, class T, class Distance>
    RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last, const T &value, 
                                            Distance*, random_access_iterator_tag) {
        Distance len = last - first;
        Distance half;
        RandomAccessIterator middle;

        while (len > 0) {
            half = len >> 1;
            middle = first;
            middle = first + half;
            if (*middle < value) {
                first = middle + 1;
                len = len - half - 1;
            }
            else
                len = half;
        }
        return first;
    }

    template <class ForwardIterator, class T>
    inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T &value) {
        return __upper_bound(first, last, value, distance_type(first), iterator_category(first));
    }

    template <class ForwardIterator, class T, class Distance>
    ForwardIterator __upper_bound(ForwardIterator first, ForwardIterator last, const T &value, Distance *,
                                  forward_iterator_tag) {
        Distance len = 0;
        distance(first, last, len); //求取长度
        Distance half;
        ForwardIterator middle;

        while (len > 0) {
            half = len >> 1;
            middle = first;
            advance(middle, half); //令middle指向中间位置
            if (value < *middle)
                len = half;
            else {
                first = middle;
                ++first;
                len = len - half - 1;
            }
        }
        return first;
    }

    template <class RandomAccessIterator, class T, class Distance>
    RandomAccessIterator __upper_bound(RandomAccessIterator first, RandomAccessIterator last, const T &value,
                                       Distance *, random_access_iterator_tag) {
        Distance len = last - first;
        Distance half;
        RandomAccessIterator middle;

        while (len > 0) {
            half = len >> 1;
            middle = first;
            middle = first + half;
            if (value < *middle)
                len = half;
            else {
                first = middle + 1;
                len = len - half - 1;
            }
        }
        return first;
    }
}
#endif