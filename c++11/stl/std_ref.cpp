#include "stl.h"
#include <functional>
#include <iostream>
#include <stdio.h>
#include <thread>

/**
 *  std::ref 用于包装按引用传递值
 *  std::cref 用户包装按const引用传递值
 */

void f(int& a, int& b, const int& c)
{
    printf("a = %d, b = %d, c = %d\n", a, b, c);
    ++a;
    ++b;
    //++c;
    printf("a = %d, b = %d, c = %d\n", a, b, c);
}

void threadRef(int const& x)
{
    int& y = const_cast<int&>(x);
    y++;
    printf("y = %d, x =%d\n", y, x);
}

int ref_main()
{
    int                   a = 1, b = 1, c = 1;
    std::function<void()> func = std::bind(f, a, std::ref(b), std::cref(c));
    a                          = 10;
    b                          = 10;
    func();
    printf("a = %d, b = %d, c = %d\n", a, b, c);

    int         x = 12;
    std::thread th1(threadRef, x);
    th1.join();
    printf("x =%d\n", x);
    std::thread th2(threadRef, std::ref(x));
    th2.join();
    printf("x =%d\n", x);
}