#ifndef _ALLOC_H
#define _ALLOC_H
#include <iostream>
using namespace std;
#pragma once

/****************************一级空间配置器*****************************/
typedef void (*ALLOC_OOM_FUN)(); 

class __MallocAllocTemplate
{
private:
    static ALLOC_OOM_FUN __sMallocAllocOomHandler; //句柄函数，如果设置了，空间分配失败就会调用它

    static void *OomMalloc(size_t n) {
        ALLOC_OOM_FUN MyMallocHandler;
        void *result;
        for (;;) {//循环尝试，失败了就调用句柄函数
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
        if (NULL == result)       
            result = OomMalloc(n);
        return result;
    }

    static void deallocate(void *p, size_t /* n */) {
        free(p); 
    }

    static void *reallocate(void *p, size_t /*old_sz*/, size_t new_sz) {
        void *result = realloc(p, new_sz);
        if (NULL == result) {
            result = OomRealloc(p, new_sz);
            return result;
        }
        return result;
    }

    static void (*SetMallocHandler(void (*f)()))() {
        void (*old)() = __sMallocAllocOomHandler;
        __sMallocAllocOomHandler = f;
        return old;
    }
};

ALLOC_OOM_FUN __MallocAllocTemplate::__sMallocAllocOomHandler = NULL;


/****************************二级空间配置器*****************************/
class __DefaultAllocTemplate
{
public:
    enum {
        __ALIGN = 8
    }; 
    enum {
        __MAX_BYTES = 128
    }; 
    enum {
        __NFREELISTS = __MAX_BYTES / __ALIGN
    }; 
        
    union Obj {//可视为一个指针，指向相同形式的另一个obj; 也可视为一个指针，指向实际区块
        union Obj *_FreeListLink; 
        char ClientData[1];       
    };

    static Obj *volatile _FreeList[__NFREELISTS]; 
    static char *_StartFree;                      
    static char *_EndFree;                        
    static size_t _HeapSize;                      

public:
    static void *allocate(size_t n) {

        //如果 n > __MAX_BYTES 就直接在一级空间配置器中获取内存， 否则在二级空间配置器中获取内存
        if (n > (size_t)__MAX_BYTES) {
            return __MallocAllocTemplate::allocate(n);
        }

        size_t index = FREELIST_INDEX(n);

        //如果自由链表中没有内存，通过Refill填充；如果自由链表中有，则返回一个结点块的内存
        Obj *head = _FreeList[index];
        if (head == NULL)
            return Refill(ROUND_UP(n));
        else {
            _FreeList[index] = head->_FreeListLink;
            return head;
        }
    }

    static void deallocate(void *p, size_t n) {

        if (n > __MAX_BYTES) {
            __MallocAllocTemplate::deallocate(p, n);
        }
        else {
            //ps : 多线程要考虑加锁
            size_t index = FREELIST_INDEX(n);
            Obj *tmp = (Obj *)p;
            ((Obj *)p)->_FreeListLink = _FreeList[index];
            _FreeList[index] = tmp;
        }
    }
    static void *reallocate(void *p, size_t old_sz, size_t new_sz) {
        void *result;
        size_t copy_sz;

        if (old_sz > (size_t)__MAX_BYTES && new_sz > __MAX_BYTES)
            return __MallocAllocTemplate::reallocate(p, old_sz, new_sz);

        if (ROUND_UP(old_sz) == ROUND_UP(new_sz))
            return p;

        result = allocate(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz; 
        memcpy(result, p, copy_sz);
        deallocate(p, old_sz); 
        return result;
    }

private:
    static size_t ROUND_UP(size_t bytes) {//将bytes上调至8的倍数
        return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
    }

    static size_t FREELIST_INDEX(size_t bytes) {
        return ((bytes + __ALIGN - 1) / __ALIGN - 1);
    }

private:
    static void *Refill(size_t n) {//获得大块内存填充到自由链表中

        int nobjs = 20;                     
        char *chunk = ChunkAlloc(n, nobjs); 
        if (nobjs == 1)
            return chunk;

        Obj *result;
        Obj *cur;
        size_t index = FREELIST_INDEX(n); 
        result = (Obj *)chunk;

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
        size_t BytesLeft = _EndFree - _StartFree; 

        if (BytesLeft >= TotalBytes) {
            result = _StartFree;
            _StartFree += TotalBytes;
            return result;
        }
        else if (BytesLeft >= size)  { 
            result = _StartFree;
            nobjs = BytesLeft / size;
            _StartFree += nobjs * size;
        }
        else { //如果内存中还有小块剩余内存(零头)，则将它头插到合适的自由链表
            if (BytesLeft > 0) {
                size_t index = FREELIST_INDEX(BytesLeft);
                ((Obj *)_StartFree)->_FreeListLink = _FreeList[index];
                _FreeList[index] = ((Obj *)_StartFree);
                _StartFree = NULL;
            }

            //从系统堆分配两倍+已分配的 _HeapSize / 8 的内存分配到内存池中
            size_t BytesToGet = 2 * TotalBytes + ROUND_UP(_HeapSize >> 4);
            _StartFree = (char *)malloc(BytesToGet);

            if (_StartFree == NULL) { // 如果在堆中内存分配失败，则尝试到自由链表中更大的节点中分配
                for (int i = size; i <= __MAX_BYTES; i += __ALIGN) {
                    Obj *head = _FreeList[FREELIST_INDEX(size)];
                    if (head) { //找到了有可用的内存，将它摘下来
                        _StartFree = (char *)head;
                        head = head->_FreeListLink;
                        _EndFree = _StartFree + i;
                        return ChunkAlloc(size, nobjs);
                    }
                }

                _EndFree = 0; 
                _StartFree = (char *)__MallocAllocTemplate::allocate(BytesToGet);
            }
            _HeapSize += BytesToGet;
            _EndFree = _StartFree + BytesToGet;
            return ChunkAlloc(size, nobjs);
        }
        return result;
    }
};

char *__DefaultAllocTemplate::_StartFree = NULL;
char *__DefaultAllocTemplate::_EndFree = NULL;
size_t __DefaultAllocTemplate::_HeapSize = 0;

typedef typename __DefaultAllocTemplate::Obj Obj;

Obj *volatile __DefaultAllocTemplate::_FreeList[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};

/****************************************最后的封装****************************************/
typedef __DefaultAllocTemplate myAlloc;//默认把二级配置器置为默认的alloc
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