#include "std_thread.h"
#include <future>

void initiazer(std::promise<int>* promObj)
{
    std::cout << "Inside thread\n";
    promObj->set_value(35);
}

/**
 *  std::future 是一个类模板，其对象来存储未来值，通过get返回成员函数实现。如果get函数没有值将会阻塞。
 *
 *  std::future 和std::promise 对象与其管理的std::future 对象共享数据
 *  std::promise 对象在赋值之前被销毁，那么管理std::future的对象将会抛出异常
 *
 *  std::promise 可以保存一个类型为T的值，该值可以被future对象获取（可能在另外一个线程中），因此promise也提供了一种线程同步手段
 */
int future_main()
{
    std::promise<int> promiseObj;
    std::future<int>  futureObj = promiseObj.get_future();  // promise所管理的std::future对象

    std::thread th(initiazer, &promiseObj);
    std::cout << futureObj.get() << "\n";  //这里会阻塞，直到get有值可读
    th.join();

    return 0;
}