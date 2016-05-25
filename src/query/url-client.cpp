
#include <utility>
#include <stdexcept>

#include "url-client.hpp"

static size_t gather_response(char *p, 
                              size_t size, 
                              size_t nmemb, 
                              std::string *str)
{
    size_t total = size * nmemb;
    
    str->append(p, total);
    
    return total;
}

url_client::url_client()
    : _header(),
      _response(),
      _curl(nullptr),
      _curl_slist(nullptr)
{
    static const char *headers[] = {
        "Accept: application/vnd.twitchtv.v3+json",
        "Client-ID: tq",
        NULL
    };
    
    curl_global::init();
    
    for (int i = 0; headers[i]; ++i) {
        auto new_list = curl_slist_append(_curl_slist, headers[i]);
        if (!new_list) {
            curl_slist_free_all(_curl_slist);
            throw std::runtime_error("curl_slist_append() failed.");
        }
        
        _curl_slist = new_list;
    }
    
    _curl = curl_easy_init();
    if (!_curl) {
        curl_slist_free_all(_curl_slist);
        throw std::runtime_error("curl_easy_init() failed.");
    }
    
    int err = 0;
    err |= curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _curl_slist);
    err |= curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
    err |= curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &gather_response);
    err |= curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &_header);
    err |= curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_response);
    if (err) {
        curl_easy_cleanup(_curl);
        curl_slist_free_all(_curl_slist);
        
        throw std::runtime_error("curl_easy_setopt() failed.");
    }
}

url_client::~url_client()
{
    curl_easy_cleanup(_curl);
    curl_slist_free_all(_curl_slist);
}

std::string url_client::get_response(const std::string &url)
{
    _header.clear();
    _response.clear();
    
    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    
    auto ok = curl_easy_perform(_curl);
    
    if (ok != CURLE_OK) {
        std::string err("curl_easy_perform() failed - ");
        err += "(";
        err += std::to_string(ok);
        err += ") ";
        err += curl_easy_strerror(ok);
 
        throw std::runtime_error(err);
    }
    
    if (_header.find("Content-Type: application/json") == std::string::npos)
        throw std::runtime_error("Server sent invalid MIME type:\n" + _header);
        
    return std::move(_response);
}

url_client::curl_global::curl_global()
{
    auto err = curl_global_init(CURL_GLOBAL_SSL);
    if (err)
        throw std::runtime_error("Failed to initialize curl_global");
}

url_client::curl_global::~curl_global()
{
    curl_global_cleanup();
}

void url_client::curl_global::init()
{
    static curl_global curl_global;
}
