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

#ifndef _QUERY_RESULTS_HPP_
#define _QUERY_RESULTS_HPP_

#include <iostream>
#include <ostream>

#include <json/json.h>

class result {
public:
    result();
    virtual ~result() = default;
    
    void set_json_value(Json::Value &&val);
    
    void set_integer_length(unsigned int len);
    void set_name_length(unsigned int len);
    void set_game_length(unsigned int len);
    
    void set_descriptive(bool val);
    void set_section(bool val);
    void set_verbose(bool val);
    
    void dump_json(std::ostream &out = std::cout) const;
    virtual void dump(std::ostream &out = std::cout) const = 0;
protected:
    void dump_channel_header(std::ostream &out) const;
    void dump_game_header(std::ostream &out) const;
    void dump_stream_header(std::ostream &out) const;
    void dump_top_header(std::ostream &out) const;
    
    void dump_channel(std::ostream &out, const Json::Value &val) const;
    void dump_channel_full(std::ostream &out, const Json::Value &val) const;
    void dump_stream(std::ostream &out, const Json::Value &val) const;
    void dump_stream_full(std::ostream &out, const Json::Value &val) const;
    
    void dump_stream_list(std::ostream &out, const Json::Value &val) const;
    
    Json::Value _json_value;
    
    unsigned int _int_len;
    unsigned int _name_len;
    unsigned int _game_len;
    
    bool _descriptive;
    bool _section;
    bool _verbose;
};

class error_result : public result {
public:
    virtual void dump(std::ostream &out = std::cout) const override;
};

class bookmarks_result : public result {
public:
    virtual void dump(std::ostream &out = std::cout) const override;
};

class channels_result : public result {
public:
    virtual void dump(std::ostream &out = std::cout) const override;
};

class featured_streams_result : public result {
public:
    virtual void dump(std::ostream &out = std::cout) const override;
};

class search_channels_result : public result {
public:
    virtual void dump(std::ostream &out = std::cout) const override;
};

class search_games_result : public result {
public:
    virtual void dump(std::ostream &out = std::cout) const override;
};

class search_streams_result : public result {
public:
    virtual void dump(std::ostream &out = std::cout) const override;
};

class streams_result : public result {
public:
    virtual void dump(std::ostream &out = std::cout) const override;
};

class top_games_result : public result {
public:
    virtual void dump(std::ostream &out = std::cout) const override;
};

class users_result : public result {
public:
    virtual void dump(std::ostream &out = std::cout) const override;
};

#endif /* _QUERY_RESULTS_HPP_ */
