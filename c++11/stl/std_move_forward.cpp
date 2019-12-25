/**
 * 左值、右值C++中并没有对左值、右值进行明确定义，从解决手段来看： 凡事能取地址的可以称之为左值，反之称之为右值
 *
 */

#include "stl.h"
// class A { };
// A a; //左值 &a是合法的
// void Test(A a)
// {
//     __Test(a);
// }
// Test(new A()); //A()为右值，因为A()产生一个临时对象，临时对象没有名字无法进行&取地址操作
// 我们可以看到上一层的右值可以"变成"左值来完成下一层的调用：（可以接受右值的右值引用本身却是个左值）

class A {
    int* array{ nullptr };
public:
    A() : array(new int[3]{ 1, 23, 2 }) {}

    ~A()
    {
        if (nullptr != array) {
            delete[] array;
        }
    }

    A(const A& a)
    {
        std::cout << "Copy COnstruct\n";
    }
};


void move_forward_main()
{
    A a1;
    A a2(a1);
}