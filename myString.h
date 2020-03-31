#ifndef _MY_STRING_H
#define _MY_STRING_H
#include <iostream>
#include <cstring>
using namespace std;

class myString
{
public:
    /****************************构造析构函数*****************************/
    myString(const char *str = NULL);
    myString(const myString &str);
    ~myString();
    /****************************运算符重载*****************************/
    myString operator+(const myString &str);
    myString &operator=(const myString &str);
    myString &operator=(const char *str);
    bool operator==(const myString &str);
    int length();
    myString substr(int start, int n);
    friend ostream &operator<<(ostream &o, const myString &str);

private:
    char *data;
    int size;
};
// 构造函数
myString::myString(const char *str)
{
    if (str == NULL)
    {
        data = new char[1];
        data[0] = '\0';
        size = 0;
    } 
    else
    {
        size = strlen(str);
        data = new char[size + 1];
        strcpy(data, str);
    } 
}

myString::myString(const myString &str)
{
    size = str.size;
    data = new char[size + 1];
    strcpy(data, str.data);
}

myString::~myString()
{
    delete[] data;
}

myString myString::operator+(const myString &str)
{
    myString newStr;
    //释放原有空间
    delete[] newStr.data;
    newStr.size = size + str.size;
    newStr.data = new char[newStr.size + 1];
    strcpy(newStr.data, data);
    strcpy(newStr.data + size, str.data);
    return newStr;
}

myString &myString::operator=(const myString &str)
{
    if (data == str.data)
        return *this;
    delete[] data;
    size = str.size;
    data = new char[size + 1];
    strcpy(data, str.data);
    return *this;
}

myString &myString::operator=(const char *str)
{
    if (data == str)
        return *this;
    delete[] data;
    size = strlen(str);
    data = new char[size + 1];
    strcpy(data, str);
    return *this;
}

bool myString::operator==(const myString &str)
{
    return strcmp(data, str.data) == 0;
}
// 获取字符串长度
int myString::length()
{
    return size;
}
// 求子字符串[start,start+n-1]
myString myString::substr(int start, int n)
{
    myString newStr;
    // 释放原有内存
    delete[] newStr.data;
    // 重新申请内存
    newStr.data = new char[n + 1];
    for (int i = 0; i < n; ++i)
    {
        newStr.data[i] = data[start + i];
    } 
    newStr.data[n] = '\0';
    newStr.size = n;
    return newStr;
}
// 重载输出
ostream &operator<<(ostream &os, const myString &str)
{
    os << str.data;
    return os;
}
#endif