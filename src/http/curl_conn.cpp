#include <functional>
#include <iostream>
#include <memory>

#include "curl/curl.h"
#include "curl_conn.h"

class OnConstruct
{
   public:
    OnConstruct(std::function<void()> func)
    {
        func();
    }

    ~OnConstruct() = default;
};

OnConstruct onConstruct([]() { curl_global_init(CURL_GLOBAL_ALL); });

CurlConn::CurlConn() : curl(curl_easy_init())
{
}

CurlConn::~CurlConn()
{
    curl_easy_cleanup(curl);
}

CURL* CurlConn::GetNativeHandle()
{
    return curl;
}