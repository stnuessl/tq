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

#include <string>
#include <utility>
#include <stdexcept>

#include <curl/curl.h>

#include "query.hpp"

bool query::_libcurl_init = false;

static size_t gather_response(char *p, 
                              size_t size, 
                              size_t nmemb, 
                              std::string *str)
{
    size_t total = size * nmemb;
    
    str->append(p, total);
    
    return total;
}

query::query(const std::string &url)
{
    if (!_libcurl_init) {
        auto err = curl_global_init(CURL_GLOBAL_SSL);
        
        if (err)
            throw std::runtime_error("curl_global_init() failed");
        
        _libcurl_init = true;
    }
    
    _curl = curl_easy_init();
    if (!_curl)
        throw std::runtime_error("curl_easy_init() failed");
    
    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &gather_response);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_response);
}

query::~query()
{
    curl_easy_cleanup(_curl);
}

std::string query::get_response()
{
    _response.clear();
    
    auto ok = curl_easy_perform(_curl);
    
    if (ok != CURLE_OK)
        throw std::runtime_error("curl_easy_perform() failed.");
    
    return std::move(_response);
}
