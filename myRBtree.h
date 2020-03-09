#ifndef _MYRBTREE_H
#define _MYRBTREE_H
#include "myTrait.h"
#include "myAllocator.h"
#include "myAlgorithm.h"
#include "myStruct.h"
#include "myFunction.h"
//删除有待完善
/****************************rb节点*****************************/
typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false;
const __rb_tree_color_type __rb_tree_black = true;
struct __rb_tree_node_base
{
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base *base_ptr;
    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;
    static base_ptr minimum(base_ptr x)
    {
        while (x->left != 0)
            x = x->left;
        return x;
    }
    static base_ptr maximum(base_ptr x)
    {
        while (x->right != 0)
            x = x->right;
        return x;
    }
};

template <class Value>
struct __rb_tree_node : public __rb_tree_node_base
{
    typedef __rb_tree_node<Value> *link_type;
    Value value_field;
};

/****************************rb迭代器*****************************/
struct __rb_tree_base_iterator
{
    typedef __rb_tree_node_base::base_ptr base_ptr;
    typedef mySTL::bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;
    base_ptr node;
    void increment() {
        if (node->right != 0) {
            node = node->right;
            while (node->left != 0)
                node = node->left;
        }
        else {
            base_ptr y = node->parent;
            while (node == y->right) {
                node = y;
                y = y->parent;
            }
            if (node->right != y)
                node = y;
        }
    }
    void decrement() {
        if (node->color == __rb_tree_red && node->parent->parent == node)
            node = node->right;
        else if (node->left != 0) {
            base_ptr y = node->left;
            while (y->right != 0)
                y = y->right;
            node = y;
        }
        else {
            base_ptr y = node->parent;
            while (node == y->left) {
                node = y;
                y = y->parent;
            }
            node = y;
        }
    }
};
template <class Value, class Ref, class Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator
{
    typedef Value value_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef __rb_tree_iterator<Value, Value &, Value *> iterator;
    typedef const __rb_tree_iterator<Value, const Value &, const Value *> const_iterator;
    typedef __rb_tree_iterator<Value, Value &, Value *> self;
    typedef __rb_tree_node<Value> *link_type;

    __rb_tree_iterator() {}
    __rb_tree_iterator(link_type x) { node = x; }
    __rb_tree_iterator(const iterator &x) { node = x.node; }

    reference operator*() const { return link_type(node)->value_field; }
    pointer operator->() const { return &(operator*()); }
    self& operator++() { increment(); return *this; }
    self operator++(int) {
        self tmp = *this;
        increment();
        return tmp;
    }
    self& operator--() { decrement(); return *this; }
    self operator--(int) {
        self tmp = *this;
        decrement();
        return tmp;
    }
    bool operator==(const iterator &x) const { return node == x.node; }
    bool operator!=(const iterator &x) const { return node != x.node; }
};

/****************************rb tree*****************************/
template <class Key, class Value, class KeyofValue, class Compare, class Alloc = myAlloc>
class myRBtree {
protected:
    /****************************typedef*****************************/
    typedef void* void_pointer;
    typedef __rb_tree_node_base* base_ptr;
    typedef __rb_tree_node<Value> rb_tree_node;
    typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
    typedef __rb_tree_color_type color_type;
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef rb_tree_node* link_type;  
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
protected:
    /****************************节点操作*****************************/
    link_type get_node() { return rb_tree_node_allocator::allocate(); }
    void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }
    link_type create_node(const value_type& x) {
        link_type tmp = get_node();
        try {
            construct(&tmp->value_field, x);
        }
        catch(...) {
            put_node(tmp);
        }
        return tmp;
    }
    link_type clone_node(link_type x) {
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = 0;
        tmp->right = 0;
        return tmp;
    }
    void destroy_node(link_type p) {
        destroy(&p->value_field);
        put_node(p);
    }

    /****************************元素及常用函数*****************************/
    size_type node_count;
    link_type header;
    Compare key_compare;

    link_type& root() const { return (link_type&) header->parent; }
    link_type& leftmost() const { return (link_type&) header->left; }
    link_type& rightmost() const { return (link_type&) header->right; }

    static link_type& left(link_type x) { return (link_type&) (x->left); }
    static link_type& right(link_type x) { return (link_type&) (x->right); }
    static link_type& parent(link_type x) { return (link_type&) (x->parent); }
    static reference value(link_type x) { return x->value_field; }
    static const Key& key(link_type x) { return KeyofValue()(value(x)); }
    static color_type& color(base_ptr x) { return (color_type&) (link_type(x)->color); }
    static link_type minimum(link_type x) { return (link_type)__rb_tree_node_base::minimum(x); }
    static link_type maximum(link_type x) { return (link_type)__rb_tree_node_base::maximum(x); }

public:
    typedef __rb_tree_iterator<value_type, reference, pointer> iterator;
    typedef const __rb_tree_iterator<value_type, reference, pointer> const_iterator;

