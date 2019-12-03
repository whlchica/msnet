#include <algorithm>
#include <iostream>
#include <vector>

bool cmp(int a, int b)
{
    return a < b;
}

int main(int argc, char const* argv[])
{
    std::vector<int> vec{ 1, 2, 5, 7, 3, 0, 19, 15 };
    std::vector<int> lvec{ vec };

    std::sort(lvec.begin(), lvec.end(), cmp);

    for (int i : lvec) {
        std::cout << i << std::endl;
    }

    std::sort(vec.begin(), vec.end(), [](int a, int b) -> bool { return a > b; });
    for (int i : vec) {
        std::cout << i << std::endl;
    }

    //捕获外部变量
    int  a = 123;
    auto f = [a](int b) { return a + b; };
    std::cout << f(12) << "\n";
    auto x = [=](int b) { return a + b; }(1236);
    /**
     * 值传递、 [a]
     * 引入传递、[&a]
     * 指针传递 []
     *
     * 隐式捕获 [=]
     */

    //修改变量的捕获
    auto mf = [=]() mutable { std::cout << ++a << "\n"; };
    mf();

    return 0;
}
