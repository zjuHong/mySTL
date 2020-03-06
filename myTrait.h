#ifndef _TRAIT_H
#define _TRAIT_H
#include <cstring>
namespace myTraits {
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag{};
    struct bidirectional_iterator_tag : public forward_iterator_tag{};
    struct random_access_iterator_tag : public bidirectional_iterator_tag{};

    template <class Category, class T, class Distance = ptrdiff_t, class Point = T*, class Reference = T&>
    struct iterator {
        typedef         Category                       iterator_catagory;  
        typedef         T                              value_type;
        typedef         Distance                       difference_type;
        typedef         Point                          pointer;
        typedef         Reference                      reference;
    };

    //traits榨汁机
    template <class Iterator> 
    struct iterator_traits {
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type        value_type;
        typedef typename Iterator::pointer           pointer;
        typedef typename Iterator::reference         reference;
        typedef typename Iterator::difference_type   difference_type;
    };

    //traits偏特化版
    template <class T>
    struct iterator_traits<T*> {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef ptrdiff_t                   difference_type;
        typedef T*                          pointer;
        typedef T&                          reference;
    };

    template <class T>
    struct iterator_traits<const T*> {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef ptrdiff_t                   difference_type;
        typedef const T*                    pointer;
        typedef const T&                    reference;
    };

    template <class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&) {
        typename iterator_traits<Iterator>::iterator_category category;
        return category();   
    };

    template <class Iterator>
    inline typename iterator_traits<Iterator>::difference_type* difference_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);   
    };

    template <class Iterator>
    inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);   
    };
}
#endif