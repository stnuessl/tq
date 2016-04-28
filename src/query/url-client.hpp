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

#ifndef _URL_CLIENT_HPP_
#define _URL_CLIENT_HPP_

#include <string>
#include <memory>

#include <curl/curl.h>

class url_client {
public:
    url_client();
    url_client(url_client &client) = delete;
    ~url_client();
    
    std::string get_response(const std::string &url);
    
    url_client &operator=(const url_client &client) = delete;
    
private:
    class curl_global {
    public:
        static void init();
        
    private:
        explicit curl_global();
        ~curl_global();
    };
    
    std::string _header;
    std::string _response;
    CURL *_curl;
    struct curl_slist *_curl_slist;
};

#endif /* _URL_CLIENT_HPP_ */