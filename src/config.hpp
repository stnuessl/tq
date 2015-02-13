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

#include "file.hpp"

class config : public file {
public:
    config(const std::string &path);
    
    unsigned int integer_length() const;
    unsigned int name_length() const;
    unsigned int game_length() const;
    
    const std::string &stream_opener() const;
    const std::vector<std::string> &stream_opener_args() const;
private:
    unsigned int _int_len;
    unsigned int _name_len;
    unsigned int _game_len;
    
    std::string _opener;
    std::vector<std::string> _args;
};

#endif /* _CONFIG_HPP_ */