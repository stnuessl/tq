/*
 * Copyright (C) 2016  Steffen Nüssle
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

#include "query-adapter.hpp"


query_adapter::query_adapter(const std::string &client_id)
    : query_adapter(client_id.c_str())
{
}

query_adapter::query_adapter(const char *client_id)
    : _json_reader(),
      _query(client_id)
{
}

std::unique_ptr<result> 
query_adapter::bookmarks(const std::vector<std::string> &channels)
{
    auto str = _query.streams(nullptr, &channels);
    
    return handle_response<bookmarks_result>(str);
}

std::unique_ptr<result> query_adapter::channels(const std::string &name)
{
    auto str = _query.channels(name);
    
    return handle_response<channels_result>(str);
}

std::unique_ptr<result> query_adapter::featured_streams(unsigned int limit)
{
    auto str = _query.featured_streams(limit);
    
    return handle_response<featured_streams_result>(str);
}

std::unique_ptr<result> query_adapter::search_channels(const std::string &query, 
                                                       unsigned int limit)
{
    auto str = _query.search_channels(query, limit);
    
    return handle_response<search_channels_result>(str);
}

std::unique_ptr<result> query_adapter::search_games(const std::string &query, 
                                                    bool live)
{
    auto str = _query.search_games(query, &live);
    
    return handle_response<search_games_result>(str);
}

std::unique_ptr<result> query_adapter::search_streams(const std::string &query, 
                                                      unsigned int limit)
{
    auto str = _query.search_streams(query, limit);
    
    return handle_response<search_streams_result>(str);
}

std::unique_ptr<result> query_adapter::streams(const std::string &game, 
                                               unsigned int limit)
{
    auto str = _query.streams(&game, nullptr, limit);
    
    return handle_response<streams_result>(str);
}

std::unique_ptr<result> 
query_adapter::streams(const std::vector<std::string> &channels)
{
    auto str = _query.streams(nullptr, &channels);
    
    return handle_response<streams_result>(str);
}

std::unique_ptr<result> query_adapter::top_games(unsigned int limit)
{
    auto str = _query.top_games(limit);
    
    return handle_response<top_games_result>(str);
}

std::unique_ptr<result> query_adapter::users(const std::string &name)
{
    auto str = _query.users(name);
    
    return handle_response<users_result>(str);
}


template <typename T>
std::unique_ptr<result> query_adapter::handle_response(const std::string &str)
{
    Json::Value val;
    
    auto ok = _json_reader.parse(str, val, false);
    if (!ok)
        throw std::runtime_error("Json::Reader::parse() failed");
    
    if (val.isMember("error")) {
        auto unique = std::make_unique<error_result>();
        unique->set_json_value(std::move(val));
        
        return std::move(unique);
    }
    
    auto unique = std::make_unique<T>();
    unique->set_json_value(std::move(val));
    
    return std::move(unique);
}

