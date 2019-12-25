#ifndef ASIO_SERVICE_POOL_H
#define ASIO_SERVICE_POOL_H

#include <asio.hpp>
#include <thread>
#include <vector>

class SrvPool : private asio::noncopyable {
public:
    // poolSize 线程池大小; 0 内部控制
    // threadPreService 每个io_service 线程数量; 0 内部控制
    explicit SrvPool(size_t poolSize = 1);

    virtual ~SrvPool();

    // 获取一个io_service
    asio::io_context& ioContext();

private:
    void join();
    using SptrIoContext = std::shared_ptr<asio::io_context>;
    using SptrWork      = std::shared_ptr<asio::io_service::work>;
    using SptrThread    = std::shared_ptr<std::thread>;
    std::vector<SptrIoContext> _ioServicePools;
    std::vector<SptrWork>      _works;
    std::vector<SptrThread>    _threads;
    SptrThread                 _assistant;
    size_t                     _nextPos;
};

#endif