#include "media_server.h"
#include "muduo/base/Logging.h"


MediaServer::MediaServer(muduo::net::EventLoop* loop,
                       const muduo::net::InetAddress& listenAddr)
    : server_(loop, listenAddr, "MediaServer")
{
    server_.setConnectionCallback(
        std::bind(&MediaServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&MediaServer::onMessage, this, _1, _2, _3));
}

void MediaServer::start()
{
    server_.start();
}

void MediaServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{ 
    if(0 == PortMap_.count(conn->localAddress.port())
    {
        LOG_INFO << "MediaServer - Not Registerd" << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");
        server.removeConnection(conn);
        return;
    }
    else
    {
        TerminalPtr ptr = PortMap_[conn->localAddress.port()];
        if(ptr->conn() || ptr->ip() != conn->localAddress().toIp() || ptr->port() != conn->localAddress().toPort())
        {
            LOG_INFO << "MediaServer - Not Matched" << conn->peerAddress().toIpPort() << " -> "
                << conn->localAddress().toIpPort() << " is "
                << (conn->connected() ? "UP" : "DOWN");
            server.removeConnection(conn);
            return;
        }

        Ptr->set_peer(conn->perrAddress().toIp(), conn->perrAddress().toPort(), conn->getFd());
        Ptr->set_conn(true);

        // Terminal.insert(make_pair(Ptr->terno, Ptr);
    }

}

void MediaServer::onMessage(const muduo::net::TcpConnectionPtr& conn)
{
    muduo::net::Eventloop* workloop = server_.threadPool()->getNextLoop();
    muduo::net::Eventloop* mainloop = get_loop();
    if(0 == RouteMap_.count(conn->localAddress.port())
    {
        LOG_INFO << "MediaServer - Not Routed" << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort();
        return;
    }

    RoutePtr ptr = RouteMap_[conn->localAddress.port()];

    std::vector<int> dst;
    int src = conn.getFd();
    
    for(auto it = ptr->begin(); it < ptr->end(); ++it)
    {
        TerminalPtr ptr = PortMap_[*it];
        dst.push_back(ptr->fd());
    }

    workloop->runInLoop([int src, std::vector<int> dst], loloop]()
    {
        int savedErrno = 0;
        Buffer inputBuffer;
        ssize_t n = inputBuffer.readFd(channel_->fd(), &savedErrno);
    
        if (n > 0)
        {
            for(auto it = dst.begin(); it < dst.end(); ++it)
            {
                ssize_t nwrote = sockets::write(*it, data, len);
                if(nwrote < 0)
                {
                    mainloop.runInLoop(std::bind(&TcpConnection::forceClose, &conn));
                }
            }
        }
        else if (n <= 0)
        {
            mainloop.runInLoop(std::bind(&TcpConnection::forceClose, &conn));
        }

    });
}