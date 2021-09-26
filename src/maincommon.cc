#include "maincommon.h"
#include <iostream>
#include <memory>

std::unique_ptr<muduo::LogFile> g_logFile_;

void logOutput(const char* msg, int len)
{
    if (g_logFile_)
        g_logFile_->append(msg, len);
}

maincommon::maincommon()
:inited_(false), mainloop_(), server_(new MediaServer(&mainloop_))
{
    Logger::setLogLevel(Logger::WARN);
    LOG_INFO << "pid = " << getpid();
    inited_ = true;
}

//todo para check
maincommon::maincommon(int threadnum, int loglevel, const char* logname, uint16_t restport, uint16_t minport, uint16_t maxport) 
:inited_(false), mainloop_(), server_(new MediaServer(&mainloop_, threadnum, restport, minport, maxport))
{
    Logger::setLogLevel(static_cast<Logger::LogLevel>(loglevel));
    if(logname != NULL && 0 != strncmp(logname, "NA", 10))
        g_logFile_.reset(new muduo::LogFile(logname, 500*1000*1000, false));
    muduo::Logger::setOutput(logOutput);
    LOG_INFO << "pid = " << getpid();
    inited_ = true;
}

int maincommon::run() 
{
    if(inited_)
    {
        server_->start();
        mainloop_.loop();
        return 0;
    }
    return -1;
}

int maincommon::main(int argc, char** argv) 
{
    std::unique_ptr<maincommon> main_;
    if(argc <= 1)
    {
        main_.reset(new maincommon());
        return main_->run();
        
    }
    else if(0 == strncmp(argv[1], "-h", 2))
    {
        std::cout << "input media_sever with part or all of args [threadnum(0-10) loglevel(0-5) logname(NA for stdout) restport minport maxport] please!\n" ;
        return 0;
    }
    switch (argc)
    {
    case 2:
        main_.reset(new maincommon(std::stoi(argv[1])));
        break;
    case 3:
        main_.reset(new maincommon(std::stoi(argv[1]), std::stoi(argv[2])));
        break;
    case 4:
        main_.reset(new maincommon(std::stoi(argv[1]), std::stoi(argv[2]), argv[3]));
        break;
    case 5:
        main_.reset(new maincommon(std::stoi(argv[1]), std::stoi(argv[2]), argv[3], static_cast<uint16_t>(std::stoi(argv[4]))));
        break;
    case 6:
        main_.reset(new maincommon(std::stoi(argv[1]), std::stoi(argv[2]), argv[3], static_cast<uint16_t>(std::stoi(argv[4])), static_cast<uint16_t>(std::stoi(argv[5]))));
        break;
    default:
        main_.reset(new maincommon(std::stoi(argv[1]), std::stoi(argv[2]), argv[3], static_cast<uint16_t>(std::stoi(argv[4])), static_cast<uint16_t>(std::stoi(argv[5])), static_cast<uint16_t>(std::stoi(argv[6]))));
        break;
    }
    return main_->run();
}
