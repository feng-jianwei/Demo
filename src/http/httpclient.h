#ifndef __HTTP_HTTPCLIENT__
#define __HTTP_HTTPCLIENT__
#include <future>
#include <string>
#include <unordered_map>
#include "template/result.h"

enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE
};

class HttpRequest {
public:
    class ReqBuilder {
    public:
        ReqBuilder();
        ~ReqBuilder();
        ReqBuilder &SetUrl(const std::string &url);
        ReqBuilder &SetMethod(enum HttpMethod method);
        ReqBuilder &SetHeader(const std::string &key, const std::string &value);
        ReqBuilder &SetHeaders(const std::unordered_multimap<std::string, std::string> &headers);
        ReqBuilder &SetBody(const std::string &body);
        ReqBuilder &SetTimeout(int timeout);
        ReqBuilder &SetConnectTimeout(int timeout);
        HttpRequest Build();
    private:
        HttpRequest *req;
    };
    std::string GetUrl() const;
    std::string GetBody() const;
    int GetTimeout() const;
    std::unordered_multimap<std::string, std::string> GetHeaders() const;
    enum HttpMethod GetMethod() const;
    int GetConnectTimeout() const;
private:
    std::string url;
    std::string body;
    int timeout;
    int connectTimeout;
    std::unordered_multimap<std::string, std::string> headers;
    enum HttpMethod method;
    friend class ReqBuilder;
};

class HttpResponse {
public:
    HttpResponse();
    ~HttpResponse();
    void SetStatusCode(int code);
    void SetBody(const std::string &body);
    void SetHeader(const std::string &key, const std::string &value);
    int GetStatusCode();
    std::string GetBody();
    std::string GetHeader(const std::string &key);
private:
    std::string body;
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    Result<std::future<HttpResponse>> AddRequest(const HttpRequest &request);
    void PackSendMsg();
private:
    class HttpClientImpl *impl;
};

#endif /* __HTTP_HTTPCLIENT__ */
