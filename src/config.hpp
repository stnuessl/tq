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

#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include <string>
#include <vector>
#include <unordered_map>

#include "file.hpp"

class config : public file {
public:
    config(const std::string &path);
    
    const std::string &client_id() const;
    unsigned int limit() const;
    
    bool live() const;
    bool json() const;
    bool verbose() const;
    bool descriptive() const;
    bool section() const;
    
    unsigned int integer_length() const;
    unsigned int name_length() const;
    unsigned int game_length() const;
    
    const std::string &stream_opener() const;
    const std::vector<std::string> &stream_opener_args() const;
    
    const std::unordered_map<std::string, std::string> &
    game_shortcut_map() const;
private:
    std::string _client_id;
    
    unsigned int _limit;
    
    bool _live;
    bool _json;
    bool _verbose;
    bool _descriptive;
    bool _section;
    
    unsigned int _int_len;
    unsigned int _name_len;
    unsigned int _game_len;
    
    std::string _opener;
    std::vector<std::string> _args;
    std::vector<std::string> _shortcuts;
    
    std::unordered_map<std::string, std::string> _shortcut_map;
};

#endif /* _CONFIG_HPP_ */