private:
    /****************************内部函数*****************************/
    inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
        __rb_tree_node_base* y = x->right;
        x->right = y->left;
        if (y->left != 0) 
            y->left->parent = x;//回马枪设定父节点
        y->parent = x->parent;
        if (x == root)//x 为根节点
            root = y;
        else if (x == x->parent->left)//x为父节点的左节点
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }
    inline void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
        __rb_tree_node_base* y = x->left;
        x->left = y->right;
        if (y->right != 0) 
            y->right->parent = x;
        y->parent = x->parent;
        if (x == root)
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
        y->right = x;
        x->parent = y;
    }
    inline void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
        x->color = __rb_tree_red;
        while (x != root && x->parent->color == __rb_tree_red) {//父节点为红
            if (x->parent == x->parent->parent->left) {//父节点为祖父节点的左节点
                __rb_tree_node_base* y = x->parent->parent->right;//伯父节点
                if (y && y->color == __rb_tree_red) {
                    x->parent->color = __rb_tree_black;//父节点改黑
                    y->color = __rb_tree_black;//伯父节点改黑
                    x->parent->parent->color = __rb_tree_red;//祖父节点改红
                    x = x->parent->parent;//指向祖父节点
                }
                else {//无伯父节点，或其为黑
                    if (x == x->parent->right) {//如果是父节点的右节点
                        x = x->parent;
                        __rb_tree_rotate_left(x, root);
                    }
                    x->parent->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    __rb_tree_rotate_right(x->parent->parent, root);
                }
            }
            else {//父节点为祖父节点的右节点
                __rb_tree_node_base* y = x->parent->parent->left;//伯父节点
                if (y && y->color == __rb_tree_red) {
                    x->parent->color = __rb_tree_black;
                    y->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    x = x->parent->parent;
                }
                else {//无伯父节点，或其为黑
                    if (x == x->parent->left) {
                        x = x->parent;
                        __rb_tree_rotate_right(x, root);
                    }
                    x->parent->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    __rb_tree_rotate_left(x->parent->parent, root);
                }
            }
        }
        root->color = __rb_tree_black;//根节点为黑
    }
    iterator __insert(base_ptr x_, base_ptr y_, const value_type& v) {
        link_type x = (link_type) x_;
        link_type y = (link_type) y_;
        link_type z;
        if (y == header || x != 0 || key_compare(KeyofValue()(v), key(y))) {
            z = create_node(v);
            left(y) = z;
            if (y == header) {
                root() = z;
                rightmost() = z;
            }
            else if (y == leftmost()) 
                leftmost() = z;
        }
        else {
            z = create_node(v);
            right(y) = z;
            if (y == rightmost())
                rightmost() = z;
        } 
        parent(z) = y;
        left(z) = 0;
        right(z) = 0;
        __rb_tree_rebalance(z, header->parent);//颜色放在此设定
        ++node_count;
        return iterator(z);
    }   
    inline base_ptr rb_tree_rebalance_for_erase(base_ptr z, base_ptr &root, base_ptr &leftmost,
                                    base_ptr &rightmost) {//删除后保证平衡
        base_ptr y = z;
        base_ptr x = nullptr;
        base_ptr x_parent = nullptr;
        if (!y->left) // z至多存在一个孩子
            x = y->right;
        else // z至少存在一个孩子
            if (!y->right)
                x = y->left;
        else {
            y = y->right;
            while (y->left)
                y = y->left;
            x = y->right;
        }
        if (y != z) { // 若条件成立，此时y为z的后代
            z->left->parent = y;
            y->left = z->left;
            if (y != z->right) {
                x_parent = y->parent;
                if (x)
                    x->parent = y->parent;
                y->parent->left = x;
                y->right = z->right;
                z->right->parent = y;
            }
            else
                x_parent = y;
            if (root == z)
                root = y;
            else if (z->parent->left == z)
                z->parent->left = y;
            else
                z->parent->right = y;
            y->parent = z->parent;
            mySTL::swap(y->color, z->color);
            y = z;
        }
        else {
            x_parent = y->parent;
            if (x)
                x->parent = y->parent;
            if (root == z)
                root = x;
            else if (z->parent->left == z)
                z->parent->left = x;
            else
                z->parent->right = x;
            if (leftmost == z) {
                if (!z->right)
                    leftmost = z->parent;
                else
                    leftmost = __rb_tree_node_base::minimum(x);
            }
            if (rightmost == z) {
                if (!z->left)
                    rightmost = z->parent;
                else
                    rightmost = __rb_tree_node_base::maximum(x);
            }
        }
        if (y->color != __rb_tree_red) {
            while (x != root && (!x || x->color == __rb_tree_black))
                if (x == x->parent->left) {
                    base_ptr w = x_parent->right;
                    if (w->color == __rb_tree_red) {
                        w->color = __rb_tree_black;
                        x_parent->color = __rb_tree_red;
                        __rb_tree_rotate_left(x_parent, root);
                        w = x_parent->right;
                    }
                    if ((!w->left || w->left->color == __rb_tree_black) &&
                        (!w->right || w->right->color == __rb_tree_black)) {
                        w->color = __rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    }
                    else {
                        if (!w->right || w->right->color == __rb_tree_black) {
                            if (w->left)
                                w->left->color = __rb_tree_black;
                            w->color = __rb_tree_red;
                            __rb_tree_rotate_right(w, root);
                            w = x_parent->right;
                        }
                        w->color = x_parent->color;
                        x_parent->color = __rb_tree_black;
                        if (x->right)
                            w->right->color = __rb_tree_black;
                        __rb_tree_rotate_left(x_parent, root);
                        break;
                    }
                }
                else {
                    base_ptr w = x_parent->left;
                    if (w->color == __rb_tree_red) {
                        w->color = __rb_tree_black;
                        x_parent->color = __rb_tree_red;
                        __rb_tree_rotate_right(x_parent, root);
                        w = x_parent->left;
                    }
                    if ((!w->right || w->right->color == __rb_tree_black) &&
                        (!w->left || w->left->color == __rb_tree_black)) {
                        w->color = __rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    }
                    else {
                        if (!w->left || w->left->color == __rb_tree_black) {
                            if (w->right)
                                w->right->color = __rb_tree_black;
                            w->color = __rb_tree_red;
                            __rb_tree_rotate_left(w, root);
                            w = x_parent->left;
                        }
                        w->color = x_parent->color;
                        x_parent->color = __rb_tree_black;
                        if (w->left)
                            w->left->color = __rb_tree_black;
                        __rb_tree_rotate_right(x_parent, root);
                        break;
                    }
                }
            if (x)
                x->color = __rb_tree_black;
        }
        return y;
    }
    void init() {
        header = get_node();
        color(header) = __rb_tree_red;
        root() = 0;
        leftmost() = header;
        rightmost() = header;
    }
