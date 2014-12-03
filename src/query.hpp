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
    explicit query();
    ~query();
    
    enum type {
        TYPE_CHANNELS,
        TYPE_FEATURED,
        TYPE_SEARCH_C,
        TYPE_SEARCH_G,
        TYPE_SEARCH_S,
        TYPE_STREAMS,
        TYPE_TOP,
    };
    
    void set_name(const std::string &str);
    void set_limit(unsigned int limit);
    void set_live(bool live);
    
    typedef std::pair<query::type, std::string> response;
    
    response get_response(query::type type);
private:
    url_client _client;
    std::string base_url(query::type type);
    
    std::string _name;
    unsigned int _limit;
    bool _live;
};

#endif /* _QUERY_HPP_ */