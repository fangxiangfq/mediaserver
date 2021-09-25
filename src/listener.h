#pragma once 

#include <functional>

#include "muduo/net/Channel.h"
#include "muduo/net/Socket.h"
#include "muduo/net/InetAddress.h"

using namespace muduo;
using namespace muduo::net;

using MyMessageCallback = std::function<void (int sockfd, InetAddress addr)>;
void myDefaultMessageCallback(int sockfd, InetAddress addr);
class Listener : noncopyable
{
public:
    
    Listener(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Listener();

    void setMessageCallback(const MyMessageCallback& cb)
    { messageCallback_ = cb; }

    void listen();

    int get_fd() {return listenSocket_.fd();};
    bool listening() const { return listening_; }

private:
    void handleRead();

    EventLoop* loop_;
    InetAddress listenAddr_;
    Socket listenSocket_;
    Channel listenChannel_;
    MyMessageCallback messageCallback_;
    bool listening_;
    int idleFd_;
};