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

static const std::string &stream_type_str(enum query::stream_type stream_type)
{
    static const std::string strings[] = {
        "all",
        "playlist",
        "live",
    };
    
    return strings[stream_type];
}

static void throw_if_invalid_name(const std::string &str)
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


static void throw_if_invalid_limit(unsigned int limit)
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

query::query()
    : _client(),
      _uri()
{
    _uri.reserve(1024);
}

std::string query::channels(const std::string &name)
{
    throw_if_invalid_name(name);
    
    _uri = BASE_URL "channels/";
    _uri += name;
    
    return _client.get_response(_uri);
}

std::string query::featured_streams(unsigned int limit, unsigned int offset)
{
    throw_if_invalid_limit(limit);
    
    _uri = BASE_URL "streams/featured?limit=";
    _uri += std::to_string(limit);
    _uri += "&offset=";
    _uri += std::to_string(offset);
    
    return _client.get_response(_uri);
}

std::string query::search_channels(const std::string &query, 
                                   unsigned int limit, 
                                   unsigned int offset)
{
    throw_if_invalid_limit(limit);
    
    _uri = BASE_URL "search/channels?q=";
    _uri += query;
    _uri += "&limit=";
    _uri += std::to_string(limit);
    _uri += "&offset=";
    _uri += std::to_string(offset);
    
    return _client.get_response(_uri);
}

std::string query::search_games(const std::string &query, const bool *live)
{
    _uri = BASE_URL "search/games?q=";
    _uri += query;
    _uri += "&type=suggest";
    
    if (live) {
        _uri += "&live=";
        _uri += (*live) ? "true" : "false";
    }
    
    return _client.get_response(_uri);
}

std::string query::search_streams(const std::string &query, 
                                  unsigned int limit, 
                                  unsigned int offset, 
                                  const bool *hls)
{
    throw_if_invalid_limit(limit);
    
    _uri = BASE_URL "search/streams?q=";
    _uri += query;
    _uri += "&limit=";
    _uri += std::to_string(limit);
    _uri += "&offset=";
    _uri += std::to_string(offset);
    
    if (hls) {
        _uri += "&hls=";
        _uri += (*hls) ? "true" : "false";
    }
    
    return _client.get_response(_uri);
}

std::string query::streams(const std::string *game, 
                           const std::vector<std::string> *channels, 
                           const unsigned int limit, 
                           const unsigned int offset, 
                           const std::string *client_id, 
                           query::stream_type *stream_type)
{
    throw_if_invalid_limit(limit);
    
    _uri = BASE_URL "streams?";
    
    if (game && !game->empty()) {
        throw_if_invalid_name(*game);
        
        _uri += "game=";
        _uri += *game;
        _uri += '&';
    }
    
    if (channels && !channels->empty()) {
        _uri += "channel=";
        
        for (const auto &x : *channels) {
            throw_if_invalid_name(x);
            
            _uri += x;
            _uri += ',';
        }
        
        /* Remove last ',' */
        _uri.pop_back();
        _uri += '&';
    }
    
    _uri += "limit=";
    _uri += std::to_string(limit);
    _uri += "&offset=";
    _uri += std::to_string(offset);
    
    if (client_id && !client_id->empty()) {
        _uri += "&client_id=";
        _uri += *client_id;
    }
    
    if (stream_type) {
        _uri += "&stream_type=";
        _uri += stream_type_str(*stream_type);
    }
    
    return _client.get_response(_uri);
}

std::string query::top_games(unsigned int limit, unsigned int offset)
{
    throw_if_invalid_limit(limit);
    
    _uri = BASE_URL "games/top?limit=";
    _uri += std::to_string(limit);
    _uri += "&offset=";
    _uri += std::to_string(offset);
    
    return _client.get_response(_uri);
}

std::string query::users(const std::string &name)
{
    throw_if_invalid_name(name);
    
    _uri = BASE_URL "users/";
    _uri += name;
    
    return _client.get_response(_uri);
}
