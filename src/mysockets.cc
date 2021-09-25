#include "muduo/base/Logging.h"
#include "mysockets.h"
#include <iostream>

namespace mysockets
{
    int createNonblockingOrDieForUdp(sa_family_t family) 
    {
        int sockfd = ::socket(family, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_UDP);
        if (sockfd < 0)
        {
            LOG_SYSFATAL << "sockets::createNonblockingOrDie";
        }
        return sockfd;
    }
    
    ssize_t sendto(sockinfo info, const void *buf, size_t count) 
    {
        std::cout << info.PerrAddr_.toIpPort() << " fd " << info.sockfd_ << std::endl;
        return ::sendto(info.sockfd_, buf, count, 0, info.PerrAddr_.getSockAddr(), sizeof(SA));
    }
}