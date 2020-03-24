#ifndef _MYHEAP_H
#define _MYHEAP_H
#include "myStruct.h"
#include "myAlgorithm.h"
#include "myTrait.h"
namespace mySTL {    
    /*****************************************heap算法***************************************/
    template <class RandomAccessIterator, class Distance, class T, 
        class Compare = less<typename RandomAccessIterator::value_type>>
    void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value, 
        const Compare &comp = Compare()) {
        Distance parent = (holeIndex - 1) / 2;                           // 找出父节点
        while (holeIndex > topIndex && comp(*(first + parent), value)) { // 尚未到达顶端且其值大于父节点
            *(first + holeIndex) = *(first + parent);                    // 令洞值为父值
            holeIndex = parent;
            parent = (holeIndex - 1) / 2; // 更新父节点
        }
        // 结束循环时更新洞值
        *(first + holeIndex) = value;
    }
    template <class RandomAccessIterator, class Distance, class T, 
        class Compare = less<typename RandomAccessIterator::value_type>>
    inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, 
        const Compare &comp = Compare()) {
        __push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)), comp);
    }
    template <class RandomAccessIterator, 
        class Compare = less<typename RandomAccessIterator::value_type>>
    inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, 
        const Compare &comp = Compare()) {
        __push_heap_aux(first, last, mySTL::distance_type(first), mySTL::value_type(first), comp);
    }
    
    template <class RandomAccessIterator, class Distance, class T, 
        class Compare = less<typename RandomAccessIterator::value_type>>
    void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value, 
        const Compare &comp = Compare()) {
        Distance topIndex = holeIndex;
        Distance secondChild = 2 * holeIndex + 2; // 洞点右子
        while (secondChild < len) {               // 当前尚存在右子
            // 自此secondChild代表较大子节点
            if (comp(*(first + secondChild), *(first + (secondChild - 1)))) // 右子小于左子
                secondChild--;
            *(first + holeIndex) = *(first + secondChild); // 将洞点赋值为较大子值
            holeIndex = secondChild;                       // 更新洞点
            secondChild = 2 * (holeIndex + 1);             // 再次指向洞点右子
        }
        if (secondChild == len) { // 不存在右子，令其左子值为洞值，并将洞号传递至左子（左子必为除首节点外最大者）
            *(first + holeIndex) = *(first + (secondChild - 1));
            holeIndex = secondChild - 1;
        }
        // 将原有的末端元素填入（更新那个洞
        __push_heap(first, holeIndex, topIndex, value, comp);
    }
    template <class RandomAccessIterator, class Distance, class T, 
        class Compare = less<typename RandomAccessIterator::value_type>>
    void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, 
        RandomAccessIterator result, T value, Distance*, const Compare &comp = Compare()) {
        *result = *first;
        __adjust_heap(first, Distance(0), Distance(last - first), value, comp);
    }
    template <class RandomAccessIterator, class T, 
        class Compare = less<typename RandomAccessIterator::value_type>>
    inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*, 
        const Compare &comp = Compare()) {
        __pop_heap(first, last - 1, last - 1, T(*(last - 1)), mySTL::distance_type(first), comp);
    }
    template <class RandomAccessIterator, 
        class Compare = less<typename RandomAccessIterator::value_type>>
    inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, 
        const Compare &comp = Compare()) {
        __pop_heap_aux(first, last, mySTL::value_type(first), comp);
    }

    template <class RandomAccessIterator, class Compare = less<typename RandomAccessIterator::value_type>>
    inline void sort_heap(RandomAccessIterator first, RandomAccessIterator last, 
        const Compare &comp = Compare()) {
        while (last - first > 1)
            pop_heap(first, last--, comp);
    }

    template <class RandomAccessIterator, class Distance, class T, 
        class Compare = less<typename RandomAccessIterator::value_type>>
    inline void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*, 
        const Compare &comp = Compare()) {
        // 遍历所有有资格为root的点
        if (last - first < 2)
            return;
        Distance len = last - first;
        // 找到需要重排的头部（即为最后一个有资格为root的点）
        Distance holeIndex = (len - 2) / 2;
        while (true) {
            // 重排以holeIndex为首的子树,len以防越界
            __adjust_heap(first, holeIndex, len, T(*(first + holeIndex)), comp);
            if (holeIndex == 0)
                return;
            holeIndex--;
        }
    }

    template <class RandomAccessIterator, 
        class Compare = less<typename RandomAccessIterator::value_type>>
    inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, 
        const Compare &comp = Compare()) {
        __make_heap(first, last, mySTL::value_type(first), mySTL::distance_type(first), comp);
    }
}
#endif