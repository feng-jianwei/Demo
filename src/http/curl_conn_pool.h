#ifndef __HTTP_CURL_CONN_POOL__
#define __HTTP_CURL_CONN_POOL__
#include <chrono>
#include <list>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "curl/curl.h"
#include "curl_conn.h"

class CurlConnPool {
   public:
    CurlConnPool();
    ~CurlConnPool();
    std::shared_ptr<CurlConn> GetCurlConn();
    int shrinkPool();

   private:
    std::shared_mutex mtx;
    std::unordered_multimap<std::shared_ptr<CurlConn>, std::chrono::steady_clock::time_point> connList;
};

#endif /* __HTTP_CURL_CONN_POOL__ */
