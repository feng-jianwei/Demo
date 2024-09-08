#ifndef __HTTP_CURL_CONN__
#define __HTTP_CURL_CONN__

#include "curl/curl.h"

class CurlConn {
public:
    CurlConn();
    ~CurlConn();
    CURL *GetNativeHandle();
private:
    CURL *curl;
};

#endif /* __HTTP_CURL_CONN__ */
