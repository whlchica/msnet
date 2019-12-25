#include "stl.h"
#include <functional>
#include <iostream>

struct Foo {
    int _num;
    Foo(int num) : _num(num) {}
    void print_add(int i) const
    {
        std::cout << _num + i << "\n";
    }
};

void function_print()
{
    std::cout << __func__ << "\n";
}

int std_function_main()
{
    //函数、
    std::function<void()> func_p = function_print;
    func_p();
    // lambda表达式、
    std::function<void()> func = []() { std::cout << "hello\n"; };
    func();
    //绑定表达式、以及其他函数对象
    std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
    Foo                                  foo(2);
    f_add_display(foo, 10);
}