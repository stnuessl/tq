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
#include <stdexcept>

#include "query-builder.hpp"

#define FILE_POSITION (std::string(__FILE__) + ":" + std::to_string(__LINE__))

const unsigned int tq::query_builder::_default_limit = 10;
const unsigned int tq::query_builder::_min_limit     = 1;
const unsigned int tq::query_builder::_max_limit     = 100;

static std::string type_to_string(tq::query_builder::type type)
{
    const static std::string str[] = {
        "channels/",
        "games/",
        "streams/",
        "search/channel",
        "search/games",
        "search/streams",
    };
    
    return str[type];
}

tq::query_builder::query_builder()
    : _limit(_default_limit), 
      _live(true)
{
}

tq::query_builder::~query_builder()
{
}

tq::query_builder &tq::query_builder::set_type(tq::query_builder::type type)
{
    _type = type;
    
    return *this;
}

tq::query_builder &tq::query_builder::set_query(const std::string& str)
{
    _query = str;
    
    return *this;
}

tq::query_builder &tq::query_builder::set_limit(unsigned int limit)
{
    limit = std::min(limit, tq::query_builder::_max_limit);
    limit = std::max(limit, tq::query_builder::_min_limit);
    
    _limit = limit;
    
    return *this;
}

tq::query_builder &tq::query_builder::set_live(bool live)
{
    _live = live;
    
    return *this;
}

tq::query tq::query_builder::build() const
{
    const static std::string base_url("https://api.twitch.tv/kraken/");
    std::string uri = base_url + type_to_string(_type);
    
    if (_type != GAMES && _query.empty())
        throw std::runtime_error(FILE_POSITION + "No query specified!");
    
    switch (_type) {
    case CHANNELS:
    case STREAMS:
        uri += _query;
        break;
    case GAMES:
        uri += "top?limit=" + std::to_string(_limit);
        break;
    case SEARCH_CHANNELS:
    case SEARCH_STREAMS:
        uri += "?q=" + _query + "&limit=" + std::to_string(_limit);
        break;
    case SEARCH_GAMES:
        uri += "?q=" + _query + "&type=suggest" + "&live=";
        uri += (_live) ? "true" : "false";
    default:
        throw std::runtime_error("Invalid query type");
    }
    
    return query(uri);
}