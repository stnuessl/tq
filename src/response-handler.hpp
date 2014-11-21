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

#ifndef _RESPONSE_HANDLER_HPP_
#define _RESPONSE_HANDLER_HPP_

#include <string>
#include <unordered_map>

#include <json/json.h>

#include "query-builder.hpp"

class response_handler {
public:
    explicit response_handler(bool v, bool u);
    ~response_handler();
    
    void handle_response(const std::string &str, query_builder::type type);
private:
    void handle_channels(const Json::Value &root);
    void handle_featured(const Json::Value &root);
    void handle_search_channels(const Json::Value &root);
    void handle_search_games(const Json::Value &root);
    void handle_search_streams(const Json::Value &root);
    void handle_streams(const Json::Value &root);
    void handle_top(const Json::Value &root);
    
    typedef void (response_handler::*handler)(const Json::Value &);
        
    std::unordered_map<int, handler> _table;
    
    Json::Reader _reader;
    
    int _max_name_str_len;
    int _max_game_str_len;
   
    bool _v;
    bool _u;
};

#endif /* _RESPONSE_HANDLER_HPP_ */