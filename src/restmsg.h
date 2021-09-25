#include "json/json.h"
#include <string>
#include <map>
#include <memory>
#include "muduo/net/http/HttpResponse.h"
namespace rest
{
    enum class Code
    {
        success = 0,

        bad_request = 1000,
        server_error,
        server_full
    };

    using MsgMap = std::map<Code, std::string>;

    class JsonParser
    {
    public:
        JsonParser(std::string& body);

        bool CreateParse(std::string& terno, std::string& ip, uint16_t& port);
        bool DeleteParse(std::string& terno);
        bool RouteParse(std::string& src, std::vector<std::string>& dst);
    private:
        bool parse_;
        Json::Value root_;
    };

    class JsonBuilder
    {
    public:
        JsonBuilder(Code code);
        JsonBuilder(Code code, std::string key, uint16_t value);
        JsonBuilder(Code code, std::string key, std::string str);

        std::string toString();
        static MsgMap msgmap_;
    private:
        Json::Value root_;
    };

}