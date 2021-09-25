#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "muduo/net/Channel.h"
#include "muduo/net/Socket.h"
#include "muduo/net/InetAddress.h"

namespace mysockets
{
    typedef struct sockaddr SA;
    struct sockinfo
    {
        sockinfo(const uint16_t& local_port, const int &fd, const uint16_t& peer_port, const std::string& ip)
        :PerrAddr_(ip, peer_port), local_port_(local_port), sockfd_(fd) {}

        muduo::net::InetAddress PerrAddr_;
        uint16_t    local_port_;
        int         sockfd_;
    };
    int createNonblockingOrDieForUdp(sa_family_t family);
    ssize_t sendto(sockinfo info, const void *buf, size_t count); 
}
