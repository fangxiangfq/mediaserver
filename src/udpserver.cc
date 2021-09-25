#include "muduo/net/SocketsOps.h"
#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "udpserver.h"
#include <memory>

UdpServer::UdpServer(EventLoop* loop,
                     const string& nameArg,
                     Option option)
  : loop_(CHECK_NOTNULL(loop)),
    name_(nameArg),
    threadPool_(new EventLoopThreadPool(loop, name_)),
    messageCallback_(myDefaultMessageCallback)
{
}

UdpServer::~UdpServer()
{
    loop_->assertInLoopThread();
    LOG_TRACE << "UdpServer::~UdpServer [" << name_ << "] destructing";

    for (auto& item : listenMap_)
    {
        std::shared_ptr<Listener> listener(item.second);
        item.second.reset();
        removeListener(std::move(listener));
    }
}

void UdpServer::setThreadNum(int numThreads)
{
    assert(0 <= numThreads);
    threadPool_->setThreadNum(numThreads);
}

void UdpServer::start()
{
    if (started_.getAndSet(1) == 0)
    {
        threadPool_->start(threadInitCallback_);
    }
}

void UdpServer::addListener(const InetAddress& listenAddr)
{
    std::shared_ptr<Listener> listener = std::make_shared<Listener>(loop_, listenAddr, true);

    listener->setMessageCallback(messageCallback_);
    listenMap_.emplace(listenAddr.port(), listener);
}

void UdpServer::removeListenerByAddr(const InetAddress& listenAddr)
{
    uint16_t port = listenAddr.port();
    if(0 < listenMap_.count(port))
    {
        std::shared_ptr<Listener> listener(listenMap_[port]);
        removeListener(std::move(listener));
        listenMap_.erase(port);
    }
}

void UdpServer::removeListener(std::shared_ptr<Listener> listener)
{
    
}

