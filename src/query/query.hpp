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

#include <vector>
#include <string>

#include "url-client.hpp"

class query {
public:
    enum stream_type {
        STREAM_TYPE_ALL,
        STREAM_TYPE_PLAYLIST,
        STREAM_TYPE_LIVE,
    };
    
    query();
    
    std::string channels(const std::string &name);
    std::string featured_streams(unsigned int limit = 25, 
                                 unsigned int offset = 0);
    std::string search_channels(const std::string &query,
                                unsigned int limit = 25,
                                unsigned int offset = 0);
    std::string search_games(const std::string &query,
                             const bool *live = nullptr);
    std::string search_streams(const std::string &query,
                               unsigned int limit = 25,
                               unsigned int offset = 0,
                               const bool *hls = nullptr);
    
    std::string streams(const std::string *game = nullptr,
                        const std::vector<std::string> *channels = nullptr,
                        const unsigned int limit = 25,
                        const unsigned int offset = 0,
                        const std::string *client_id = nullptr,
                        enum stream_type *stream_type = nullptr);
    
    std::string top_games(unsigned int limit = 10, 
                          unsigned int offset = 0);
    
    
    std::string users(const std::string &name);
private:
    url_client _client;
    std::string _uri;
};

#endif /* _QUERY_HPP_ */