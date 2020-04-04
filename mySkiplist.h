#ifndef MY_SKIP_LIST_H
#define MY_SKIP_LIST_H
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "myTrait.h"
using namespace std;
// 参考：https://subetter.com/algorithm/skip-list.html#menu_index_3
#define P 0.25
#define MAX_LEVEL 32
template <class T>
struct Node
{
    T key;
    Node **forward;
    Node(T key = 0, int level = MAX_LEVEL)
    {
        this->key = key;
        forward = new Node*[level];
        memset(forward, 0, level * sizeof(Node*));
    }
};
template <class T>
class mySkipList
{
private:
    Node<T> * header;
    int level;
private:
    int random_level();
public:
    mySkipList();
    ~mySkipList();
    bool insert(T key);
    bool find(T key);
    bool erase(T key);
    void print();
};
// 给定一个概率 p（p 小于 1），产生一个 [0,1) 之间的随机数，如果这个随机数小于 p，则层数加 1；
// 重复以上动作，直到随机数大于概率 p（或层数大于程序给定的最大层数限制）。
template <class T>
int mySkipList<T>::random_level()
{
    int level = 1;
    while ((rand() & 0xffff) < (P * 0xffff) && level < MAX_LEVEL)
        level++;
    return level;
}

template <class T>
mySkipList<T>::mySkipList()
{
    header = new Node<T>;
    level = 0;
}

template <class T>
mySkipList<T>::~mySkipList()
{
    Node<T> * cur = header;
    Node<T> * next = nullptr;
    while (cur)
    {
        next = cur->forward[0];
        delete cur;
        cur = next;
    }
    header = nullptr;
}

template <class T>
bool mySkipList<T>::insert(T key)
{
    Node<T> * node = header;
    Node<T> * update[MAX_LEVEL];
    memset(update, 0, MAX_LEVEL * sizeof(Node<T>*));
    // 找到该结点在每一层应该被插入的位置
    for (int i = level - 1; i >= 0; i--)
    {
        while (node->forward[i] && node->forward[i]->key < key)
            node = node->forward[i];
        update[i] = node;
    }

    node = node->forward[0];
    if (node == nullptr || node->key != key)
    {
        int new_level = random_level();
        // 若新生成的层数比之前的大，那么高出的部分需特殊处理
        if (new_level > level)
        {
            for (int i = level; i < new_level; i++)
                update[i] = header;
            level = new_level;
        }

        Node<T> * new_node = new Node<T>(key, new_level);
        // 前后结点连接起来
        for (int i = 0; i < new_level; i++)
        {
            new_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = new_node;
        }
        return true;
    }

    return false;
}

template <class T>
bool mySkipList<T>::find(T key)
{
    Node<T> * node = header;
    for (int i = level - 1; i >= 0; i--)
    {
        while (node->forward[i] && node->forward[i]->key <= key)
            node = node->forward[i];
        if (node->key == key)
            return true;
    }
    return false;
}

template <class T>
bool mySkipList<T>::erase(T key)
{
    Node<T> * node = header;
    Node<T> * update[MAX_LEVEL];
    memset(update, 0, MAX_LEVEL * sizeof(Node<T>*));
    // 找到要删除结点的位置
    for (int i = level - 1; i >= 0; i--)
    {
        while (node->forward[i] && node->forward[i]->key < key)
            node = node->forward[i];
        update[i] = node;
    }

    node = node->forward[0];
    if (node && node->key == key)
    {
        // 把待删除结点的前后结点连接起来
        for (int i = 0; i < level; i++)
            if (update[i]->forward[i] == node)
                update[i]->forward[i] = node->forward[i];
        delete node;

        // 如果最高层没有结点存在，那么相应的，跳跃表的层数就应该降低
        for (int i = level - 1; i >= 0; i--)
        {
            if (header->forward[i] == nullptr)
                level--;
            else
                break;
        }
    }

    return false;
}

template <class T>
void mySkipList<T>::print()
{
    Node<T> * node = nullptr;
    for (int i = 0; i < level; i++)
    {
        node = header->forward[i];
        cout << "Level " << i << " : ";
        while (node)
        {
            cout << node->key << " ";
            node = node->forward[i];
        }
        cout << endl;
    }

    cout << endl;
}

#endif