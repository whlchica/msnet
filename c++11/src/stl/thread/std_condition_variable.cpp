#include "std_thread.h"
#include <condition_variable>
#include <mutex>
#include <unistd.h>
#include <functional>

class application {
    std::mutex              _mutex;
    std::condition_variable _cv;
    bool                    _dataLoaded;

public:
    application()
    {
        _dataLoaded = false;
    }

    void loadData()
    {
        std::cout << __FILE__ << __FUNCTION__ << __LINE__ << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //锁定数据
        std::lock_guard<std::mutex> lock(_mutex);
        _dataLoaded = true;
        _cv.notify_one();
    }

    bool isDataLoad()
    {
        return _dataLoaded;
    }

    void mainTask()
    {
        std::cout << __FILE__ << __FUNCTION__ << __LINE__ << "\n";
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, std::bind(&application::isDataLoad, this));
        /**
         *  开始等待条件变量得到信号 wait()将在内部释放锁，并使线程阻塞
         *  一旦条件变量发出信号，则恢复线程并再次获取锁，检测条件是否满足，满足go on，不满足，再次进入wait()
         */
        std::cout << "do processing on loaded data\n";
    }
};

int condition_variable_main()
{
    application app;
    std::thread th1(&application::mainTask, &app);
    std::thread th2(&application::loadData, &app);
    std::cout << __FILE__ << __FUNCTION__ << __LINE__ << "\n";
    sleep(1);
    th1.join();
    th2.join();
    return 0;
}