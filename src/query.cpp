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
#include <algorithm>
#include <stdexcept>

#include "query.hpp"

#define BASE_URL "https://api.twitch.tv/kraken/"

const unsigned int query::default_limit = 10;

query::query()
    : _client()
{
}

query::response query::channels(const std::string &name)
{
    throw_if_invalid_name(name);
    
    std::string url(BASE_URL "channels/");
    url += name;
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_CHANNELS, str);
}

query::response query::featured(unsigned int limit)
{
    throw_if_invalid_limit(limit);
    
    std::string url(BASE_URL "streams/featured?limit=");
    url += std::to_string(limit);
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_FEATURED, str);
}

query::response query::search_channels(const std::string &name, 
                                       unsigned int limit)
{
    throw_if_invalid_name(name);
    throw_if_invalid_limit(limit);
    
    std::string url(BASE_URL "search/channels?q=");
    url += name;
    url += "&limit=";
    url += std::to_string(limit);
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_SEARCH_C, str);
}

query::response query::search_games(const std::string &name, bool live)
{
    throw_if_invalid_name(name);
    
    std::string url(BASE_URL "search/games?q=");
    url += name;
    url += "&type=suggest&live=";
    url += (live) ? "true" : "false";
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_SEARCH_G, str);
}


query::response query::search_streams(const std::string &name, 
                                      unsigned int limit)
{
    throw_if_invalid_name(name);
    throw_if_invalid_limit(limit);
    
    std::string url(BASE_URL "search/streams?q=");
    url += name;
    url += "&limit=";
    url += std::to_string(limit);
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_SEARCH_S, str);
}

query::response query::streams(const std::string &name)
{
    throw_if_invalid_name(name);
    
    std::string url(BASE_URL "streams/");
    url += name;
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_STREAMS, str);
}

query::response query::top(unsigned int limit)
{
    throw_if_invalid_limit(limit);
    
    std::string url(BASE_URL "games/top?limit=");
    url += std::to_string(limit);
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_TOP, str);
}

void query::throw_if_invalid_name(const std::string &str)
{
    auto pred = [](char c) { return std::isspace(c); };
    
    auto it = std::find_if(str.begin(), str.end(), pred);
    
    if (it != str.end()) {
        std::string err_msg = "Invalid whitespace character in ";
        err_msg += "\"";
        err_msg += str;
        err_msg += "\""; 
        throw std::invalid_argument(err_msg);
    }
}


void query::throw_if_invalid_limit(unsigned int limit)
{
    const static unsigned int min = 1;
    const static unsigned int max = 100;
    
    if (limit < min || limit > max) {
        std::string err_msg = "Invalid limit \"";
        err_msg += std::to_string(limit);
        err_msg += "\": value must be of range [";
        err_msg += std::to_string(min);
        err_msg += ", ";
        err_msg += std::to_string(max);
        err_msg += "]";

        throw std::invalid_argument(err_msg);
    }
}

