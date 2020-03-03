#ifndef _ALLOC_H
#define _ALLOC_H

#pragma once
#include <iostream>
using namespace std;

/****************************一级空间配置器*****************************/
typedef void (*ALLOC_OOM_FUN)(); //函数指针

class __MallocAllocTemplate
{
private:
    static ALLOC_OOM_FUN __sMallocAllocOomHandler; //句柄函数，如果设置了，空间分配失败就会调用它

    static void *OomMalloc(size_t n) {
        ALLOC_OOM_FUN MyMallocHandler;
        void *result;
        /*
			1:分配内存成功，直接返回
			2:分配失败，则检查是否有设置处理的MyMallocHandler，
			   如果有，则调用它以后再尝试分配，并不断重复，直到分配成功
			   如果没有，则直接结束程序
		*/
        for (;;) {
            MyMallocHandler = __sMallocAllocOomHandler;
            if (MyMallocHandler) {
                MyMallocHandler();
                void *ret = malloc(n);
                if (ret) 
                    return ret;
            }
            else {
                throw bad_alloc();
            }
        }
        return result;
    }

    static void *OomRealloc(void *p, size_t n) {
        /*
			步骤同上
		*/
        ALLOC_OOM_FUN MyMallocHandler;
        for (;;) {
            MyMallocHandler = __sMallocAllocOomHandler;
            if (MyMallocHandler) {
                MyMallocHandler();
                void *ret = realloc(p, n);
                if (ret)
                    return ret;
            }
            else
                throw bad_alloc();
        }
    }

public:
    static void *allocate(size_t n) {
        void *result = malloc(n); //第一级配置器直接使用malloc
        if (NULL == result)       // 0 == result
            result = OomMalloc(n);
        return result;
    }

    static void deallocate(void *p, size_t /* n */) {
        free(p); //第一级配置器直接使用free()
    }

    static void *reallocate(void *p, size_t /*old_sz*/, size_t new_sz) {
        void *result = realloc(p, new_sz);
        if (NULL == result) {
            result = OomRealloc(p, new_sz);
            return result;
        }
        return result;
    }

    /*设置句柄函数的函数*/
    static void (*SetMallocHandler(void (*f)()))() {
        void (*old)() = __sMallocAllocOomHandler;
        __sMallocAllocOomHandler = f;
        return old;
    }
};

//将句柄函数指针初始化为空
ALLOC_OOM_FUN __MallocAllocTemplate::__sMallocAllocOomHandler = NULL;


/****************************二级空间配置器*****************************/
class __DefaultAllocTemplate
{
public:
    enum {
        __ALIGN = 8
    }; //排列基准值(间隔)
    enum {
        __MAX_BYTES = 128
    }; //最大值,上限
    enum {
        __NFREELISTS = __MAX_BYTES / __ALIGN
    }; //自由链表的个数
        //自由链表(FreeLists)结点
    union Obj {//可视为一个指针，指向相同形式的另一个obj; 也可视为一个指针，指向实际区块
        union Obj *_FreeListLink; //指向下一个内存块的指针
        char ClientData[1];       //客户端可见(测试用)
    };

    static Obj *volatile _FreeList[__NFREELISTS]; //自由链表
    static char *_StartFree;                      //内存池起始位置
    static char *_EndFree;                        //内存池结束位置
    static size_t _HeapSize;                      //从系统堆分配的内存的总大小

public:
    //空间配置函数
    static void *allocate(size_t n) {
        //__TRACE_DEBUG("(n ;%n)\n", n);

        //如果 n > __MAX_BYTES 就直接在一级空间配置器中获取内存
        //否则在二级空间配置器中获取内存

        if (n > (size_t)__MAX_BYTES) {
            return __MallocAllocTemplate::allocate(n);
        }

        size_t index = FREELIST_INDEX(n);

        //1、如果自由链表中没有内存，通过Refill填充
        //2、如果自由链表中有，则返回一个结点块的内存
        //ps: 多线程环境需要考虑加锁

        Obj *head = _FreeList[index];
        if (head == NULL)
            return Refill(ROUND_UP(n));
        else {
            //__TRACE_DEBUG("自由链表取内存 : _FreeList[%s]\n", index);
            _FreeList[index] = head->_FreeListLink;
            return head;
        }
    }

