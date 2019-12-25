#include "SrvPool.h"
#include <functional>
#include <iostream>

// poolSize 线程池大小; 0 内部控制
// threadPreService 每个io_service 线程数量; 0 内部控制
SrvPool::SrvPool(size_t poolSize) : _nextPos(0)
{
    if (0 == poolSize) {
        // 获取硬件支持线程
        return;
    }
    for (size_t i = 0; i < poolSize; i++) {
        SptrIoContext ioService(new asio::io_context());
        SptrWork      work(new asio::io_service::work(*ioService));
        _ioServicePools.push_back(ioService);
        _works.push_back(work);
        // thread_ptr th(new std::thread(std::bind(&asio::io_service::run, ioService)));
        SptrThread th(new std::thread([&ioService]() { ioService->run(); }));
        _threads.push_back(th);
    }
    // 启动一个辅助线程等待io_service安全退出
    SptrThread assistant(new std::thread(std::bind(&SrvPool::join, this)));
    _assistant = assistant;
}

SrvPool::~SrvPool()
{
    // 停止service
    for (size_t i = 0; i < _ioServicePools.size(); i++) {
        _ioServicePools[i]->stop();
    }

    // 辅助线程
    if (_assistant) {
        _assistant->join();
        _assistant.reset();
    }
    _nextPos = 0;
    // 释放works threads
    _works.clear();
    _threads.clear();
    // 释放service poll
    _ioServicePools.clear();
}

// 获取一个io_service
asio::io_context& SrvPool::ioContext()
{
    std::cout << ":=---------->" << _nextPos << "\n";
    asio::io_context& ioService = *_ioServicePools[_nextPos];
    ++_nextPos;
    if (_nextPos == _ioServicePools.size())
        _nextPos = 0;
    return ioService;
}
void SrvPool::join()
{
    for (size_t i = 0; i < _threads.size(); i++) {
        _threads[i]->join();
    }
}