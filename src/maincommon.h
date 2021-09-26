#pragma once

#include "media_server.h"
#include "muduo/base/Logging.h"
#include "muduo/base/Logging.h"
#include "muduo/base/LogFile.h"


class maincommon
{
public:
    maincommon();
    maincommon(int threadnum, int loglevel = 3, const char* logname = "NA", uint16_t restport = 8000, uint16_t minport = 10000, uint16_t maxport = 20000);

    int run();

    static int main(int argc, char** argv);

private:
friend  void logOutput(const char* msg, int len);
    bool                            inited_;
    muduo::net::EventLoop           mainloop_;
    std::unique_ptr<MediaServer>    server_;
};
