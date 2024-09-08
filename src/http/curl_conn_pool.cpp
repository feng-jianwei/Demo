#include <chrono>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "curl_conn_pool.h"
#include "timer/timer_mgr.h"

using namespace std::chrono_literals;
using std::chrono::duration_cast;
using std::chrono::steady_clock;

constexpr int DEFAULT_POOL_SIZE = 10;
constexpr int MAX_POOL_SIZE = 50;

CurlConnPool::CurlConnPool()
{
    for (int i = 0; i < DEFAULT_POOL_SIZE; i++) {
        connList.emplace(new CurlConn(), std::chrono::steady_clock::now());
    }
}

std::shared_ptr<CurlConn> CurlConnPool::GetCurlConn()
{
    {
        std::shared_lock lock(mtx);
        for (auto& conn : connList) {
            if (conn.first.use_count() == 1) {
                conn.second = std::chrono::steady_clock::now();
                return conn.first;
            }
        }
    }
    std::unique_lock lock(mtx);
    if (connList.size() < MAX_POOL_SIZE) {
        std::shared_ptr<CurlConn> conn(new CurlConn());
        connList.emplace(conn, std::chrono::steady_clock::now());
        return conn;
    }
    return nullptr;
}

int CurlConnPool::shrinkPool()
{
    std::unique_lock lock(mtx);
    auto currentTime = steady_clock::now();
    for (auto iter = connList.begin(); iter != connList.end();) {
        if (iter->first.use_count() == 1 && duration_cast<std::chrono::seconds>(currentTime - iter->second) > 60s) {
            iter = connList.erase(iter);
        } else {
            ++iter;
        }
    }
    return -1;
}

CurlConnPool::~CurlConnPool()
{
    connList.clear();
}