#include "stl.h"
#include <array>
#include <cassert>
#include <chrono>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <functional>

namespace st {

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}  // namespace std

void contains_main()
{
    // array
    {
        std::cout << "---------------std arrary contains\n";
        std::array<int, 5> stdarray = { 1, 2, 6, 4, 20 };
        for (auto it = stdarray.begin(); it != stdarray.end(); it++) {
            std::cout << *it << "\t";
        }
        std::cout << "\n";
    }
}

struct sBase {
    sBase()
    {
        std::cout << "sBase::sBase()\n";
    }
    virtual ~sBase()
    {
        std::cout << "sBase::~sBase()\n";
    }
};

struct sDerived : public sBase {
    sDerived()
    {
        std::cout << "sDerived::sDerived()\n";
    }
    virtual ~sDerived()
    {
        std::cout << "sDerived::~sDerived()\n";
    }
};

void thr(std::shared_ptr<sBase> p, int i)
{
    std::shared_ptr<sBase> lp = p;
    std::cout << i << " local pointer in a thread:\n"
              << "lp.get() = " << lp.get() << " lp.use_conut() = " << lp.use_count() << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        static std::mutex           io_mutex;
        std::lock_guard<std::mutex> lk(io_mutex);
        std::cout << i << " local pointer in a thread:\n"
                  << "lp.get() = " << lp.get() << " lp.use_conut() = " << lp.use_count() << "\n";
    }
}

struct B {
    virtual void bar()
    {
        std::cout << "B::bar()\n";
    }
    virtual ~B() = default;
};

struct D : B {
    D()
    {
        std::cout << "D:D()\n";
    }
    ~D()
    {
        std::cout << "D:~D()\n";
    }
    void bar() override
    {
        std::cout << "D:bar()\n";
    }
};

std::unique_ptr<D> pass_through(std::unique_ptr<D> p)
{
    p->bar();
    return p;
}

void close_file(std::FILE* fp)
{
    std::cout << "deleter\n";
    std::fclose(fp);
}

/**
 *  1、尽量不使用相同的原始指针来创建多个shared_ptr对象
 *
 *
 *
 *
 */

void error_shared_ptr()
{
    {
        int*                 rawPtr = new int();
        std::shared_ptr<int> ptr_1(rawPtr);
        std::shared_ptr<int> ptr_2(rawPtr);
        //当我们去清理ptr_2时，清除原始指针，那么ptr_1就会成为nullptr
    }
    {
        int                  x = 12;
        std::shared_ptr<int> ptr(&x);
        //这里shared_ptr关联的内存是堆，当析构时执行delete删除堆上的对象会引起程序崩溃。
        //所以建议用std::make_shared<>来创建shared_ptr指针
    }
}

void smart_ptr_main()
{
    //拥有共享对象所有权的语义的智能指针
    // shared_ptr 多个智能指针可以共享一个对象，对象末尾一个拥有着销毁对象的责任，并清理与该对象的多有资源
    {
        std::cout << "---------------std shared_ptr\n";
        std::shared_ptr<sBase> p = std::make_shared<sDerived>();

        std::cout << "create a shared sDerived(as a pointer to sBase)\n"
                  << "p.get() = " << p.get() << ", p.use_count() = " << p.use_count() << "\n";

        std::thread t1(thr, p, 1), t2(thr, p, 2), t3(thr, p, 3);
        std::cout << "create a shared sDerived(as a pointer to sBase)\n" 
                  << "p.get() = " << p.get() << ", p.use_count() = " << p.use_count() << "\n";
        p.reset();  //这里会把计数减1 p.reset(new sDerived());也可以重新指向新指针
        std::cout << "shared ownership between 3 threads and released\n"
                  << "ownership from main:\n"
                  << "p.get() = " << p.get() << ", p.use_count() = " << p.use_count() << "\n";

        t1.join();
        t2.join();
        t3.join();
        std::cout << "all threads completed, the last one deleted sDerived\n";
    }

    // unique_ptr 拥有独立对象所有权的智能指针
    {
        std::cout << "---------------std shared_ptr\n";
        {
            std::cout << "unqiue ownership semantics demo\n";
            auto p = st::make_unique<D>();
            auto q = pass_through(std::move(p));
            assert(!p);  //现在p不占任何内容并保有空指针
            q->bar();
        }  //代码段在此结束，q会自动释放掉
        {
            std::cout << "runtime polymorphism demo\n";
            std::unique_ptr<B> p = st::make_unique<D>();  // p 是占有D的unique_ptr 作为指向基类的指针
            p->bar();                                      //虚派发

            std::vector<std::unique_ptr<B>> v;
            v.push_back(st::make_unique<D>());
            v.push_back(std::move(p));
            v.emplace_back(new D);
            for (auto& p : v) {
                p->bar();
            }
        }
        {
            std::cout << "custom deleter demo\n";
            std::ofstream("demo.txt") << 'x';
            std::unique_ptr<std::FILE, decltype(&close_file)> fp(std::fopen("demo.txt", "r"), &close_file);
            //std::unique_ptr<std::FILE, void (*)(std::FILE*)> fp(std::fopen("demo.txt", "r"), close_file);
            if (fp) {
                std::cout << ( char )std::fgetc(fp.get()) << "\n";
            }
        }  //这里会调用自定义的deleter函数 close_file, 只有fopen成功

        {
            std::cout << "custom lambda-expression deleter demo\n";
            std::unique_ptr<D, std::function<void(D*)>> p(new D, [](D* ptr) {
                std::cout << "destorying from a custom deleter...\n";
                delete ptr;
            });
            p->bar();
        }  //这里调用上述lambda 并销毁D

        {
            std::cout << "array form of unique_ptr demo\n";
            std::unique_ptr<D[]> p(new D[ 3 ]);
        }
    }
}
