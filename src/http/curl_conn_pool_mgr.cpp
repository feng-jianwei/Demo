#include <memory>
#include <mutex>
#include <string>

#include "curl_conn_pool_mgr.h"
#include "timer/timer_mgr.h"

using namespace std::chrono_literals;
constexpr std::string_view DEFAULT_POOL_NAME = "default";

CurlConnPoolMgr& CurlConnPoolMgr::GetInstance()
{
    static CurlConnPoolMgr instance;
    return instance;
}

CurlConnPoolMgr::CurlConnPoolMgr()
{
    poolMap.emplace(DEFAULT_POOL_NAME.data(), new CurlConnPool());
}

CurlConnPoolMgr::~CurlConnPoolMgr() = default;

std::shared_ptr<CurlConn> CurlConnPoolMgr::GetCurlConn(std::string_view poolName)
{
    std::shared_lock lock(mtx);
    auto iter = poolMap.find(poolName.data());
    if (iter == poolMap.end()) {
        lock.unlock();
        auto pool = CreatePool(poolName);
        if (!pool) {
            return nullptr;
        }
        return pool->GetCurlConn();
    }
    return iter->second->GetCurlConn();
}

std::shared_ptr<CurlConnPool> CurlConnPoolMgr::CreatePool(std::string_view poolName)
{
    std::unique_lock lock(mtx);
    auto iter = poolMap.find(poolName.data());
    if (iter != poolMap.end()) {
        return iter->second;
    }
    auto pool = std::make_shared<CurlConnPool>();
    TimerMgr::GetInstance().AddTimer([pool]() { pool->shrinkPool(); }, 0s, 10s);
    poolMap.emplace(poolName.data(), pool);
    return pool;
}