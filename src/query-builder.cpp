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
#include <unordered_map>
#include <stdexcept>

#include "query-builder.hpp"

const unsigned int query_builder::_default_limit = 10;
const unsigned int query_builder::_min_limit     = 1;
const unsigned int query_builder::_max_limit     = 100;

query_builder::query_builder(query_builder::type type)
    : _limit(_default_limit), 
      _live(true),
      _type(type)
{
}

query_builder::~query_builder()
{
}

std::string query_builder::as_string(query_builder::type type) 
{
#define BASE_URL "https://api.twitch.tv/kraken/"
    std::string s;
    
    switch (type) {
    case TYPE_CHANNELS:
        s = std::string(BASE_URL "channels/");
        break;
    case TYPE_FEATURED:
        s =  std::string(BASE_URL "streams/featured");
        break;
    case TYPE_SEARCH_CHANNELS:
        s = std::string(BASE_URL "search/channels");
        break;
    case TYPE_SEARCH_GAMES:
        s = std::string(BASE_URL "search/games");
        break;
    case TYPE_SEARCH_STREAMS:
        s = std::string(BASE_URL "search/streams");
        break;
    case TYPE_STREAMS:
        s = std::string(BASE_URL "streams/");
        break;
    case TYPE_TOP:
        s = std::string(BASE_URL "games/top");
        break;
    }
    
    return s;
}

void query_builder::set_query(const std::string& str)
{
    _query  = str;
}

void query_builder::set_limit(unsigned int limit)
{
    limit = std::min(limit, _max_limit);
    limit = std::max(limit, _min_limit);
    
    _limit = limit;
}

void query_builder::set_live(bool live)
{
    _live = live;
}

query query_builder::build() const
{
    auto url = as_string(_type);    
    auto limit = "limit=" + std::to_string(_limit);
    auto live  = std::string("&live=") + ((_live) ? "true" : "false"); 
    
    if (_type != TYPE_TOP && _type != TYPE_FEATURED && _query.empty())
        throw std::runtime_error("No query string specified.");
    
    switch (_type) {
        case TYPE_CHANNELS:
        case TYPE_STREAMS:
            url += _query;

            break;
        case TYPE_FEATURED:
        case TYPE_TOP:
            url += "?";
            url += limit;
            
            break;
        case TYPE_SEARCH_CHANNELS:
        case TYPE_SEARCH_STREAMS:
            url += "?q=";
            url += _query;
            url += "&";
            url += limit;
            
            break;
        case TYPE_SEARCH_GAMES:
            url += "?q=";
            url += _query;
            url += "&type=suggest";
            url += live;
            
            break;
    }

    return query(url);
}