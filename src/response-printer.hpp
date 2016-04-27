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
#include <memory>

#include <json/json.h>

#include "query.hpp"
#include "file.hpp"
#include "config.hpp"

class response_printer {
public:
    static const unsigned int default_max_integer_length;
    static const unsigned int default_max_name_length;
    static const unsigned int default_max_game_length;

    response_printer();
    
    void set_max_integer_length(unsigned int len);
    void set_max_name_length(unsigned int len);
    void set_max_game_length(unsigned int len);
    void set_section(bool val);
    void set_verbose(bool val);
    void set_descriptive(bool val);
    void set_json(bool val);
    
    void print_response(const query::response &response);
    
private:
    void print_channels(const Json::Value &val);
    void print_featured(const Json::Value &val);
    void print_search_channels(const Json::Value &val);
    void print_search_games(const Json::Value &val);
    void print_search_streams(const Json::Value &val);
    void print_streams(const Json::Value &val);
    void print_top(const Json::Value &val);
    void print_user(const Json::Value &user);
    
    void print_channel_full(const Json::Value &channel);
    void print_channel_short(const Json::Value &channel);
    void print_stream_full(const Json::Value &stream);
    void print_stream_short(const Json::Value &stream);
    void print_top_game(const Json::Value &top);
    
    void print_channel_array(const Json::Value &array);
    void print_stream_array(const Json::Value &array);
    
    void print_channel_short_header() const;
    void print_stream_short_header() const;
    void print_top_game_header() const;
    void print_game_header() const;
    
    Json::Reader _reader;
    
    unsigned int _int_len;
    unsigned int _name_len;
    unsigned int _game_len;
    
    bool _section;
    bool _verbose;
    bool _descriptive;
    bool _json;
};

#endif /* _RESPONSE_PRINTER_HPP_ */