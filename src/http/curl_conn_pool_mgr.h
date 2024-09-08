#ifndef __HTTP_CURL_CONN_POOL_MGR__
#define __HTTP_CURL_CONN_POOL_MGR__

#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include "curl_conn_pool.h"

class CurlConnPoolMgr {
public:
    static CurlConnPoolMgr &GetInstance();
    std::shared_ptr<CurlConn> GetCurlConn(std::string_view poolName = "default");

private:
    CurlConnPoolMgr();
    ~CurlConnPoolMgr();
    std::shared_ptr<CurlConnPool> CreatePool(std::string_view poolName);
    std::shared_mutex mtx;
    std::unordered_map<std::string, std::shared_ptr<CurlConnPool>> poolMap;
};

#endif /* __HTTP_CURL_CONN_POOL_MGR__ */
