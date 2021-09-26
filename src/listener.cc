#include "listener.h"
#include "muduo/net/SocketsOps.h"
#include "muduo/net/EventLoop.h"
#include "muduo/base/Logging.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "mysockets.h"

void myDefaultMessageCallback(int sockfd, InetAddress addr){}

Listener::Listener(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
  : loop_(loop),
    listenAddr_(listenAddr),
    listenSocket_(mysockets::createNonblockingOrDieForUdp(listenAddr.family())),
    listenChannel_(loop, listenSocket_.fd()),
    listening_(false),
    idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    assert(idleFd_ >= 0);
    listenSocket_.setReuseAddr(true);
    listenSocket_.setReusePort(reuseport);
    listenSocket_.bindAddress(listenAddr);
    listenChannel_.setReadCallback(
        std::bind(&Listener::handleRead, this));
    LOG_TRACE << "fd = " << listenSocket_.fd();
}

Listener::~Listener()
{
    LOG_TRACE << "Distruct fd = " << listenSocket_.fd();
    listenChannel_.disableAll();
    listenChannel_.remove();
    ::close(idleFd_);
}

void Listener::listen()
{
    loop_->assertInLoopThread();
    listening_ = true;
    // listenSocket_.listen();
    listenChannel_.enableReading();
}

void Listener::handleRead()
{
    loop_->assertInLoopThread();
    {
        if (messageCallback_)
        {
            messageCallback_(listenSocket_.fd(), listenAddr_);
            LOG_TRACE << "Rev fd = " << listenSocket_.fd();
        }
    }
}