    //空间释放函数
    static void deallocate(void *p, size_t n) {
        //__TRACE_DEBUG("(释放 p : %p, n : %u)\n", p, n);

        //若 n > __MAX_BYTES则直接还给一级配置器
        //否则放回自由链表
        if (n > __MAX_BYTES) {
            __MallocAllocTemplate::deallocate(p, n);
        }
        else {
            //ps : 多线程要考虑加锁
            size_t index = FREELIST_INDEX(n);
            //头插回自由链表
            Obj *tmp = (Obj *)p;
            ((Obj *)p)->_FreeListLink = _FreeList[index];
            _FreeList[index] = tmp;
        }
    }
    static void *reallocate(void *p, size_t old_sz, size_t new_sz) {
        void *result;
        size_t copy_sz;

        //内存块较大，改用一级空间配置器
        if (old_sz > (size_t)__MAX_BYTES && new_sz > __MAX_BYTES)
            return __MallocAllocTemplate::reallocate(p, old_sz, new_sz);

        //修改前和修改后的大小相同，不用修改
        if (ROUND_UP(old_sz) == ROUND_UP(new_sz))
            return p;

        //否则，分配一块空间，并将原来空间的数据拷贝至新空间
        result = allocate(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz; //如果new_sz 小于 old_sz 会丢失数据
        memcpy(result, p, copy_sz);
        deallocate(p, old_sz); //拷贝完后, 释放原来的空间
        return result;
    }

private:
    //将bytes上调至8的倍数
    static size_t ROUND_UP(size_t bytes) {
        return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
        //位运算，保证效率  比如14： 14 + 7 =  21 (0x0001 0101)    和  (0x1000) 进行 & ,结果是: (0x0001 0000) 即 16
    }

    //选择使用哪个自由链表
    static size_t FREELIST_INDEX(size_t bytes) {
        return ((bytes + __ALIGN - 1) / __ALIGN - 1);
    }

private:
    //获得大块内存填充到自由链表中
    static void *Refill(size_t n) {
        //__TRACE_DEBUG("(n : %u)\n", n);

        //分配n bytes的内存，如果不够则能分配多少是多少
        int nobjs = 20;                     //一次要获得的数目(一大块内存)
        char *chunk = ChunkAlloc(n, nobjs); //试图获得nobjs块内存，每块内存的大小为n，并把实际获得的内存数赋予nobjs

        //如果只分配到一块，则直接返回这块内存
        if (nobjs == 1)
            return chunk;

        Obj *result;
        Obj *cur;
        size_t index = FREELIST_INDEX(n); //根据n选择要存放的链表
        result = (Obj *)chunk;

        //把剩余的块链链接到自由链表上面
        cur = (Obj *)(chunk + n);
        _FreeList[index] = cur;
        for (int i = 2; i < nobjs; ++i) {
            cur->_FreeListLink = (Obj *)(chunk + n * i);
            cur = cur->_FreeListLink;
        }
        cur->_FreeListLink = NULL;
        return result;
    }

    static char *ChunkAlloc(size_t size, int &nobjs) {
        char *result;
        size_t TotalBytes = size * nobjs;
        size_t BytesLeft = _EndFree - _StartFree; //内存池的剩余空间

        /*
			1、内存池中内存足够，即BytesLeft >= TotalBytes时，直接从内存池中取出
			2、内存池中的内存不足，但足够一块时，即 Bytesleft >= size时，也直接取出来
			3、内存池中内存连一块内存也不够的时候，从系统堆中分配大块内存到内存池中
		*/
        if (BytesLeft >= TotalBytes) {//1
            result = _StartFree;
            _StartFree += TotalBytes;
            return result;
        }
        else if (BytesLeft >= size)  { //2
            result = _StartFree;
            nobjs = BytesLeft / size;
            _StartFree += nobjs * size;
        }
        else {//3
            //如果内存中还有小块剩余内存(零头)，则将它头插到合适的自由链表
            if (BytesLeft > 0) {
                size_t index = FREELIST_INDEX(BytesLeft);
                ((Obj *)_StartFree)->_FreeListLink = _FreeList[index];
                _FreeList[index] = ((Obj *)_StartFree);
                _StartFree = NULL;
            }

            //从系统堆分配两倍+已分配的 _HeapSize / 8 的内存分配到内存池中
            size_t BytesToGet = 2 * TotalBytes + ROUND_UP(_HeapSize >> 4);
            _StartFree = (char *)malloc(BytesToGet);

            // 如果在堆中内存分配失败，则尝试到自由链表中更大的节点中分配
            if (_StartFree == NULL) {
                //为什么要从大小为size的链表中开始找呢？因为如果是多线程，有可能刚好其他线程
                //有释放大小为size的内存，也就是说，大小为size的链表有可能有内存可以用
                //所以从大小为size的链表开始向后找
                for (int i = size; i <= __MAX_BYTES; i += __ALIGN) {
                    Obj *head = _FreeList[FREELIST_INDEX(size)];
                    if (head) { //找到了有可用的内存，将它摘下来
                        _StartFree = (char *)head;
                        head = head->_FreeListLink;
                        _EndFree = _StartFree + i;
                        return ChunkAlloc(size, nobjs);
                    }
                }

                _EndFree = 0; //防止异常，防止野指针

                //在自由链表中也没找到，则退到一级空间配置器，希望句柄函数能做一些处理，以得到内存

                //__TRACE_DEBUG("系统堆和自由链表中都没有内存了，调到在一级配置器里做最后的尝试\n");
                _StartFree = (char *)__MallocAllocTemplate::allocate(BytesToGet);
            }
            //从系统堆分配的总字节数(可用于下次分配时进行调节)
            _HeapSize += BytesToGet;
            _EndFree = _StartFree + BytesToGet;
            //递归调用获取内存
            return ChunkAlloc(size, nobjs);
        }
        return result;
    }
};

//对类内部的静态成员进行初始化
char *__DefaultAllocTemplate::_StartFree = NULL;
char *__DefaultAllocTemplate::_EndFree = NULL;
size_t __DefaultAllocTemplate::_HeapSize = 0;

typedef typename __DefaultAllocTemplate::Obj Obj;

Obj *volatile __DefaultAllocTemplate::_FreeList[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};

/****************************************最后的封装****************************************/
typedef __DefaultAllocTemplate myAlloc;//默认把二级配置器置为默认的alloc
// typedef __MallocAllocTemplate myAlloc;//把一级配置器置为默认的alloc
template<class T, class Alloc = myAlloc>
class simple_alloc{
public:
    static T* allocate(size_t n) {
        return 0 == n ? 0 : (T*) Alloc::allocate(n * sizeof(T));
    }
    static T* allocate(void) {
        return (T*) Alloc::allocate(sizeof(T));
    }
    static void deallocate(T *p, size_t n) {
        if (0 != n) Alloc::deallocate(p, n * sizeof(T));
    }
    static void deallocate(T *p) {
        Alloc::deallocate(p, sizeof(T));
    }
};

#endif