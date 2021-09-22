#pragma once

#include <memory>
#include <map>
#include <vector>
#include "muduo/net/TcpServer.h"
#include "muduo/base/noncopyable.h"


class Terminal : noncopyable,
                      public std::enable_shared_from_this<Terminal>
{
    Terminal(uint16_t port, std::string  terno, std::string  ip): port_(port), terno_(terno), ip_(ip){}
    void set_peer(uint16_t port, std::string ip, int fd)
    {
        tport_ = port;
        tip_   = ip;
        fd_    = fd;
    }

    int          fd() const {return fd_;}
    std::string  terno() const {return terno_;}
    uint16_t     port() const {return port_;}
    std::string  ip() const {return ip_;}
    uint16_t     tport() const {return tport_;}
    std::string  tip() const{return tip_;}
    bool         conn() const{return conn_;}
    void         set_conn(const bool& conn) const {conn_ = conn;}
private:
    uint16_t     port_;
    std::string  terno_;
    std::string  ip_;

    uint16_t     tport_;
    std::string  tip_;

    int          fd_;
    bool         conn_{false};
}

typedef std::shared_ptr<Terminal> TerminalPtr;                     
typedef std::shared_ptr<std::vector<uint16_t>> RoutePtr;    

// RFC 862
class MediaServer
{
 public:
    MediaServer(muduo::net::EventLoop* loop,
                const muduo::net::InetAddress& listenAddr);

    void start();  // calls server_.start();
    muduo::net::EventLoop*  get_loop() const {return loop_};
 private:
    void onConnection(const muduo::net::TcpConnectionPtr& conn);

    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                    muduo::net::Buffer* buf,
                    muduo::Timestamp time);

    std::map<uint16_t, TerminalPtr> PortMap_;
    // std::map<std::string, TerminalPtr> TerminalMap_;
    std::map<uint16_t, RoutePtr> RouteMap_;
    std::map<std::string, TcpConnectionPtr> ConnectionMap_;

    muduo::net::TcpServer server_;
    muduo::net::EventLoop* loop_;
};