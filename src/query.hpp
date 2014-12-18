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

#ifndef _QUERY_HPP_
#define _QUERY_HPP_

#include <string>
#include <utility>

#include "url-client.hpp"

class query {
public:
    enum type {
        TYPE_CHANNELS,
        TYPE_FEATURED,
        TYPE_SEARCH_C,
        TYPE_SEARCH_G,
        TYPE_SEARCH_S,
        TYPE_STREAMS,
        TYPE_TOP,
    };
    
    typedef std::pair<const query::type, const std::string> response;
    
    static const unsigned int default_limit;
    
    explicit query();
    
    response channels(const std::string &name);
    response featured(unsigned int limit = default_limit);
    response search_channels(const std::string &name, 
                             unsigned int limit = default_limit);
    response search_games(const std::string &name, bool live = true);
    response search_streams(const std::string &name,
                            unsigned int limit = default_limit);
    response streams(const std::string &name);
    response top(unsigned int limit = default_limit);
private:
    static const std::string valid_limit_str(unsigned int limit);

    url_client _client;
};

#endif /* _QUERY_HPP_ */