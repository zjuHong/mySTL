#ifndef MYHASHTABLE_H
#define MYHASHTABLE_H
#include "myAllocator.h"
#include "myTrait.h"
#include "myVector.h"
/*****************************************hash函数***************************************/
namespace mySTL {
    // hash function是计算元素位置的函数,其本质为取模
    template <class Key> struct hash {};

    // 显然，hash并不能支持所有类型，针对某些自定义类型，应当自主撰写函数对象模板全特化版本
    // 对const char* 提供字符串转换函数
    inline size_t __stl_hash_string(const char *s) {
        unsigned long h = 0;
        for (; *s; ++s)
            h = 5 * h + *s;
        return static_cast<size_t>(h);
    }

    template <> struct hash<char *> {
        size_t operator()(const char *s) const noexcept { return __stl_hash_string(s); }
    };

    template <> struct hash<const char *> {
        size_t operator()(const char *s) const noexcept { return __stl_hash_string(s);  }
    };

    template <> struct hash<char> {
        size_t operator()(char x) const noexcept { return x; }
    };

    template <> struct hash<unsigned char> {
        size_t operator()(unsigned char x) const noexcept { return x; }
    };

    template <> struct hash<signed char> {
        size_t operator()(unsigned char x) const noexcept { return x; }
    };

    template <> struct hash<short> {
        size_t operator()(short x) const noexcept { return x; }
    };

    template <> struct hash<unsigned short> {
        size_t operator()(unsigned short x) const noexcept { return x; }
    };

    template <> struct hash<int> {
        size_t operator()(int x) const noexcept { return x; }
    };

    template <> struct hash<unsigned int> {
        size_t operator()(unsigned int x) const noexcept { return x; }
    };

    template <> struct hash<long> {
        size_t operator()(long x) const noexcept { return x; }
    };

    template <> struct hash<unsigned long> {
        size_t operator()(unsigned long x) const noexcept { return x; }
    };
} // namespace mySTL

/*****************************************桶相关***************************************/
static const int __stl_num_primes = 28; 
static const unsigned long __stl_prime_list[__stl_num_primes] = {
        53ul,        97ul,        193ul,       389ul,       769ul,        1543ul,       3079ul,
        6151ul,      12289ul,     24593ul,     49157ul,     98317ul,      196613ul,     393241ul,
        786433ul,    1572869ul,   3145739ul,   6291469ul,   12582917ul,   25165843ul,   50331653ul,
        100663319ul, 201326611ul, 402653189ul, 805306457ul, 1610612741ul, 3221225473ul, 4294967291ul};
// 找出大于n的最小质数
inline unsigned long __stl_next_prime(unsigned long n) noexcept
{
    const unsigned long *first = __stl_prime_list;
    const unsigned long *last = __stl_prime_list + __stl_num_primes;
    const unsigned long *pos = mySTL::lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}
