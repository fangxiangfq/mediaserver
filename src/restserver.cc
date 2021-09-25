#include "restserver.h"

#include "muduo/base/Logging.h"


void defaultHttpCallback(const HttpRequest&, Buffer* buf, std::string* msg, rest::Code* code)
{
    *code = rest::Code::bad_request;
}

RestServer::RestServer(EventLoop* loop,
                       const InetAddress& listenAddr,
                       const string& name,
                       TcpServer::Option option)
  : server_(loop, listenAddr, name, option),
    httpCallback_(defaultHttpCallback)
{

    rest::JsonBuilder::msgmap_[rest::Code::success] = std::string("success");
    rest::JsonBuilder::msgmap_[rest::Code::bad_request] = std::string("bad request");
    rest::JsonBuilder::msgmap_[rest::Code::server_error] = std::string("server error");
    rest::JsonBuilder::msgmap_[rest::Code::server_full] = std::string("server full");
    server_.setConnectionCallback(
        std::bind(&RestServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&RestServer::onMessage, this, _1, _2, _3));
}

void RestServer::start()
{
    LOG_WARN << "RestServer[" << server_.name()
        << "] starts listening on " << server_.ipPort();
    server_.start();
}

void RestServer::onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
        conn->setContext(HttpContext());
    }
}

void RestServer::onMessage(const TcpConnectionPtr& conn,
                           Buffer* buf,
                           Timestamp receiveTime)
{
    HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());

    if (!context->parseRequest(buf, receiveTime))
    {
        std::string msg = rest::JsonBuilder(rest::Code::bad_request).toString();
        sendResponse(conn, msg, rest::Code::bad_request, true);
    }

    if (context->gotAll())
    {
        onRequest(conn, context->request(), buf);
        context->reset();
    }
}

void RestServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req, Buffer* buf)
{
    const std::string& connection = req.getHeader("Connection");
    bool close = connection == "close" ||  (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    std::string msg;
    rest::Code code;
    httpCallback_(req, buf, &msg, &code);

    sendResponse(conn, msg, code, close);
}

void RestServer::sendResponse(const TcpConnectionPtr& conn, std::string& str, rest::Code code, bool close) 
{
    HttpResponse rsp(close);
    if(rest::Code::success == code)
    {
        rsp.setStatusCode(HttpResponse::k200Ok);
        rsp.setStatusMessage("OK");
    }
    else if (rest::Code::bad_request == code)
    {
        rsp.setStatusCode(HttpResponse::k400BadRequest);
        rsp.setStatusMessage("ERR");
    }
    else
    {
        rsp.setStatusCode(HttpResponse::k301MovedPermanently);
        rsp.setStatusMessage("ERR");
    }

    rsp.setContentType("application/json");
    rsp.addHeader("Server", "Rest");
    
    rsp.setBody(str);

    Buffer buf;
    rsp.appendToBuffer(&buf);
    conn->send(&buf);

    if (rsp.closeConnection())
    {
        conn->shutdown();
    }
}

