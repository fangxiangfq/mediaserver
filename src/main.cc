#include "media_server.h"
#include "muduo/base/Logging.h"
#include "muduo/base/Logging.h"
#include "muduo/base/LogFile.h"

std::unique_ptr<muduo::LogFile> g_logFile;

void LogOutput(const char* msg, int len)
{
    if (g_logFile)
    {
        g_logFile->append(msg, len);
    }
}

int main(int argc, char** argv)
{

    g_logFile.reset(new muduo::LogFile("ms", 500*1000*1000, false));
    muduo::Logger::setOutput(LogOutput);
    Logger::setLogLevel(Logger::INFO);
    LOG_INFO << "pid = " << getpid();
    muduo::net::EventLoop loop;
    MediaServer server(&loop, 1, 8000);
    
    server.start();
    loop.loop();

    return 0;
}