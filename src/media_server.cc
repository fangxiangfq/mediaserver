#include "media_server.h"
#include "muduo/base/Logging.h"
#include "muduo/net/Buffer.h"
#include "muduo/net/SocketsOps.h"
#include "json/json.h"


MediaServer::MediaServer(muduo::net::EventLoop* loop, int num, uint16_t rest_port, uint16_t min_media_port, uint16_t max_media_port)
    : server_(loop, "MediaServer") , rest_(loop, muduo::net::InetAddress(rest_port), "RestServer")
{
    setMediaPort(min_media_port, max_media_port);
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

void MediaServer::onSend(int fd, std::vector<mysockets::sockinfo> dst, muduo::net::InetAddress addr, muduo::net::EventLoop* mainloop)
{
    int savedErrno = 0;
    muduo::net::Buffer inputBuffer;
    ssize_t n = inputBuffer.readFd(fd, &savedErrno);
    std::string msg(inputBuffer.retrieveAllAsString());
    if (n > 0)
    {
        for(auto it = dst.begin(); it < dst.end(); ++it)
        {
            ssize_t nwrote = mysockets::sendto(*it, msg.c_str(), msg.length());
            if(nwrote < 0)
            {//todo know why runned without judge
                mainloop->runInLoop(std::bind(&UdpServer::removeListenerByAddr, &server_, addr));
            }
        }
    }
    else if (n <= 0)
    {//todo know why runned without judge
        mainloop->runInLoop(std::bind(&UdpServer::removeListenerByAddr, &server_, addr));
    }
}

void MediaServer::onMessage(int fd, muduo::net::InetAddress addr)
{
    muduo::net::EventLoop* workloop = server_.threadPool()->getNextLoop();
    muduo::net::EventLoop* mainloop = get_loop();

    if(0 == RouteMap_.count(addr.port()))
    {
        LOG_INFO << "MediaServer - Not Routed" << addr.toIpPort();
    }

    const uint16_t port = addr.port();

    std::vector<mysockets::sockinfo> dst;
    
    for(auto it = RouteMap_.lower_bound(port); it != RouteMap_.upper_bound(port); ++it)
    {
        TerminalPtr ter = PortMap_[it->second];
        dst.push_back(ter->get_sockinfo());
    }

    workloop->runInLoop(std::bind(&MediaServer::onSend, this, fd, dst, addr, mainloop));
}

// maybe remove -Wconversion
void MediaServer::setMediaPort(uint16_t min_media_port, uint16_t max_media_port) 
{
    server_.getLoop()->assertInLoopThread();
    PortSet_.clear();

    for(uint16_t i = min_media_port; i <= max_media_port; i = static_cast<uint16_t>(i + 2))
    {
        PortSet_.insert(i); 
    }
}

uint16_t MediaServer::onCreate(std::string& terno, std::string& ip, uint16_t& peer_port, rest::Code* code)
{
    if(TermMap_.count(terno) > 0)
    {
        *code = rest::Code::bad_request;
        return 0;
    }

    if(PortSet_.empty())
    {
        *code = rest::Code::server_full;
        return 0;
    }

    uint16_t port = *PortSet_.begin();
    PortSet_.erase(PortSet_.begin());

    int fd = server_.addListener(muduo::net::InetAddress(port));

    TermMap_[terno] = port;

    PortMap_.emplace(port, std::make_shared<Terminal>(port, terno, peer_port, ip, fd));

    return port;
}

//todo route as dst delete
void MediaServer::onDelete(std::string& terno, rest::Code* code)
{
    if(TermMap_.count(terno) == 0) 
        return;
    uint16_t port = TermMap_[terno];
    PortMap_.erase(port);
    server_.removeListenerByAddr(muduo::net::InetAddress(port));

    TermMap_.erase(terno);

    RouteMap_.erase(port);

    PortSet_.insert(port);
}

//todo route change part apply
void MediaServer::onRoute(std::string& terno, std::vector<std::string>& dst, rest::Code* code)
{
    if(TermMap_.count(terno) == 0) 
    {
        *code = rest::Code::bad_request;
        return;
    }

    uint16_t port = TermMap_[terno];
    RouteMap_.erase(port);
    for(auto& it : dst)
    {
        if(TermMap_.count(it))
            RouteMap_.emplace(port, TermMap_[it]);
    }
}

void MediaServer::onRequest(const HttpRequest& req, Buffer* buf, std::string* body, rest::Code* code)
{
    std::string reqBody(buf->retrieveAllAsString());
    rest::JsonParser parser(reqBody);
    std::string terno;
    *code = rest::Code::success;
    if (req.path() == "/create")
    {
        std::string ip;
        uint16_t peer_port;
        if(!parser.CreateParse(terno, ip, peer_port))
        {
            *code = rest::Code::bad_request;
        }
        else
        {
            uint16_t port = onCreate(terno, ip, peer_port, code);
            if(0 != port)
            {
                rest::JsonBuilder builder(*code, "port", port);
                std::string msg = builder.toString();
                *body = msg;
                return;
            }
        }
    }
    else if(req.path() == "/delete")
    {
        if(!parser.DeleteParse(terno))
        {
            *code = rest::Code::bad_request;
        }
        else
        {
            onDelete(terno, code);
        }
    }
    else if(req.path() == "/route")
    {
        std::vector<std::string> dst;
        if(!parser.RouteParse(terno, dst))
        {
            *code = rest::Code::bad_request;
        }
        else
        {
            onRoute(terno, dst, code);
        }
    }
    else
    {
        *code = rest::Code::bad_request;
    }
    rest::JsonBuilder builder(*code);
    std::string msg = builder.toString();
    *body = msg;
}

