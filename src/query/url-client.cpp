/*
 * Copyright (C) 2014  Steffen Nüssle
 * tq - Twitch Query
 *
 * This file is part of tq.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <utility>
#include <stdexcept>
#include <cctype>

#include "url-client.hpp"

#define CLIENT_ID "cdmq41iul8hs3ytq8i82p5s5g6ehyng"

static size_t gather_response(char *p, 
                              size_t size, 
                              size_t nmemb, 
                              std::string *str)
{
    size_t total = size * nmemb;
    
    str->append(p, total);
    
    return total;
}

url_client::url_client(const std::string &client_id)
    : url_client(client_id.c_str())
{
}

url_client::url_client(const char *client_id)
    : _header(),
      _response(),
      _curl(nullptr),
      _curl_slist(nullptr)
{
    curl_global::init();
    
    auto client_id_header = std::string("Client-ID: ");
    client_id_header += client_id;
    
    curl_slist_add("Accept: application/vnd.twitchtv.v3+json");
    curl_slist_add(client_id_header);

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

    for (auto &x : _header)
        x = std::tolower(x);
    
    if (_header.find("content-type: application/json") == std::string::npos)
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

void url_client::curl_slist_add(const std::string &info)
{
    curl_slist_add(info.c_str());
}

void url_client::curl_slist_add(const char *info)
{
    auto new_list = curl_slist_append(_curl_slist, info);
    if (!new_list)
        throw std::runtime_error("curl_slist_append() failed.");
    
    _curl_slist = new_list;
}