public:
    /****************************构造析构*****************************/
    myRBtree(const Compare& comp = Compare()) : node_count(0), key_compare(comp) { init(); }
    ~myRBtree() {
        clear();
        put_node(header);
    }
    myRBtree<Key, Value, KeyofValue, Compare, Alloc>& operator=(myRBtree<Key, Value, KeyofValue, Compare, Alloc>& x); 

    /****************************成员函数*****************************/
    Compare key_comp() const { return key_compare; }
    iterator begin() const { return leftmost(); }
    iterator end() const { return header; }
    bool empty() { return node_count == 0;}
    size_type size() const { return node_count; }
    size_type max_size() const { return size_type(-1); }

    mySTL::pair<iterator, bool> insert_unique(const value_type& v) {
        link_type y = header;
        link_type x = root();
        bool comp = true;
        while (x != 0) {
            y = x;
            comp = key_compare(KeyofValue()(v), key(x));
            x = comp ? left(x) : right(x);
        }
        iterator j = iterator(y);//插入点父节点
        if (comp) {
            if (j == begin()) //父节点为最左节点
                return mySTL::pair<iterator, bool>(__insert(x, y, v), true);
            else
                --j;
        }
        if (key_compare(key(link_type(j.node)), KeyofValue()(v)))//比较两次判断是否重复
            return mySTL::pair<iterator, bool>(__insert(x, y, v), true);
        return mySTL::pair<iterator, bool>(j, false);//重复返回false
    }
    void insert_unique(iterator first, iterator last) {
        for (; first != last; ++first)
            insert_unique(*first);
    }

    iterator insert_equal(const value_type& v) {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            y = x;
            x = key_compare(KeyofValue()(v), Key(x)) ? left(x) : right(x);
        }
        return __insert(x, y, v);
    }
    iterator find(const Key& k) {
        link_type y = header;//上一个值不小于k的节点
        link_type x = root();
        while (x != 0) {
            if (!key_compare(key(x), k))
                y = x, x = left(x);
            else
                x = right(x);
        }
        iterator j = iterator(y);
        return (j == end() || key_compare(k, key(j.node))) ? end() : j;
    }
    void erase_aux(link_type x)  {
        while (x) {// 递归式删除
            erase_aux(right(x));
            link_type y = left(x);
            destroy_node(x);
            x = y;
        }
    }
    void erase(iterator pos) {
        link_type y = reinterpret_cast<link_type>(
            rb_tree_rebalance_for_erase(pos.node, header->parent, header->left, header->right));
        destroy_node(y);
        --node_count;
    }
    void erase(iterator first, iterator last) {
        if (first == begin() && last == end())
            clear();
        else
            while (first != last)
                erase(first++);
    }
    void clear(void) {
        if (node_count) {
            erase_aux(root());
            leftmost() = header;
            root() = nullptr;
            rightmost() = header;
            node_count = 0;
        }
    }
    const_iterator lower_bound(const key_type &k) const {
        link_type y = header;
        link_type x = root();
        while (x)
            if (!key_compare(key(x), k))
                y = x, x = left(x);
            else
                x = right(x);
        return const_iterator(y);
    }
    iterator upper_bound(const key_type &k)  {
        link_type y = header;
        link_type x = root();
        while (x)
            if (key_compare(k, key(x)))
                y = x, x = left(x);
            else
                x = right(x);
        return iterator(y);
    }
};
#endif