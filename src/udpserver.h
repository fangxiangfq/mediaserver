#pragma once

#include "muduo/base/Atomic.h"
#include "muduo/base/Types.h"
#include "muduo/net/Callbacks.h"
#include "muduo/net/EventLoopThreadPool.h"
#include "listener.h"
#include <map>

using namespace muduo;
using namespace muduo::net;

class UdpServer : noncopyable
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;
    enum Option
    {
        kNoReusePort,
        kReusePort,
    };

    UdpServer(EventLoop* loop,
              const string& nameArg,
              Option option = kReusePort);
    ~UdpServer(); 

    const string& name() const { return name_; }
    EventLoop* getLoop() const { return loop_; }

    void setThreadNum(int numThreads);
    void setThreadInitCallback(const ThreadInitCallback& cb)
    { threadInitCallback_ = cb; }

    std::shared_ptr<EventLoopThreadPool> threadPool()
    { return threadPool_; }

    void start();

    void setMessageCallback(const MyMessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }


    int addListener(const InetAddress& listenAddr);
    void removeListenerByAddr(const InetAddress& listenAddr);
    void removeListener(std::shared_ptr<Listener> listener);
private:  

    EventLoop* loop_; 
    const string name_;

    using ListenMap = std::map<uint16_t, std::shared_ptr<Listener>>;
    ListenMap listenMap_;
    std::shared_ptr<EventLoopThreadPool> threadPool_;

    MyMessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    ThreadInitCallback threadInitCallback_;
    AtomicInt32 started_;
};