#include <future>

#include "curl_conn.h"
#include "curl_conn_pool_mgr.h"
#include "httpclient.h"

using namespace std;

class ReqToCurl {
    bool ConvertRequestToCurl(const HttpRequest& request, CURL* curl);
    struct curl_slist* GenerateHeader(const HttpRequest& request);
};

class HttpClientImpl {
public:
    HttpClientImpl();
    ~HttpClientImpl();
    Result<std::future<HttpResponse>> AddRequest(const HttpRequest& request);
    
    void PackSendMsg();
private:
    std::list<std::shared_ptr<CurlConn>> connList;

private:
    CURLM* multiHandle;
    std::promise<HttpResponse> rspPromise;
};

HttpClientImpl::HttpClientImpl() : multiHandle(curl_multi_init())
{}

HttpClientImpl::~HttpClientImpl()
{
    curl_multi_cleanup(multiHandle);
}

string CovertMethodToCurl(HttpMethod method)
{
    switch (method) {
        case HttpMethod::GET:
            return "GET";
        case HttpMethod::POST:
            return "POST";
        case HttpMethod::PUT:
            return "PUT";
        case HttpMethod::DELETE:
            return "DELETE";
        default:
            return "GET";
    }
}

struct curl_slist* ReqToCurl::GenerateHeader(const HttpRequest& request)
{
    struct curl_slist* header = nullptr;
    string headerStr;
    for (const auto& kv : request.GetHeaders()) {
        headerStr = kv.first + ": " + kv.second;
        header = curl_slist_append(header, headerStr.c_str());
    }
    return header;
}

bool ReqToCurl::ConvertRequestToCurl(const HttpRequest& request, CURL* curl)
{
    curl_easy_setopt(curl, CURLOPT_URL, request.GetUrl().c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, request.GetTimeout());
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, request.GetConnectTimeout());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, GenerateHeader(request));
    curl_easy_setopt(curl, CURLOPT_HEADER, request.GetHeader());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, request.GetVerbose());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request.GetHttpHeaders());
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, request.GetPostData());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.GetPostFields().c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.GetPostFields().size());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, request.GetPostFields().size());
    curl_easy_setopt(curl, CURLOPT_READDATA, request.GetReadData());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, request.GetReadFunction());
    curl_easy_setopt(curl, CURLOPT_UPLOAD, request.GetUpload());
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, request.GetInFileSize());
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, request.GetInFileSize());
    curl_easy_setopt(curl, CURLOPT_WRITEHEADER, request.GetWriteHeader());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, request.GetWriteFunction());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, request.GetWriteData());
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, request.GetHeaderData());
    curl_easy_setopt(curl, CURLOPT_RESUME_FROM, request.GetResumeFrom());
}


size_t CurlWriteCB(char* buffer, size_t size, size_t nitems, void* outstream)
{
    auto rsp = static_cast<std::promise<HttpResponse>*>(outstream);
    HttpResponse response;
    response.SetBody(std::string(buffer, size * nitems));
    rsp->set_value(std::move(response));
    return size * nitems;
}

Result<std::future<HttpResponse>> HttpClientImpl::AddRequest(const HttpRequest& request)
{
    auto conn = CurlConnPoolMgr::GetInstance().GetCurlConn();
    if (!conn) {
        return false;
    }
    connList.emplace_back(std::move(conn));
    if (!ConvertRequestToCurl(request, conn->GetNativeHandle())) {
        return false;
    }
    curl_easy_setopt(conn->GetNativeHandle(), CURLOPT_WRITEFUNCTION, CurlWriteCB);
    curl_easy_setopt(conn->GetNativeHandle(), CURLOPT_WRITEDATA, &rspPromise);
    return rspPromise.get_future();
}

HttpClient::HttpClient()
{
    impl = new HttpClientImpl();
}

HttpClient::~HttpClient()
{
    delete impl;
}