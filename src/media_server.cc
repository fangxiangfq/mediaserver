#include "media_server.h"
#include "muduo/base/Logging.h"
#include "muduo/net/Buffer.h"
#include "muduo/net/SocketsOps.h"
#include "json/json.h"

MediaServer::MediaServer(muduo::net::EventLoop* loop, int num, uint16_t rest_port)
    : server_(loop, "MediaServer") , rest_(loop, muduo::net::InetAddress(rest_port), "RestServer")
{
    server_.setMessageCallback(
        std::bind(&MediaServer::onMessage, this, _1, _2));
    server_.setThreadNum(num);
    rest_.setHttpCallback(
        std::bind(&MediaServer::onRequest, this, _1, _2, _3, _4));
    rest_.setThreadNum(0);
}

void MediaServer::start()
{
    server_.start();
    rest_.start();
}

void MediaServer::onMessage(int fd, muduo::net::InetAddress addr)
{
    muduo::net::EventLoop* workloop = server_.threadPool()->getNextLoop();
    muduo::net::EventLoop* mainloop = get_loop();

    if(0 == RouteMap_.count(addr.port()))
    {
        LOG_INFO << "MediaServer - Not Routed" << addr.toIpPort();
        return;
    }

    RoutePtr ptr = RouteMap_[addr.port()];

    std::vector<int> dst;
    
    for(auto it = ptr->begin(); it < ptr->end(); ++it)
    {
        TerminalPtr ter = PortMap_[*it];
        dst.push_back(ter->getfd());
    }

    workloop->runInLoop([&fd, &dst, &mainloop, &addr, this]()
    {
        int savedErrno = 0;
        muduo::net::Buffer inputBuffer;
        ssize_t n = inputBuffer.readFd(fd, &savedErrno);
        std::string msg(inputBuffer.retrieveAllAsString());
        if (n > 0)
        {
            for(auto it = dst.begin(); it < dst.end(); ++it)
            {
                ssize_t nwrote = sockets::write(*it, msg.c_str(), msg.length());
                if(nwrote < 0)
                {
                    mainloop->runInLoop(std::bind(&UdpServer::removeListenerByAddr, &server_, addr));
                }
            }
        }
        else if (n <= 0)
        {
            mainloop->runInLoop(std::bind(&UdpServer::removeListenerByAddr, &server_, addr));
        }

    });
}

uint16_t MediaServer::onCreate(std::string& terno, rest::Code* code)
{
    return 0;
}

void MediaServer::onDelete(std::string& terno, rest::Code* code)
{

}

void MediaServer::onRoute(std::string& terno, std::vector<std::string>& dst, rest::Code* code)
{

}

void MediaServer::onRequest(const HttpRequest& req, Buffer* buf, std::string* body, rest::Code* code)
{
    std::string reqBody(buf->retrieveAllAsString());
    rest::JsonParser parser(reqBoby);
    if (req.path() == "/create")
    {
        
    }
    else if(req.path() == "/delete")
    {

    }
    else if(req.path() == "/route")
    {
        
    }
    else
    {
        *code = rest::Code::bad_request;
    }
}

