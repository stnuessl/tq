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
    std::string url(BASE_URL "channels/");
    url += name;
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_CHANNELS, str);
}

query::response query::featured(unsigned int limit)
{
    std::string url(BASE_URL "streams/featured?limit=");
    url += valid_limit_str(limit);
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_FEATURED, str);
}

query::response query::search_channels(const std::string &name, 
                                       unsigned int limit)
{
    std::string url(BASE_URL "search/channels?q=");
    url += name;
    url += "&limit=";
    url += valid_limit_str(limit);
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_SEARCH_C, str);
}

query::response query::search_games(const std::string &name, bool live)
{
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
    std::string url(BASE_URL "search/streams?q=");
    url += name;
    url += "&limit=";
    url += valid_limit_str(limit);
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_SEARCH_S, str);
}

query::response query::streams(const std::string &name)
{
    std::string url(BASE_URL "streams/");
    url += name;
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_STREAMS, str);
}

query::response query::top(unsigned int limit)
{
    std::string url(BASE_URL "games/top?limit=");
    url += valid_limit_str(limit);
    
    auto str = _client.get_response(url);
    
    return response(query::TYPE_TOP, str);
}

const std::string query::valid_limit_str(unsigned int limit)
{
    const static unsigned int min = 1;
    const static unsigned int max = 100;
    
    /* A valid limit value has to be in range [1, 100] */
    limit = std::max(limit, min);
    limit = std::min(limit, max);
    
    return std::to_string(limit);
}

