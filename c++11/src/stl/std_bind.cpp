#include "stl.h"
#include <functional>

class A {
public:
    void func(int k)
    {
        std::cout << k << "\n";
    }
};

void func(int x, int y)
{
    std::cout << x + y << "\n";
}

void std_bind_main()
{
    auto f1 = std::bind(func, 1, 2);
    f1();

    auto f2 = std::bind(func, std::placeholders::_1, std::placeholders::_2);
    f2(2, 34);

    A    a;
    auto f3 = std::bind(&A::func, a, std::placeholders::_1);
    f3(23);

    std::function<void(int, int)> f4 = std::bind(func, std::placeholders::_1, std::placeholders::_2);
    f4(1, 3);
}