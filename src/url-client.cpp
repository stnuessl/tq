
#include <string>
#include <utility>
#include <stdexcept>

#include <curl/curl.h>

#include "url-client.hpp"

bool url_client::_libcurl_init = false;

static size_t gather_response(char *p, 
                              size_t size, 
                              size_t nmemb, 
                              std::string *str)
{
    size_t total = size * nmemb;
    
    str->append(p, total);
    
    return total;
}

url_client::url_client(const std::string &url)
: _curl_slist(nullptr)
{
    const static char header[] = "Accept: application/vnd.twitchtv.v2+json";
    
    if (!_libcurl_init) {
        auto err = curl_global_init(CURL_GLOBAL_SSL);
        
        if (err)
            throw std::runtime_error("curl_global_init() failed.");
        
        _libcurl_init = true;
    }
    
    _curl_slist = curl_slist_append(_curl_slist, header);
    if (!_curl_slist)
        throw std::runtime_error("curl_slist_append() failed.");
    
    _curl = curl_easy_init();
    if (!_curl)
        throw std::runtime_error("curl_easy_init() failed.");
    
    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _curl_slist);
    curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &gather_response);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_response);
}

url_client::~url_client()
{
    curl_easy_cleanup(_curl);
    curl_slist_free_all(_curl_slist);
}

std::string url_client::get_response()
{
    _response.clear();
    
    auto ok = curl_easy_perform(_curl);
    
    if (ok != CURLE_OK) {
        std::string err("curl_easy_perform() failed - ");
        err += "(";
        err += std::to_string(ok);
        err += ") ";
        err += curl_easy_strerror(ok);
        throw std::runtime_error(err);
    }
    return std::move(_response);
}