#pragma once

#include "muduo/net/TcpServer.h"
#include "muduo/net/http/HttpContext.h"
#include "muduo/net/http/HttpRequest.h"
#include "muduo/net/http/HttpResponse.h"
#include "restmsg.h"

using namespace muduo;
using namespace muduo::net;

class RestServer : noncopyable
{
public:
    typedef std::function<void (const HttpRequest&, Buffer* buf, std::string*, rest::Code*)> HttpCallback;

    RestServer(EventLoop* loop,
                const InetAddress& listenAddr,
                const string& name,
                TcpServer::Option option = TcpServer::kNoReusePort);

    EventLoop* getLoop() const { return server_.getLoop(); }

    /// Not thread safe, callback be registered before calling start().
    void setHttpCallback(const HttpCallback& cb)
    {
        httpCallback_ = cb;
    }

    void setThreadNum(int numThreads)
    {
        server_.setThreadNum(numThreads);
    }

    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn,
                    Buffer* buf,
                    Timestamp receiveTime);
    void onRequest(const TcpConnectionPtr&, const HttpRequest&, Buffer* buf);
    void sendResponse(const TcpConnectionPtr& conn, std::string& str, rest::Code code, bool close);
    TcpServer server_;
    HttpCallback httpCallback_;
};

