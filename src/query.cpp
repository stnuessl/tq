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


query::query()
    : _client(),
      _name(),
      _limit(10),
      _live(false)
{

}

query::~query()
{
    
}

void query::set_name(const std::string &str)
{
    _name = str;
}

void query::set_limit(unsigned int limit)
{
    const static unsigned int min = 1;
    const static unsigned int max = 100;
    
    limit = std::max(limit, min);
    limit = std::min(limit, max);
    
    _limit = limit;
}

void query::set_live(bool live)
{
    _live = live;
}

std::pair<query::type, std::string> query::get_response(query::type type)
{
    if (type != TYPE_TOP && type != TYPE_FEATURED && _name.empty())
        throw std::runtime_error("No string to query specified.");
    
    std::string limit = "limit=";
    std::string live  = "live=";
    
    limit += std::to_string(_limit);
    live  += (_live) ? "true" : "false";
    
    auto url = base_url(type);
    
    switch (type) {
    case TYPE_CHANNELS:
    case TYPE_STREAMS:
        url += _name;
        break;
    case TYPE_FEATURED:
    case TYPE_TOP:
        url += "?";
        url += limit;
        break;
    case TYPE_SEARCH_C:
    case TYPE_SEARCH_S:
        url += "?q=";
        url += _name;
        url += "&";
        url += limit;
        break;
    case TYPE_SEARCH_G:
        url += "?q=";
        url += _name;
        url += "&type=suggest&";
        url += live;
        break;
    default:
        throw std::runtime_error("Invalid query::type specified.");
    }
    
    _client.set_url(url);
    
    return std::make_pair(type, _client.get_response());
}

std::string query::base_url(query::type type)
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
        case TYPE_SEARCH_C:
            s = std::string(BASE_URL "search/channels");
            break;
        case TYPE_SEARCH_G:
            s = std::string(BASE_URL "search/games");
            break;
        case TYPE_SEARCH_S:
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

