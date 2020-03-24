#ifndef _MYALGORITHM_H
#define _MYALGORITHM_H
#include "myStruct.h"
#include <cstring>
#include "myTrait.h"
#include "myHeap.h"
#include "myFunction.h"

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

    template <class RandomAccessIterator, class T, class Compare = less<typename RandomAccessIterator::value_type>>
    void __partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, T*, 
                                                                    const Compare &comp = Compare()) {
        mySTL::make_heap(first, middle, comp);
        for (RandomAccessIterator i = middle; i < last; ++i) {
            if (*i < *first) 
                mySTL::__pop_heap(first, middle, i, T(*i), distance_type(first), Compare());
        }
        mySTL::sort_heap(first, middle, comp);
    }
    template <class RandomAccessIterator, class Compare = less<typename RandomAccessIterator::value_type>>
    inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, 
                                                                    const Compare &comp = Compare()) {
        __partial_sort(first, middle, last, value_type(first), comp);
    }

    /****************************sort相关*****************************/
    const int __stl_threshold = 16; 

    template <class RandomAccessIterator, class T, class Compare = less<typename RandomAccessIterator::value_type>>
    void __unguarded_linear_insert(RandomAccessIterator last, T value, 
                                                                    const Compare &comp = Compare()) {
        RandomAccessIterator next = last;
        --next;
        while (comp(value, *next)) {//大于value的往后移
            *last = *next;
            last = next;
            --next;
        }
        *last = value;
    }

    template <class RandomAccessIterator, class T, class Compare = less<typename RandomAccessIterator::value_type>>
    void __linear_insert(RandomAccessIterator first, RandomAccessIterator last, T*, 
                                                                    const Compare &comp = Compare()) {
        T value = *last;
        if (comp(value, *first)) {
            copy_backward(first, last, last + 1);
            *first = value;
        }
        else
            __unguarded_linear_insert(last, value, comp);
    }

    template <class RandomAccessIterator, class Compare = less<typename RandomAccessIterator::value_type>>
    void __insertion_sort(RandomAccessIterator first, RandomAccessIterator last, 
                                                                    const Compare &comp = Compare()) {
        if (first == last) return;
        for (RandomAccessIterator i = first + 1; i != last; ++i) 
            __linear_insert(first, i, value_type(first), comp);
    }

    template <class RandomAccessIterator, class T, class Compare = less<typename RandomAccessIterator::value_type>>
    void __unguarded_insertion_sort_aux(RandomAccessIterator first, RandomAccessIterator last, T*, 
                                                                    const Compare &comp = Compare()) {
        for (RandomAccessIterator i = first; i != last; ++i) 
            __unguarded_linear_insert(i, T(*i), comp);
    }

    template <class RandomAccessIterator, class Compare = less<typename RandomAccessIterator::value_type>>
    void __unguarded_insertion_sort(RandomAccessIterator first, RandomAccessIterator last, 
                                                                    const Compare &comp = Compare()) {
        __unguarded_insertion_sort_aux(first, last, value_type(first), comp);
    }

    template <class RandomAccessIterator, class Compare = less<typename RandomAccessIterator::value_type>>
    void __final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last, 
                                                                    const Compare &comp = Compare()) {
        //子序列已经有相当程度的排序，但尚未完全排序
        if (last - first > __stl_threshold) {//大于16就分割成两部分
            __insertion_sort(first, first + __stl_threshold, comp);
            __unguarded_insertion_sort(first + __stl_threshold, last, comp);
        }
        else
            __insertion_sort(first, last, comp);
    }

    template <class T, class Compare = less<typename T::value_type>>
    inline const T& __median(const T& a, const T& b, const T& c, 
                                                                    const Compare &comp = Compare()) {
        if (comp(a, b)) 
            if (comp(b, c)) return b; 
            else if (comp(a, c)) return c;
            else return a;
        else if (comp(a, c)) return a;
        else if (comp(b, c)) return c;
        else return b;
    }

    template <class RandomAccessIterator, class T, class Compare = less<typename RandomAccessIterator::value_type>>
    RandomAccessIterator __unguarded_partion(RandomAccessIterator first, RandomAccessIterator last, T pivot, 
                                                                    const Compare &comp = Compare()) {
        while (true) {
            while (comp(*first, pivot)) ++first;
            --last;
            while (comp(pivot, *last)) --last;
            if (!(comp(*first, *last))) return last;
            swap(first, last);
            ++first;
        }
    }

    template <class RandomAccessIterator, class T, class Size, class Compare = less<typename RandomAccessIterator::value_type>> 
    void __introsort_loop(RandomAccessIterator first, RandomAccessIterator last, T*, Size depth_limit, 
                                                                    const Compare &comp = Compare()) {
        while (last - first > __stl_threshold) {
            if (depth_limit == 0) {//分割恶化采用堆排序
                partial_sort(first, last, last, comp);
                return;
            }
            --depth_limit;
            RandomAccessIterator cut = __unguarded_partion(first, last, T(__median(*first, *(first + (last - first) / 2), 
                *(last - 1), comp)), comp);
            __introsort_loop(cut, last, value_type(first), depth_limit, comp);//右半段递归sort
            last = cut;//之后继续递归，对左半段递归
        }
    }
    template <class Size>
    inline Size __lg(Size n) {//log2(n)
        Size k;
        for (k = 0; n > 1; n >>= 1) ++k;
        return k;
    }

    template <class RandomAccessIterator, class Compare = less<typename RandomAccessIterator::value_type>> 
    inline void sort(RandomAccessIterator first, RandomAccessIterator last, 
                                                                    const Compare &comp = Compare()) {
        if (!(first == last)) { //先快排最后插入排序
            __introsort_loop(first, last, value_type(first), __lg((last - first) * 2), comp);
            __final_insertion_sort(first, last, comp);
        }
    }
}
#endif