#pragma once

#include <memory>
#include <map>
#include <set>
#include <vector>
#include "muduo/net/EventLoop.h"
#include "udpserver.h"
#include "restserver.h"
#include "mysockets.h"

using std::placeholders::_4;
class Terminal : noncopyable,
                      public std::enable_shared_from_this<Terminal>
{
public:
    Terminal(const uint16_t& local_port, const std::string& terno, const uint16_t& peer_port, const std::string& ip, const int& fd)
    :local_port_(local_port), terno_(terno), sockinfo_(local_port, fd, peer_port, ip) {}

    std::string get_terno(){return terno_;}
    uint16_t get_local(){return local_port_;}
    mysockets::sockinfo get_sockinfo(){return sockinfo_;}
private:
    uint16_t     local_port_;
    std::string  terno_;
    mysockets::sockinfo sockinfo_;
};

typedef std::shared_ptr<Terminal> TerminalPtr;                     

class MediaServer
{
 public:
    MediaServer(muduo::net::EventLoop* loop, int num = 1, uint16_t rest_port = 8000, uint16_t min_media_port = 10000, uint16_t max_media_port = 20000);

    void start(); 
    muduo::net::EventLoop* get_loop() const {return server_.getLoop();};
 private:
    void onMessage(int fd, InetAddress addr);
    void onSend(int fd, std::vector<mysockets::sockinfo> dst, muduo::net::InetAddress addr, muduo::net::EventLoop* mainloop);
    void setMediaPort(uint16_t min_media_port, uint16_t max_media_port);
    uint16_t onCreate(std::string& terno, std::string& ip, uint16_t& peer_port, rest::Code* code);
    void onDelete(std::string& terno, rest::Code* code);
    void onRoute(std::string& terno, std::vector<std::string>& dst, rest::Code* code);
    void onRequest(const HttpRequest& req, Buffer* buf, std::string* body, rest::Code* code);
    std::set<uint16_t> PortSet_;
    std::map<std::string, uint16_t> TermMap_;
    std::map<uint16_t, TerminalPtr> PortMap_;
    std::multimap<uint16_t, uint16_t> RouteMap_;

    UdpServer server_;
    RestServer rest_;
};