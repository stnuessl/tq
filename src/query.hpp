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

#ifndef _QUERY_HPP_
#define _QUERY_HPP_

#include <string>

#include <curl/curl.h>

class query {
public:
    explicit query(const std::string &url);
    ~query();
    
    std::string get_response();
private:
    static bool _libcurl_init;
    
    std::string _response;
    CURL *_curl;
    struct curl_slist *_curl_slist;
};

#endif /* _QUERY_HPP_ */