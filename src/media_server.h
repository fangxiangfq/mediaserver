#pragma once

#include <memory>
#include <map>
#include <set>
#include <vector>
#include "muduo/net/EventLoop.h"
#include "udpserver.h"
#include "restserver.h"
using std::placeholders::_4;
class Terminal : noncopyable,
                      public std::enable_shared_from_this<Terminal>
{
public:
    Terminal(uint16_t port, std::string  terno, std::string  ip): port_(port), terno_(terno), ip_(ip){}
    void set_peer(uint16_t port, std::string ip, int fd)
    {
        tport_ = port;
        tip_   = ip;
        fd_    = fd;
    }

    int          getfd() const {return fd_;}
    std::string  getterno() const {return terno_;}
    uint16_t     getport() const {return port_;}
    std::string  getip() const {return ip_;}
    uint16_t     gettport() const {return tport_;}
    std::string  gettip() const{return tip_;}
private:
    uint16_t     port_;
    std::string  terno_;
    std::string  ip_;

    uint16_t     tport_;
    std::string  tip_;

    int          fd_;
};

typedef std::shared_ptr<Terminal> TerminalPtr;                     
typedef std::shared_ptr<std::vector<uint16_t>> RoutePtr;    

class MediaServer
{
 public:
    MediaServer(muduo::net::EventLoop* loop, int num, uint16_t rest_port);

    void start(); 
    muduo::net::EventLoop* get_loop() const {return server_.getLoop();};
 private:
    void onMessage(int fd, InetAddress addr);
    uint16_t onCreate(std::string& terno, rest::Code* code);
    void onDelete(std::string& terno, rest::Code* code);
    void onRoute(std::string& terno, std::vector<std::string>& dst, rest::Code* code);
    void onRequest(const HttpRequest& req, Buffer* buf, std::string* body, rest::Code* code);
    std::set<uint16_t> PortSet_;
    std::map<std::string, uint16_t> TermMap_;
    std::map<uint16_t, TerminalPtr> PortMap_;
    std::map<uint16_t, RoutePtr> RouteMap_;

    UdpServer server_;
    RestServer rest_;
};