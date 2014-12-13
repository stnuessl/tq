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

#ifndef _RESPONSE_PRINTER_HPP_
#define _RESPONSE_PRINTER_HPP_

#include <unordered_map>
#include <iostream>
#include <utility>

#include <json/json.h>

#include "query.hpp"
#include "file.hpp"

class response_printer : public file {
public:
    explicit response_printer(const std::string &config, bool json = false,
                              bool verbose = false);
    
    void print_response(const query::response &response);
private:
    void print_channels(const Json::Value &val);
    void print_featured(const Json::Value &val);
    void print_search_channels(const Json::Value &val);
    void print_search_games(const Json::Value &val);
    void print_search_streams(const Json::Value &val);
    void print_streams(const Json::Value &val);
    void print_top(const Json::Value &val);
    
    void print_channel_full(const Json::Value &channel);
    void print_channel_short(const Json::Value &channel);
    void print_stream_full(const Json::Value &stream);
    void print_stream_short(const Json::Value &stream);
    void print_top_game(const Json::Value &top);
    
    typedef void (response_printer::*handler)(const Json::Value &);
        
    std::unordered_map<int, handler> _table;
    
    Json::Reader _reader;
    
    unsigned int _int_len;
    unsigned int _name_len;
    unsigned int _game_len;
    
    bool _json;
    bool _verbose;
};

#endif /* _RESPONSE_PRINTER_HPP_ */