/*****************************************hash节点和迭代器***************************************/
template <class Value>
struct __hashtable_node {
    __hashtable_node* next;
    Value val;
};
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc = myAlloc>
class myHashTable;

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator {

    typedef myHashTable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> hashtable;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef __hashtable_node<Value> node;
    
    typedef mySTL::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    node* cur;
    hashtable* ht;

    /*****************************************构造函数***************************************/
    __hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
    __hashtable_iterator() {}

    /*****************************************运算符重载***************************************/
    reference operator*() const { return cur->val; } 
    pointer operator->() const { return &(operator*()); }
    __hashtable_iterator& operator++() {
        const node* old = cur;
        cur = cur->next;
        if (!cur) {//如果已经到达当前桶的末位，就到寻找下一个桶
            size_type bucket = ht->bkt_num(old->val);
            while (!cur && ++bucket < ht->buckets.size())
                cur = ht->buckets[bucket];
        }
        return *this;
    }
    __hashtable_iterator operator++(int) {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
    bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
    bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class myHashTable {
public:
    typedef HashFcn hasher;
    typedef EqualKey key_equal;
    typedef size_t size_type;
    typedef Value value_type;
    typedef Key key_type;
    typedef ptrdiff_t difference_type;
    typedef Value& reference;
    typedef const Value& const_reference;
    typedef Value* pointer;
    typedef const Value* const_pointer;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef const __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;
private:
    hasher hash;
    key_equal equals;
    ExtractKey get_key;

    typedef __hashtable_node<Value> node;
    typedef simple_alloc<node, Alloc> node_allocator;

    myVector<node*, Alloc> buckets;
    size_type num_element;

    /*****************************************节点操作***************************************/
    node *new_node(const value_type &obj) {
        node *n = node_allocator::allocate();
        try {
            construct(&n->val, obj);
            return n;
        }
        catch (std::exception &) {
            node_allocator::deallocate(n);
        }
    }
    void delete_node(node *n) {
        destroy(&n->val);
        node_allocator::deallocate(n);
    }
    void initialize_buckets(size_type n) {
        const size_type n_buckets = __stl_next_prime(n);
        // 保留空间，由于此时vector's size == 0，因此等价于全部置0
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, static_cast<node *>(nullptr));
        num_element = 0;
    }
    size_type bkt_num_key(const key_type &key, size_type n) const {
        return hash(key) % n;
    }
    size_type bkt_num_key(const key_type &key) const {
        return bkt_num_key(key, buckets.size());
    }
    size_type bkt_num(const value_type &obj) const {
        return bkt_num_key(get_key(obj));
    }
    size_type bkt_num(const value_type &obj, size_type n) const {
        return bkt_num_key(get_key(obj), n);
    }
private:
    friend class __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
public:
    size_type bucket_count() const { return buckets.size(); }
    size_type max_bucket_count() const { return __stl_prime_list[__stl_num_primes - 1]; }

    /*****************************************构造函数***************************************/
    myHashTable(size_type n, const hasher &hf, const key_equal &eql, const ExtractKey &ext)
        : hash(hf), equals(eql), get_key(ext), num_element(0) {
        initialize_buckets(n);
    }
    myHashTable(size_type n, const hasher &hf, const key_equal &eql)
        : hash(hf), equals(eql), get_key(ExtractKey()), num_element(0) {
        initialize_buckets(n);
    }
    ~myHashTable() {
        clear();
    }
    size_type size() const { return num_element; }
    iterator begin() {
        for (size_type n = 0; n < buckets.size(); ++n)
            if (buckets[n])
                return iterator(buckets[n], this);
        return end();
    }

    iterator end() {
        return iterator(nullptr, this);
    }

    /*****************************************常用函数***************************************/
    void resize(size_type num_elements_hint) {
        // 是否重建表格的原则为：若元素个数大于bucket's size,则试图重建表格（我想是为了保证负载率低于0.5）
        const size_type old_n = buckets.size();
        if (num_elements_hint > old_n) { //确定需要扩容
            const size_type n = __stl_next_prime(num_elements_hint);
            if (n > old_n) {
                myVector<node *> temp(n, static_cast<node *>(nullptr));
                try {
                    // 处理每一个旧bucket
                    for (size_type bucket = 0; bucket < old_n; ++bucket) {
                        node *first = buckets[bucket];
                        while (first) {
                            // 找出节点应置于new_bucket的何处
                            size_type new_bucket = bkt_num(first->val, n);
                            buckets[bucket] = first->next;  // 将first与原vector分离
                            first->next = temp[new_bucket]; // first连接至new buckets
                            temp[new_bucket] = first;       // 将first彻底放入new bucket内部
                            first = buckets[bucket];        // 回归old bucket的下一个节点
                        }
                    }
                    buckets.swap(temp); // copy && swap
                }
                catch (std::exception &) {
                    clear();
                }
            }
        }
    }
    inline pair<iterator, bool> insert_unique_noreseize(const value_type &obj) {
        const size_type n = bkt_num(obj); // 决定位于哪个bucket
        node *first = buckets[n];
        for (node *cur = first; cur; cur = cur->next) {
            if (equals(get_key(cur->val), get_key(obj))) // 存在相同键值，拒绝插入
                return pair<iterator, bool>(iterator(cur, this), false);
        }
        node *temp = new_node(obj); // 当前已离开循环或根本未进入循环,创造新节点并将其作为bucket的头部
        temp->next = first;
        buckets[n] = temp;
        ++num_element;
        return pair<iterator, bool>(iterator(temp, this), true);
    }
    iterator insert_equal_noresize(const value_type &obj) {
        const size_type n = bkt_num(obj);
        node *first = buckets[n];
        for (node *cur = first; cur; cur = cur->next) {
            if (equals(get_key(cur->val), get_key(obj))) {
                node *temp = new_node(obj);
                temp->next = cur->next;
                cur->next = temp;
                ++num_element;
                return iterator(temp, this);
            }
        }
        node *temp = new_node(obj);
        temp->next = first;
        buckets[n] = temp;
        ++num_element;
        return iterator(temp, this);
    }
    inline pair<iterator, bool> insert_unique(const value_type &obj) {
        resize(num_element + 1); // 判断是否需要扩充
        return insert_unique_noreseize(obj);
    }
    inline iterator insert_equal(const value_type &obj) {
        resize(num_element + 1);
        return insert_equal_noresize(obj);
    }
    inline void clear() {// clear并没有释放vector
        for (size_type i = 0; i != buckets.size(); ++i) {
            node *cur = buckets[i];
            while (cur) {
                node *next = cur->next;
                delete_node(cur);
                cur = next;
            }
            buckets[i] = nullptr;
        }
        num_element = 0;
    }
    inline void copy_from(const myHashTable &rhs) {
        buckets.clear();
        buckets.reserve(rhs.buckets.size());
        buckets.insert(buckets.end(), rhs.buckets.size(), static_cast<node *>(nullptr));
        try {
            for (size_type i = 0; i != rhs.buckets.size(); ++i) {// 复制每一个vector元素
                if (const node *cur = rhs.buckets[i]) {
                    node *copy = new_node(cur->val);
                    buckets[i] = copy;
                    for (node *next = cur->next; next; cur = next, next = next->next) {
                        copy->next = new_node(next->val);
                        copy = copy->next;
                    }
                }
            }
            num_element = rhs.num_element;
        }
        catch (std::exception &) {
            clear();
        }
    }
    reference find_or_insert(const value_type &obj) {
        resize(num_element + 1);
        size_type n = bkt_num(obj);
        node *first = buckets[n];
        for (node *cur = first; cur; cur = cur->next)
            if (equals(get_key(cur->val), get_key(obj)))
                return cur->val;
        node *temp = new_node(obj);
        temp->next = first;
        buckets[n] = temp;
        ++num_element;
        return temp->val;
    }
    void erase(const iterator &pos) {
        node *p = pos.cur;
        if (p) {
            const size_type n = bkt_num(p->val);
            node *cur = buckets[n];

            if (cur == p) {
                buckets[n] = cur->next;
                delete_node(cur);
                --num_element;
            }
            else {
                node *next = cur->next;
                while (next) {
                    if (next == p) {
                        cur->next = next->next;
                        delete_node(next);
                        --num_element;
                        break;
                    }
                    else {
                        cur = next;
                        next = cur->next;
                    }
                }
            }
        }
    }
};
#endif