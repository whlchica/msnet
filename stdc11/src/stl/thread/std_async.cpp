#include "std_thread.h"
#include <chrono>
#include <future>
#include <string>

std::string fetchDataFromDB(std::string recvData)
{
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return "DB_" + recvData;
}

std::string fetchDataFromFile(std::string recvData)
{
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return "File_" + recvData;
}

/**
 * lambda函数作为回调用std::async
 * std::future<std::string> result = std::async([](std::string recData) {
 *      std::this_thread::sleep_for(std::chrons::seconds(5));
 *      return "DB_" + recData;
 * })；
 *
 *
 */

int async_main()
{
    {
        std::chrono::system_clock::time_point start    = std::chrono::system_clock::now();
        std::string                           dbData   = fetchDataFromDB("Data");
        std::string                           fileData = fetchDataFromFile("Data");
        auto                                  end      = std::chrono::system_clock::now();
        auto                                  diff     = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        std::cout << "total time taken = " << diff << "s\n";
    }
    {
        std::chrono::system_clock::time_point start  = std::chrono::system_clock::now();
        std::future<std::string>              result = std::async(std::launch::async, fetchDataFromDB, "Data");
        //接受一个回调函数作为参数的函数模板，并可能异步执行他们
        /**
         * std::lanch::async 保证异步行为，即传递函数将在单独的线程中执行
         * std::lanch::deferred 当其他线程调用get来方位共享状态时，将调用非异步行为
         * std::lanch::async | std::lanch::deferred 默认行为，他可以异步运行或不运行，取决于系统的负载无法控制
         *
         * 自动创建一个线程(或者从内部线程池中挑选)和一个promise对象
         * 然后将std::promise 对象差U能动给线程函数，便返回std::future对象
         * 当我们传递参数的函数退出时，他的值将被设置在这个promise对象中，所以最终的返回值将在std::future对象中可用
         *
         */
        std::string fileData = fetchDataFromFile("Data");
        std::cout << result.get();
        auto end  = std::chrono::system_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        std::cout << "total time taken = " << diff << "s\n";
    }
    {
        std::future<std::string> result = std::async(
            [](std::string recData) {
                std::this_thread::sleep_for(std::chrono::seconds(5));
                return "DB_" + recData;
            },
            "Hello");
        std::cout << result.get() << "\n";
    }
}